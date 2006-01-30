
class Column:
    """Represents one column in the final tabular model of the ruleset list.
       This class tracks a usage count for the column, so unnecessary
       columns can be excluded from the model and the order of column
       evaluation can be optimized.
       """
    def __init__(self):
        self.name = None
        self.useCount = 0

    def ref(self):
        """Increase this column's use count, and return it"""
        self.useCount += 1
        return self

class Variable:
    """A Variable is a quantity which can be derived from the original
       XML message. The variable may be a string (the text resulting
       from an xpath match) or it may be a boolean indicating the result
       of a test that can't be performed in SQL, like a <find>.

       Each variable is modeled with two Columns, one of which is tested
       to be equal, the other of which is tested to be inequal. This lets
       us negate variables that may not be boolean.

       The method of deriving a variable from the XML message is called
       the variable's 'recipe'.
       """
    def __init__(self, recipe):
        self.recipe = recipe
        self.equal = Column()
        self.inequal = Column()
        self.columns = (self.equal, self.inequal)

    def __repr__(self):
        return str(self.recipe)


class VariableSet:
    """A container for Variables. One VariableSet is shared among all
       rulesets during compilation.
       """
    def __init__(self):
        self._recipes = {}

    def get(self, recipe):
        if recipe in self._recipes:
            return self._recipes[recipe]
        var = Variable(recipe)
        self._recipes[recipe] = var
        return var

class Term(object):
    """A Term in a logic equation. Every term is of the form
       'variable == constant' or 'variable != constant'.
       """
    def __init__(self, variable, constant, equal=True):
        self.variable = variable
        self.constant = constant
        self.equal = equal

    def __invert__(self):
        return Term(self.variable, self.constant, not self.equal)

    def __cmp__(self, other):
        return cmp((id(self.variable), self.constant, self.equal),
                   (id(other.variable), other.constant, other.equal))

    def __hash__(self):
        return hash((id(self.variable), self.constant, self.equal))

    def __repr__(self):
        return "(%r %s %r)" % (
            self.variable, ('!=', '==')[self.equal], self.constant)

    def _getColumn(self):
        if self.equal:
            return self.variable.equal
        else:
            return self.variable.inequal

    column = property(_getColumn)


