from bonvivant.lib.goopy import functional

from django.contrib.auth.decorators import login_required
from django.db.models.fields import BLANK_CHOICE_DASH
from django.http import Http404, HttpResponseRedirect
from django import forms
from django.shortcuts import get_object_or_404, render_to_response
from django.template import RequestContext

import bonvivant.apps.recipes.models as recipes

class RecipeManipulator(forms.Manipulator):
    def __init__(self):
        servings_choices = functional.flatten1((BLANK_CHOICE_DASH,
                                                recipes.SERVING_CHOICES))

        self.fields = (
            forms.TextField(field_name='title',
                            length=30, maxlength=256,
                            is_required=True),
            forms.SelectField(field_name='servings',
                              choices=servings_choices),
            forms.TextField(field_name='prep_time',
                            length=8, maxlength=8,
                            is_required=False),
            forms.TextField(field_name='cooking_time',
                            length=8, maxlength=8,
                            is_required=False),

            forms.LargeTextField(field_name='instructions',
                                 rows=10, cols=80,
                                 is_required=True),

            forms.LargeTextField(field_name='notes',
                                 rows=10, cols=80,
                                 is_required=False),

            forms.SelectField(field_name='license',
                              choices=recipes.LICENSE_CHOICES),
        )

    def get_validation_errors(self, new_data, ingredients):
        errors = forms.Manipulator.get_validation_errors(self, new_data)
        i = 0
        for ingredient in ingredients:
            if type(ingredient) is recipes.Ingredient.BadIngredient:
                errors['ingredient_%d' % i] = ['Bad string']
            i += 1

        return errors

class AddRecipe(RecipeManipulator):
    def save(self, new_data, ingredients):
        r = recipes.Recipe(title        = new_data['title'],
                           author       = new_data['author'],
                           servings     = new_data['servings'],
                           prep_time    = new_data['prep_time'],
                           cooking_time = new_data['cooking_time'],
                           instructions = new_data['instructions'],
                           notes        = new_data['notes'],
                           license      = new_data['license'])
        r.save()

        for ingredient in ingredients:
            ingredient.recipe = r
            ingredient.save()

        return r

class ChangeRecipe(RecipeManipulator):
    def __init__(self, object_id):
        RecipeManipulator.__init__(self)

        self.original_object = recipes.Recipe.objects.get(id=object_id)

    def flatten_data(self):
        recipe = self.original_object
        data = {}
        data['title']        = recipe.title
        data['servings']     = recipe.servings
        data['prep_time']    = recipe.prep_time
        data['cooking_time'] = recipe.cooking_time
        data['instructions'] = recipe.instructions
        data['notes']        = recipe.notes
        data['license']      = recipe.license

        return data

    def save(self, new_data, ingredients):
        # Delete the old ingredients
        recipe.Ingredient.objects.filter(recipe=self.original_object).delete()

        self.original_object.title        = new_data['title']
        self.original_object.servings     = new_data['servings']
        self.original_object.prep_time    = new_data['prep_time']
        self.original_object.cooking_time = new_data['cooking_time']
        self.original_object.instructions = new_data['instructions']
        self.original_object.notes        = new_data['notes']
        self.original_object.license      = new_data['license']

        self.original_object.save()

        for ingredient in ingredients:
            ingredient.recipe = self.original_object
            ingredient.save()

        return self.original_object

def _gather_ingredients(data):
    i = 0
    results = []
    while True:
        try:
            string = data['ingredient_%d' % i]
            ingredient = recipes.Ingredient.construct(string)
        except ValueError, e:
            # Parse error
            print e
            ingredient = recipes.Ingredient.BadIngredient(string)
        except KeyError:
            # No more ingredients
            return results

        results.append(ingredient)

        if results[-1]:
            try:
                results[-1].optional = data['ingredient_opt_%d' % i]
            except KeyError:
                pass
        i += 1

@login_required
def new(request):
    manipulator = AddRecipe()

    if request.method == 'POST':
        new_data = request.POST.copy()

        # Fill in automatic fields
        new_data['author'] = request.user
        ingredients = [ing for ing in _gather_ingredients(new_data) if ing is not None]

        errors = manipulator.get_validation_errors(new_data, ingredients)

        if not errors:
            manipulator.do_html2python(new_data)
            new_recipe = manipulator.save(new_data, ingredients)
            return HttpResponseRedirect('/recipes/edit/%i' % new_recipe.id)

        ingredients += [None]*2
    else:
        # No POST, so we want a brand-new form
        new_data = {}
        errors = {}
        ingredients = [None]*3

    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('recipes/new.html',
                              {'form'        : form,
                               'ingredients' : ingredients},
                              context_instance=RequestContext(request))

@login_required
def edit(request, object_id):
    try:
        manipulator = ChangeRecipe(object_id)
    except recipes.Recipe.DoesNotExist:
        raise Http404

    recipe = manipulator.original_object

    if recipe.author.id != request.user.id:
        # FIXME: return a permission denied error page
        raise Http404

    if request.method == 'POST':
        new_data = request.POST.copy()
        ingredients = [ing for ing in _gather_ingredients(new_data) if ing is not None]
        errors = manipulator.get_validation_errors(new_data, ingredients)
        if not errors:
            manipulator.do_html2python(new_data)
            manipulator.save(new_data, ingredients)

            return HttpResponseRedirect('/recipes/edit/%i' % recipe.id)
    else:
        errors = {}
        new_data = manipulator.flatten_data()
        ingredients = list(recipes.Ingredient.objects.filter(recipe=recipe))

    ingredients += [None]*2

    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('recipes/edit.html',
                              {'form'        : form,
                               'recipe'      : recipe,
                               'ingredients' : ingredients},
                              context_instance=RequestContext(request))

def filterCBs(data):
    """
    Filter a text string to split it into commentable blocks.
    """
    return [data, ]

def detail(request, object_id):
    recipe = get_object_or_404(recipes.Recipe, pk=object_id)
    ingredients = recipes.Ingredient.objects.filter(recipe=recipe.id)

    instructions = filterCBs(recipe.instructions)
    notes = filterCBs(recipe.notes)

    comments = recipes.Comment.objects.filter(recipe=recipe.id)

    return render_to_response('recipes/detail.html',
                              {'recipe'       : recipe,
                               'ingredients'  : ingredients,
                               'instructions' : instructions,
                               'notes'        : notes,
                               'comments'     : comments},
                              context_instance=RequestContext(request))
