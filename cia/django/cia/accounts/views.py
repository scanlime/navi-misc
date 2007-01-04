from django.shortcuts import render_to_response
from django.http import HttpResponse

def index(request):
    foo = 123
    return render_to_response('layout_account.html', locals())

def bot(request, bot_id):
    return HttpResponse("You've reached bot number %d. Please hold." % int(bot_id)) 

