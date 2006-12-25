from bonvivant.recipes.models import Ingredient, Recipe, Comment
from django.contrib.auth.decorators import login_required
from django.http import Http404, HttpResponseRedirect
from django import forms
from django.shortcuts import get_object_or_404, render_to_response
from django.template import RequestContext

@login_required
def new(request):
    manipulator = Recipe.AddManipulator()

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
        manipulator = Recipe.ChangeManipulator(object_id)
    except Recipe.DoesNotExist:
        raise Http404

    recipe = manipulator.original_object

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
                              {'form'   : form,
                               'recipe' : recipe},
                              context_instance=RequestContext(request))

def filterCBs(data):
    """
    Filter a text string to split it into commentable blocks.
    """
    return [data, ]

def detail(request, object_id):
    recipe = get_object_or_404(Recipe, pk=object_id)
    ingredients = Ingredient.objects.filter(recipe=recipe.id)

    instructions = filterCBs(recipe.instructions)
    notes = filterCBs(recipe.notes)

    comments = Comment.objects.filter(recipe=recipe.id)

    return render_to_response('recipes/detail.html',
                              {'recipe'       : recipe,
                               'ingredients'  : ingredients,
                               'instructions' : instructions,
                               'notes'        : notes,
                               'comments'     : comments},
                              context_instance=RequestContext(request))
