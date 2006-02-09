#
# Experimental ruleset compiler bits for CIA
# Copyright (C) 2006 Micah Dowty
#
# TODO: Fix inequality representation and optimization,
#       validate against stats://author
#

from LibCIA import XML, Formatters
import binascii, cPickle

class Variable:
    """A Variable is a quantity which can be derived from the
       original XML message.
       """
    MATCH_HEX = 1
    MATCH_LOWER = 2
    FIND_EXACT = 3
    FIND_CASE_INSENSITIVE = 4
    
    def __init__(self, op, path=None, arg=None):
        self.op = op
        self.path = path
        self.arg = arg

    def __repr__(self):
        # A very terse representation that leaves out
        # most of the variable's information, but makes
        # long Equations much more readable
        lastPathSeg = self.path.split("/")[-1]
        if self.arg:
            return "%s[%s]" % (lastPathSeg, self.arg)
        else:
            return lastPathSeg

class VariableSet:
    """A container for Variables. One VariableSet is shared among all
       rulesets during compilation.
       """
    def __init__(self):
        self._vars = {}

    def get(self, *args):
        if args in self._vars:
            return self._vars[args]
        var = Variable(*args)
        self._vars[args] = var
        return var

class Term(object):
    """A Term in a logic equation- an object which operates upon a
       variable in order to yield a True or False value.

       For bool variables, this is classic Boolean logic. A term
       takes the form 'var' or '!var'.

       For string variables, this gets a little more complicated.
       A term typically takes the form 'var == constant' but,
       in order to efficiently support equation optimizations,
       the inverted form looks like 'var not in list'.

       Boolean variables should have an empty list of constants.
       String variables with polarity==True must have exactly
       one constant, string variables with polarity==False
       must have at least one constant.
       """
    def __init__(self, variable, polarity=True, *constants):
        # Make constants unique and sorted
        cdict = {}
        for c in constants:
            cdict[c] = None
        constants = cdict.keys()
        constants.sort()

        self.variable = variable
        self.polarity = polarity
        self.constants = tuple(constants)

    def invert(self):
        """Returns a minterm sequence describing the
           inverse of this term.
           """
        if not self.constants:
            # Booleans just get inverted polarity
            return (Term(self.variable, not self.polarity),)
        if self.polarity:
            # A single inequality
            assert len(self.constants) == 1
            return (Term(self.variable, False, self.constants[0]),)
        else:
            return [Term(self.variable, True, c) for c in self.constants]

    def combine(self, other):
        """Try to reduce two Terms of the same variable.
           This is an AND operation, returning a Term or
           returning None of the result is always false.
           """
        assert self.variable is other.variable

        # Are these terms entirely equal?
        if self == other:
            return self

        # If this is a boolean variable, now we know it's
        # a contradiction.
        if not self.constants:
            assert not other.constants
            return None

        if self.polarity and other.polarity:
            # Two equalities. We know they aren't equal, so
            # this is a contradiction.
            return None

        if not (self.polarity or other.polarity):
            # Two inequalities, merge the lists
            return Term(self.variable, False, *(
                self.constants + other.constants))

        # By now we know that we have one equal
        # and one inequal term.
        if self.polarity:
            equal, inequal = self, other
        else:
            equal, inequal = other, self

        assert len(equal.constants) == 1
        if equal.constants[0] in inequal.constants:
            # We have a contradiction
            return None
        else:
            # The equality already implies all the
            # inequalities. Discard the latter.
            return equal
        
    def __cmp__(self, other):
        return cmp((self.variable, self.polarity, self.constants),
                   (other.variable, other.polarity, other.constants))

    def __hash__(self):
        return hash((self.variable, self.polarity, self.constants))

    def __repr__(self):
        if self.constants:
            if self.polarity:
                assert len(self.constants) == 1
                return "%r=%s" % (self.variable, self.constants[0])
            else:
                return "%r!%s" % (self.variable, "/".join(self.constants))
        else:
            if self.polarity:
                return repr(self.variable)
            else:
                return "~%r" % self.variable

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
  
       Equations should be treated as immutable objects. They
       can be safely used as dictionary keys. All operations
       on an Equation will result in a new instance.
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
            raise TypeError("Invalid Equation argument %r" % obj)

        self._hash = None
        self._key = None

    def __repr__(self):
        if self._sums == {}:
            return "False"
        elif self._sums == {(): None}:
            return "True"
        return " | ".join(["(%s)" % " & ".join(map(repr, terms))
                           for terms in self._sums.iterkeys()])

    def _getKey(self):
        # Return a hashable object that uniquely identifies this Equation
        if self._key is None:
            s = self._sums.keys()
            s.sort()
            self._key = tuple(s)
        return self._key

    def __cmp__(self, other):
        return cmp(self._getKey(), other._getKey())

    def __hash__(self):
        # Cache the hash
        if self._hash is None:
            self._hash = hash(self._getKey())
        return self._hash

    def __or__(self, other):
        if not isinstance(other, Equation):
            raise TypeError("Can't OR an Equation with %r" % other)

        # Concatenate the minterm list, automatically
        # overwriting any duplicated terms.
        new = dict(self._sums)
        new.update(other._sums)

        # If we have any True minterms, the entire
        # equation is always True.
        if () in new:
            return Equation(True)

        self._optimizeMinterms(new)
        return Equation(new)

    def _mintermImplies(self, mt1, mt2):
        """Returns True if the first minterm implies the second-
           the second minterm is unnecessary given the first.
           Both minterms must be sorted tuples.

           This is effectively a string matching problem:
           Find mt1 inside mt2.
           """
        # Naieve string matching, for now
        l1 = len(mt1)
        l2 = len(mt2)
        for i in range(l2 - l1):
            if mt2[i:i+l1] == mt1:
                return True
        return False

    def _optimizeMinterms(self, d):
        """Remove unnecessary minterms from the provided dict"""
        # XXX: the O() of this is disgusting, but
        #      it's much better than letting the minterm
        #      list fill with garbage.
        #
        # We sort the terms by length, then
        # run _mintermImplies on pairs with
        # dissimilar lengths.
        sorted = [(len(k), k) for k in d.iterkeys()]
        sorted.sort()

        for i in xrange(len(sorted)):
            si = sorted[i][1]
            if si not in d:
                continue
            for j in xrange(i+1, len(sorted)):
                sj = sorted[j][1]
                if sj not in d:
                    continue

                if self._mintermImplies(si, sj):
                    del d[sj]

    def __and__(self, other):
        if not isinstance(other, Equation):
            raise TypeError("Can't AND an Equation with %r" % other)
        new = {}

        # Polynomial multiplication: take the cartesian product
        # of the two minterm lists.
        for mt1 in self._sums:
            for mt2 in other._sums:
                combined = self._combineMinterms(mt1, mt2)
                if combined is not None:
                    new[combined] = None

        self._optimizeMinterms(new)
        return Equation(new)

    def _combineMinterms(self, mt1, mt2):
        """Multiply the Terms in two minterm lists. Returns
           a new sorted tuple, or returns None if the result
           is always False.
           """
        # Storage for the minterms we're combining: a dict
        # maps variables to terms
        varMap = {}

        # Populate it with the first minterm list. We're assuming
        # that it's already well-formed and we don't need to
        # check for contradictions or duplicates.
        for term in mt1:
            varMap[term.variable] = term

        # Now check the other terms, using varMap to limit
        # our search to terms with the same variable.
        for term in mt2:
            if term.variable not in varMap:
                varMap[term.variable] = term
            else:
                v = varMap[term.variable].combine(term)
                if v is None:
                    # Contradiction- the whole thing is always false
                    return None
                assert term.variable is v.variable
                varMap[term.variable] = v

        # Flatten back into a sorted tuple
        t = varMap.values()
        t.sort()
        return tuple(t)

    def __invert__(self):
        # We can negate this equation by DeMorgan's theorem:
        # negate all terms, and exchange AND and OR.
        products = Equation(True)
        for mt in self._sums:
            sums = Equation(False)
            for term in mt:
                for iterm in term.invert():
                    sums = sums | Equation(iterm)
            products = products & sums
        return products