class Equation:
    """A combinational logic equation, specified as a sum of
       products. Equations can be created in three ways:

       - Equation(True) or Equation(False) always evaluate
         to True or False, respectively.

       - Equation(term) evaluates the Term instance

       - The &, |, and ~ operators can be used to combine
         equations.

       - Equation(list) creates the Equation from a raw
         sum-of-products list. This is actually represented
         as a dictionary of sums, in which each key is a sorted
         tuple of products. The dict values are unused and None.
       """
    def __init__(self, obj):
        if type(obj) is bool:
            if obj:
                # Always true: a single minterm where all terms
                # are "Don't care", therefore excluded from the dict.
                self._sums = {(): None}
            else:
                # Always false: no minterms.
                self._sums = {}

        elif isinstance(obj, Term):
            self._sums = {(obj,): None}

        elif type(obj) is dict:
            self._sums = obj

        else:
            raise TypeError

    def __repr__(self):
        if self._sums == {}:
            return "<0>"
        elif self._sums == {(): None}:
            return "<1>"
        return "<%s>" % " + ".join(["*".join(map(repr, terms))
                                    for terms in self._sums.iterkeys()])

    def __or__(self, other):
        if not isinstance(other, Equation):
            raise TypeError

        # Concatenate the minterm list, automatically
        # overwriting any duplicated terms.
        new = dict(self._sums)
        new.update(other._sums)

        # If we have any True minterms, the entire
        # equation is always True.
        if () in new:
            return Equation(True)

        return Equation(new)

    def __and__(self, other):
        if not isinstance(other, Equation):
            raise TypeError
        new = {}

        # Polynomial multiplication: take the cartesian product
        # of the two minterm lists.
        for mt1 in self._sums:
            for mt2 in other._sums:
                combined = self._combineMinterms(mt1, mt2)
                if combined is not None:
                    new[combined] = None

        return Equation(new)

    def _combineMinterms(self, mt1, mt2):
        """Multiply the Terms in two minterm lists. Returns
           a new sorted tuple, or returns None if the result
           is always False.
           """
        # Storage for the minterms we're combining: a dict
        # maps variables to a list of terms containing that
        # variable
        varMap = {}

        # Populate it with the first minterm list. We're assuming
        # that it's already well-formed and we don't need to
        # check for contradictions or duplicates.
        for term in mt1:
            if term.variable not in varMap:
                varMap[term.variable] = [term]
            else:
                varMap[term.variable].append(term)

        # Now check the other terms, using varMap to limit
        # our search to terms with the same variable.
        for term in mt2:
            if term.variable not in varMap:
                varMap[term.variable] = [term]
            else:
                # We have other terms to check against this one
                termList = varMap[term.variable]
                if term in termList:
                    # Exact duplicate. Ignore it.
                    pass
                elif term.equal:
                    if termList[0].equal:
                        # We already have a single value this variable
                        # must equal. This means the entire minterm list
                        # is always False. Note that duplicates are already
                        # weeded out, so we can assume at this point that
                        # there's a conflict.
                        assert len(termList) == 1
                        assert termList[0].constant != term.constant
                        return None
                    else:
                        # We had a list of inequalities, but the equality
                        # we're adding either conflicts with the inequalities
                        # or it replaces them all.
                        for oldTerm in termList:
                            assert not oldTerm.equal
                            if oldTerm.constant == term.constant:
                                # An old inequality conflicts
                                # with the new equality
                                return None
                        del termList[:]
                        termList.append(term)
                else:
                    if termList[0].equal:
                        # We're adding a new equality, but there's
                        # always a single equality in the term list.
                        # As long as there isn't a conflict, the new
                        # term can be ignored.
                        if termList[0].constant == term.constant:
                            return None
                    else:
                        # We're adding an inequality to an existing list
                        # of inequalities. Furthermore, we know it's not
                        # a duplicate. We can just add the new term.
                        termList.append(term)

        # Flatten back into a sorted tuple
        t = []
        map(t.extend, varMap.itervalues())
        t.sort()
        return tuple(t)

    def __invert__(self):
        # We can negate this equation by DeMorgan's theorem:
        # negate all terms, and exchange AND and OR.
        products = Equation(True)
        for mt in self._sums:
            sums = Equation(False)
            for term in mt:
                sums = sums | Equation(~term)
            products = products & sums
        return products


if __name__ == "__main__":
    vs = VariableSet()

    a = Equation(Term(vs.get('a'), 1))
    b = Equation(Term(vs.get('b'), 2))
    c = Equation(Term(vs.get('c'), 3))
    d = Equation(Term(vs.get('d'), 4))
        
    print (a|b) & (c|d)


    print (
        (Equation(Term(vs.get('project'), 'foo')) |
         Equation(Term(vs.get('project'), 'bar')) |
         Equation(Term(vs.get('project'), 'foo')) |
         Equation(Term(vs.get('project'), 'bar')) |
         Equation(Term(vs.get('project'), 'foo')) |
         Equation(Term(vs.get('project'), 'squish')) |
         Equation(Term(vs.get('project'), 'foo')))
        & Equation(Term(vs.get('author'), 'x'))
        & Equation(Term(vs.get('game'), 'y'))
        & Equation(Term(vs.get('author'), 'x'))
        | Equation(Term(vs.get('destination'), 'moon'))
        )

    print (
        ~Equation(Term(vs.get('project'), 'x')) &
        Equation(Term(vs.get('project'), 'x'))
        )

    print (a&b) | (c&d)
    print ~( (a&b) | (c&d) )
