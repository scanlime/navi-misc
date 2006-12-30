from bonvivant.lib import human
from bonvivant.lib.gourmet import convert

from django.db import models
from django.contrib.auth.models import User

LICENSE_CHOICES = (
    ('by-nc-sa', 'Creative Commons Attribution Non-commercial Share Alike'),
    ('by-nc',    'Creative Commons Attribution Non-commercial'),
    ('by-sa',    'Creative Commons Attribution Share Alike'),
    ('by',       'Creative Commons Attribution'),
    ('pd',       'Public Domain'),
    ('c',        'Copyright Only'),
)

SERVING_CHOICES = tuple([('%s' % i, i) for i in range(1, 11)])

class Recipe(models.Model):
    title = models.CharField(maxlength=256)
    author = models.ForeignKey(User, related_name='authored_recipes')

    servings = models.IntegerField(blank=True, null=True, choices=SERVING_CHOICES, default=1)

    prep_time = models.TimeField('Preparation Time', blank=True, null=True)
    cooking_time = models.TimeField('Cooking Time', blank=True, null=True)

    instructions = models.TextField()
    notes = models.TextField(blank=True)

    ctime = models.DateTimeField('Date Created', auto_now_add=True)
    mtime = models.DateTimeField('Date Published', auto_now=True)

    license = models.CharField(maxlength=8, choices=LICENSE_CHOICES, default='by-sa')

    slug = models.SlugField(prepopulate_from=("title",), unique=True)
    public = models.BooleanField(default=False)

    # Recipes can be pulled into an individual user's "recipe box"
    box = models.ManyToManyField(User, related_name='recipe_box', blank=True, null=True)

    def __str__(self):
        return '"%s" by %s' % (self.title, self.author)

    def get_absolute_url(self):
        return '/recipes/%i/' % self.id

    class Admin:
        fields = (
            ('Metadata', {'fields' : ('title',
                                      'author',
                                      'license')}),
            ('Recipe',   {'fields' : ('servings',
                                      'prep_time',
                                      'cooking_time',
                                      'instructions',
                                      'notes')}),
        )

class Ingredient(models.Model):
    amount = models.FloatField(max_digits=8, decimal_places=4, core=True, blank=True)
    unit = models.CharField(maxlength=20, blank=True) # FIXME: add choices
    item = models.CharField(maxlength=128, core=True)
    optional = models.BooleanField(core=True)

    recipe = models.ForeignKey(Recipe, edit_inline=models.TABULAR, min_num_in_admin=5)

    conv = convert.converter()

    def __str__(self):
        str = '%f-%s-%s' % (self.amount, self.unit, self.item)
        if self.optional:
            str = '%s (optional)' % str
        return str

    def write(self, includeOptional=False):
        parts = []
        if self.amount != 0:
            parts.append(human.prettynumber(self.amount))
        if len(self.unit):
            parts.append(self.unit)
        if self.item:
            parts.append(self.item)
        if includeOptional and self.optional:
            parts.append("(optional)")
        return ' '.join(parts)

    def write_o(self):
        return self.write(True)

    def error(self):
        return False

    class BadIngredient:
        def __init__(self, string):
            self.value = string
            self.optional = False

        def write(self):
            return self.value

        def error(self):
            return True

    @staticmethod
    def construct(string):
        """
        Construct an Ingredient given a string.  This code is derived from gourmet.

        >>> Ingredient.construct('2 eggs')
        <Ingredient: 2.000000-None-eggs>

        >>> Ingredient.construct('1 1/2 cups ketchup')
        <Ingredient: 1.500000-cups-ketchup>

        >>> Ingredient.construct('2 tbsp. grated cheese')
        <Ingredient: 2.000000-tbsp.-grated cheese>
        """

        # FIXME: hardcoding utf-8 here sucks!
        s = string.decode('utf-8')
        s = s.strip("\n\t #*+-")

        if len(s) == 0:
            return None

        m = convert.ING_MATCHER.match(s)

        if m:
            a, u, i = (m.group(convert.ING_MATCHER_AMT_GROUP),
                       m.group(convert.ING_MATCHER_UNIT_GROUP),
                       m.group(convert.ING_MATCHER_ITEM_GROUP))

            amount = 0
            unit = ''
            item = None

            if a:
                amount = convert.frac_to_float(a.strip())

                print '"%s" = %f\n' % (a, amount)

            if u:
                if Ingredient.conv and Ingredient.conv.unit_dict.has_key(u.strip()):
                    # Don't convert units to our units!
                    unit = u.strip()
                else:
                    # FIXME: previous uses?
                    # unit is not a unit
                    ' '.join((u, i))

            if i:
                item = i
            else:
                raise ValueError("Unable to parse ingredient")

            return Ingredient(amount=float(amount), unit=unit, item=item)

        else:
            print 'no match for %s' % s
            raise ValueError("Unable to parse ingredient")


class Comment(models.Model):
    author = models.ForeignKey(User)
    recipe = models.ForeignKey(Recipe)

    block = models.CharField(maxlength=10)

    body = models.TextField()

    submit_date = models.DateTimeField(_('date/time submitted'), auto_now_add=True)

    def __str__(self):
        body = self.body[:20]
        if len(self.body) > 20:
            body += '...'
        return '"%s" by %s on %s' % (body, self.author, self.recipe.title)

    class Admin:
        pass