class XMLEquation(XML.XMLObjectParser):
    """This object decodes an XML representation of a logic equation.
       It isn't useful on its own, but subclasses can define terms in
       order to yield meaningful equations.

       This supports <and>, <or>, and <not> containers.
       <true/> and <false/> are provided as constants.
       Note that <not> typically only has a single argument,
       but it's actually implemented as a NOR function. This
       is intuitive, since all children must evaluate to
       False for it to be true.
       """
    resultAttribute = 'equation'
    
    def element_and(self, element):
        """Evaluates to True if and only if all children evaluate to True"""
        eq = Equation(True)
        for child in self.childParser(element):
            if child is not None:
                eq = eq & child
        return eq

    def element_or(self, element):
        """Evaluates to True if any children evaluate to True"""
        eq = Equation(False)
        for child in self.childParser(element):
            if child is not None:
                eq = eq | child
        return eq

    def element_not(self, element):
        """The NOR function, returns True if and only if
           every child evaluates to False.
           """
        eq = Equation(True)
        for child in self.childParser(element):
            if child is not None:
                eq = eq & ~child
        return eq

    def _noChildren(self, element):
        for child in node.childNodes:
            raise XML.XMLValidityError(
                "Child elements are not allowed below %r" % element.name)

    def element_true(self, element):
        self._noChildren(element)
        return Equation(True)

    def element_false(self, element):
        self._noChildren(element)
        return Equation(False)

