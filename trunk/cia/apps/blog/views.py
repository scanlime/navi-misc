from django.shortcuts import render_to_response
from django.template.context import RequestContext
from django.contrib.syndication.feeds import Feed
from django.contrib.comments.views.comments import post_free_comment
from django.contrib.comments.models import FreeComment
from django.http import HttpResponse, Http404, HttpResponseRedirect
from django.template.defaultfilters import slugify
from django.conf import settings
from cia.apps.blog.models import Post
import django.newforms as forms
import datetime

def is_blog_admin(request):
    return request.user.is_authenticated() and request.user.is_staff

def archive(request, num_latest=15, year=None, month=None):
    can_post = is_blog_admin(request)
    if can_post:
        posts = Post.objects.all()
    else:
        posts = Post.objects.filter(
            pub_date__lte = datetime.datetime.now(),
            listed = True)
    
    # Filter by a month/year
    latest = posts
    if year and month:
        latest = latest.filter(pub_date__year = int(year),
                               pub_date__month = int(month))
        current_archive_date = datetime.datetime(year = int(year),
                                                 month = int(month),
                                                 day = 1)
    else:
        current_archive_date = None

    latest = latest.order_by('-pub_date')[:num_latest]
    archive_dates = posts.dates('pub_date', 'month')[::-1]

    return render_to_response('blog/archive.html', RequestContext(request, locals()))


class EditPostForm(forms.Form):
    title = forms.CharField()
    listed = forms.BooleanField(
        required = False,
        widget = forms.CheckboxInput(attrs = {'class': 'checkbox'}),
        )
    content = forms.CharField(
        widget = forms.Textarea,
        )

def detail(request, year=None, month=None, slug=None):
    # Note that all posts are visible in the detail view. Unlisted
    # posts are never listed in the archive or the feeds, but if you
    # know the URL of an unlisted post, you can see it. This can be
    # used to share drafts of blog posts.
    posts = Post.objects.all()
    archive_dates = posts.dates('pub_date', 'month')[::-1]
    can_post = is_blog_admin(request)

    if slug is None:
        # Start a new blank Post. If the user isn't logged in,
        # this link won't even show up on the page- so let's
        # just keep it hidden with a 404.

        if not can_post:
            raise Http404
        post = Post(posted_by = request.user)

    else :
        # Look up an existing post. The slug and date must both match.

        try:
            post = posts.get(pub_date__year = int(year),
                             pub_date__month = int(month),
                             slug = slug)
        except Post.DoesNotExist:
            raise Http404

    # Show an editing form if this user is allowed to make posts
    # and if they own the current post.

    post_form = None
    if can_post and post.posted_by.id == request.user.id:

        if request.POST:
            model = dict(request.POST.items())
            model.setdefault('listed', False)
            post_form = EditPostForm(model)

            if post_form.is_valid():
                post.content = post_form.clean_data['content']
                post.title = post_form.clean_data['title']

                # Bump the publication date if we're transitioning from draft to listed.
                if (not post.pub_date) or (post_form.clean_data['listed'] and not post.listed):
                    post.pub_date = datetime.datetime.now()
                post.listed = post_form.clean_data['listed']

                # Update the slug if the post is unlisted
                if (not post.slug) or (not post.listed):
                    post.slug = slugify(post.title)

                # If the post is listed, mark all of its images as permanent.
                # Images in drafts are temporary!
                if post.listed:
                    post.reference_images()

                post.save()
                post.invalidate_cache()
            
        else:
            post_form = EditPostForm(initial = post.__dict__)

    return render_to_response('blog/detail.html', RequestContext(request, {
        'can_post': can_post,
        'archive_dates': archive_dates,
        'post': post,
        'post_form': post_form,
        }))


class BlogFeed(Feed):
    title = 'CIA Blog'
    link = '/blog/'
    description = ('Announcements and discussion for CIA, the '
                   'open source change notification system. Find '
                   'out about new and upcoming features here.')

    def items(self):
        return Post.objects.filter(
            pub_date__lte = datetime.datetime.now(),
            listed = True,
            ).order_by('-pub_date')[:20]

    def item_author_name(self, item):
        return item.posted_by.get_full_name()

    def item_pubdate(self, item):
        return item.pub_date

def blog_feed(request):
    f = BlogFeed('blog', request.path).get_feed()
    response = HttpResponse(mimetype = f.mime_type)
    f.write(response, 'utf-8')
    return response


class CommentFeed(Feed):
    title = 'CIA Blog Comments'
    link = '/blog/'

    def items(self):
        return FreeComment.objects.filter(
            site__pk = settings.SITE_ID,
            is_public = True,
            content_type__app_label__exact = 'blog',
            content_type__model__exact = 'Post',
            )

    def item_author_name(self, item):
        return item.person_name

    def item_pubdate(self, item):
        return item.submit_date

    def item_link(self, item):
        obj = item.get_content_object()
        if obj:
            item_url = obj.get_absolute_url()
        else:
            # The object doesn't exist any more, but this will at
            # least give us a unique URL for the comment.
            item_url = '/'
        return "%s#c%d" % (item_url, item.id)


def comment_feed(request):
    f = CommentFeed('comment', request.path).get_feed()
    response = HttpResponse(mimetype = f.mime_type)
    f.write(response, 'utf-8')
    return response


def post_comment(request):
    """Post a comment, and redirect back to the blog entry on success."""
    response = post_free_comment(request)
    if isinstance(response, HttpResponseRedirect):
        # We can assume content_type refers to Post, since this
        # comment posting URL is local to the blogging app.
        target = request.POST['target']
        content_type_id, object_id = target.split(':')
        return HttpResponseRedirect(Post.objects.get(pk=int(object_id)).get_absolute_url())
    return response
