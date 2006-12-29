from django.template import Library, Node

register = Library()

class CounterNode(Node):
    def __init__(self, start = 0, variable_name = None):
        self.counter = start
        self.variable_name = variable_name

    def render(self, context):
        value = self.counter
        if self.variable_name:
            context[self.variable_name] = value
        self.counter += 1
        return '%d' % value

@register.tag
def counter(parser, token):
    """
    Kind of like django's "cycle" tag, but increments a counter.

    The syntax is: {% counter x %} or {% counter x from 4 %}

    This tag will always return an integer in a string.  If the counter has
    not been used before and does not contain a "from" field, it will
    initialize at 0.

    Using the "from" syntax will reset the counter to the given value,
    regardless of whatever value it held previously.
    """

    if not hasattr(parser, '_namedCounterNodes'):
        parser._namedCounterNodes = {}

    args = token.contents.split()
    if len(args) < 2:
        raise TemplateSyntaxError("'counter' statement requires at least two arguments")
    elif len(args) == 2:
        name = args[1]

        try:
            node = parser._namedCounterNodes[name]
        except KeyError:
            node = CounterNode(variable_name = name)
            parser._namedCounterNodes[name] = node

        return node
    elif len(args) == 4:
        if args[2] != 'from':
            raise TemplateSyntaxError("Second 'counter' argument must be 'from'")

        name = args[1]
        default = int(args[3])
        node = CounterNode(default, name)

        parser._namedCounterNodes[name] = node

        return node
    else:
        raise TemplateSyntaxError("Invalid arguments to 'counter': %s" % args)