class MessageFilter(XMLEquation):
    """This is a parser for CIA's XML message filters, resulting in an
       Equation instance. Uses the provided VariableSet instance to
       define terms.
       """
    def __init__(self, doc, vars):
        self.vars = vars
        XMLEquation.__init__(self, doc)

    def _getXPath(self, element):
        p = element.getAttributeNS(None, 'path')
        if not p:
            raise XML.XMLValidityError(
                "The %r element has a required 'path' attribute" %
                element.name)
        return p

    def _isCaseSensitive(self, element):
        cs = element.getAttributeNS(None, 'caseSensitive')
        return cs and int(cs)

    def element_match(self, element):
        text = XML.shallowText(element).strip()

        if self._isCaseSensitive(element):
            # SQL isn't guaranteed to be case sensitive, compare a
            # hex encoded version of the string.
            return Equation(Term(
                self.vars.get(Variable.MATCH_HEX,
                              self._getXPath(element)),
                True,
                binascii.b2a_hex(text)))
        else:
            # On the other hand, we're not guaranteeing that
            # SQL will be case insensitive either. Lowercase the
            # string first.
            return Equation(Term(
                self.vars.get(Variable.MATCH_LOWER,
                              self._getXPath(element)),
                True,
                text.lower()))

    def element_find(self, element):
        text = XML.shallowText(element).strip()
        if self._isCaseSensitive(element):
            op = Variable.FIND_EXACT
        else:
            op = Variable.FIND_CASE_INSENSITIVE
        return Equation(Term(
            self.vars.get(op, self._getXPath(element), text),
            True))

class RulesetOutcomes:
    """This object tracks a list of potential execution
       paths. Each execution path is described by a logic
       equation indicating its preconditions, and a stack
       of formatters to apply.
       """
    def __init__(self):
        # We have three dictionaries tracking execution
        # paths. 'active' paths are running right now,
        # and are operated on by formatters and by normal
        # control flow. 'retired' paths are permanently
        # inactive due to a 'return' operation. Finally,
        # 'scopes' track paths that are being suspended
        # by 'break'.
        self._active = {Equation(True): []}
        self._retired = {}
        self._scopes = []

    def enterScope(self):
        self._scopes.append({})

    def leaveScope(self):
        """Reactivate all threads of execution that
           were suspended by break() since the last
           enterScope.
           """
        self._active.update(self._scopes.pop())

    def pushFormatter(self, formatter):
        """To all active execution paths at this point,
           append a new formatter description.
           """
        for formatList in self._active.itervalues():
            formatList.append(formatter)

    def clearFormatters(self):
        """Remove all existing formatters"""
        for formatList in self._active.itervalues():
            del formatList[:]

    def _suspendUnless(self, condition, dest):
        """Move all active threads of execution over to
           the supplied destination scope iff condition==False.
           """
        activeItems = self._active.items()
        self._active = {}
        notCondition = ~condition
        for e, f in activeItems:
            enc = e & notCondition
            if enc != Equation(False):
                dest[enc] = list(f)
            ec = e & condition
            if ec != Equation(False):
                self._active[ec] = f

    def suspendScopeUnless(self, condition):
        """Suspend all active execution paths, when condition
           is False, until the end of the current scope.
           """
        self._suspendUnless(condition, self._scopes[-1])

    def suspendAll(self):
        """Permanently suspend all active execution paths."""
        self._suspendUnless(Equation(False), self._retired)


