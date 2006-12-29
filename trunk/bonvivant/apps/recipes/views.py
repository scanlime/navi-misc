from bonvivant.lib.goopy import functional

from django.contrib.auth.decorators import login_required
from django.db.models.fields import BLANK_CHOICE_DASH
from django.http import Http404, HttpResponseRedirect
from django import forms
from django.shortcuts import get_object_or_404, render_to_response
from django.template import RequestContext

import bonvivant.apps.recipes.models as recipes

class AddRecipe(forms.Manipulator):
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

        return errors

    def save(self, new_data):
        pass

def _gather_ingredients(data):
    i = 0
    results = []
    try:
        while True:
            results.append(data['ingredient_%d' % i].strip())
            i += 1
    except KeyError:
        pass
    return results

@login_required
def new2(request):
    manipulator = AddRecipe()

    if request.method == 'POST':
        new_data = request.POST.copy()

        # Fill in automatic fields
        new_data['author'] = request.user.id
        ingredients = _gather_ingredients(new_data)

        errors = manipulator.get_validation_errors(new_data, ingredients)

        ingredients = [recipes.Ingredient.construct(data) for data in ingredients]

        if not errors:
            manipulator.do_html2python(new_data)
            new_recipe = manipulator.save(new_data, ingredients)
            return HttpResponseRedirect('/recipes/edit/%i' % new_recipe.id)
    else:
        # No POST, so we want a brand-new form
        new_data = {}
        errors = {}
        ingredients = [recipes.Ingredient(amount=0)]*3

    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('recipes/new.html',
                              {'form'        : form,
                               'ingredients' : ingredients},
                              context_instance=RequestContext(request))

@login_required
def new(request):
    manipulator = recipes.Recipe.AddManipulator()

    if request.method == 'POST':
        new_data = request.POST.copy()

        # Fill in automatic fields
        new_data['author'] = request.user.id

        errors = manipulator.get_validation_errors(new_data)

        if not errors:
            manipulator.do_html2python(new_data)
            new_recipe = manipulator.save(new_data)
            return HttpResponseRedirect('/recipes/edit/%i' % new_recipe.id)
    else:
        # No POST, so we want a brand-new form
        new_data = {}
        errors = {}

    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('recipes/new.html',
                              {'form' : form},
                              context_instance=RequestContext(request))

@login_required
def edit(request, object_id):
    try:
        manipulator = recipes.Recipe.ChangeManipulator(object_id)
    except recipes.Recipe.DoesNotExist:
        raise Http404

    recipe = manipulator.original_object

    ingredients = list(recipes.Ingredient.objects.filter(recipe=recipe.id))

    if recipe.author.id != request.user.id:
        # FIXME: return a permission denied error page
        raise Http404

    if request.method == 'POST':
        new_data = request.POST.copy()
        errors = manipulator.get_validation_errors(new_data)
        if not errors:
            manipulator.do_html2python(new_data)
            manipulator.save(new_data)

            return HttpResponseRedirect('/recipes/edit/%i' % recipe.id)
    else:
        errors = {}
        new_data = manipulator.flatten_data()

    form = forms.FormWrapper(manipulator, new_data, errors)
    return render_to_response('recipes/edit.html',
                              {'form'        : form,
                               'recipe'      : recipe,
                               'ingredients' : ingredients + [recipes.Ingredient(amount=0)]*3},
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