class RulesetParser(XML.XMLObjectParser):
    """This object recursively parses CIA's rulesets. A ruleset
       consists of various formatting actions, the application of
       which is controlled by embedded Filter trees.

       This will evaluate all possible execution paths in a ruleset,
       using the RulesetOutcomes object.
       """
    requiredRootElement = "ruleset"

    def __init__(self, doc, vars):
        self.vars = vars
        XML.XMLObjectParser.__init__(self, doc)

    def element_ruleset(self, element):
        """<ruleset> for the most part works just like <rule>, but since
           it's the root node it's responsible for initialization.
           """
        # Go ahead and store the URI attribute if we have one.
        # If not, this will be None.
        self.uri = element.getAttributeNS(None, 'uri') or None

        # URIs are always encoded if necessary, since just about everywhere
        # we'd need to use a URI we can't support Unicode yet. Specific
        # examples are IRC servers/channels
        # and as dict keys in an XML-RPC response.
        if type(self.uri) is unicode:
            self.uri = self.uri.encode()

        self.outcomes = RulesetOutcomes()
        self.element_rule(element)

        # Join all active threads of execution and return them
        self.outcomes.suspendAll()
        return self.outcomes._retired

    def element_rule(self, element):
        """Evaluate each child element in sequence until one returns False"""
        self.outcomes.enterScope()
        for child in self.childParser(element):
            pass
        self.outcomes.leaveScope()

    def element_return(self, element):
        """Set the current result and exit the ruleset immediately"""
        path = element.getAttributeNS(None, 'path')
        const = XML.shallowText(element)

        self.outcomes.clearFormatters()
        if path:
            self.outcomes.pushFormatter(('returnPath', path))
        elif const:
            self.outcomes.pushFormatter(('returnConst',
                                         XML.shallowText(element)))

        self.outcomes.suspendAll()

    def element_break(self, element):
        """Just exit the ruleset immediately, preserving the current
           formatter result
           """
        self.outcomes.suspendAll()

    def element_formatter(self, element):
        """Creates a Formatter instance matching the element's description,
           returns a function that applies the formatter against the current
           message and result.
           """
        # Evaluate this once at parse-time so any silly errors
        # like unknown formatters or mediums can be detected.
        Formatters.getFactory().fromXml(element)

        self.outcomes.pushFormatter(('format', XML.toString(element)))

    def unknownElement(self, element):
        # Unknown ruleset elements should be filters
        self.outcomes.suspendScopeUnless(
            MessageFilter(element, self.vars).equation)


if __name__ == "__main__":
    vs = VariableSet()
    if 0:
        a = Equation(Term(vs.get('a')))
        b = Equation(Term(vs.get('b')))
        c = Equation(Term(vs.get('c')))

        e = Equation(False)
        for i in xrange(50):
            v = Equation(Term(vs.get('v'), True, str(i)))
            e = e | (a & b & v)

        print "e = %s" % e
        for i in xrange(20):
            print "~e = %s" % ~e

            
    else:
        rulesets = cPickle.load(open("rulesets.pickle"))

        for r in rulesets:
            if r.find("stats://author")>=0:
                continue
            print
            print "============================================="
            print
            print r
            print
            print "---------------------------------------------"
            for k, v in RulesetParser(r, vs).result.iteritems():
                if v:
                    print
                    print k
                    print
                    for f in v:
                        print "\t" + str(f)
        
