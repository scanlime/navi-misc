''' GtkChatBuffer.py

This module contains the object for the chat buffer.

  W. Evan Sheehan <evan@navi.cx>
'''

import gtk, string

class ChatBuffer(gtk.ScrolledWindow):
  ''' Objects for creating text buffers for displaying text from the channels.
      This will hold a reference to the name of the channel the buffer is showing.
      '''
  def __init__(self):
    # Initialize the scrolled window.
    gtk.ScrolledWindow.__init__(self)
    self.set_policy(2,0)
    self.set_shadow_type(gtk.SHADOW_IN)

    # Create the text view.
    self.text_view = gtk.TextView()
    self.text_view.show()
    self.text_view.set_wrap_mode(gtk.WRAP_WORD)
    self.text_view.get_buffer().create_tag('highlight', foreground='yellow')
    self.text_view.get_buffer().create_tag('nickends', foreground='purple')
    self.text_view.set_editable(gtk.FALSE)
    self.text_view.set_cursor_visible(gtk.FALSE)

    # For now the default colors are white on black, this needs to be configurable.
    bg = gtk.gdk.color_parse("black")
    fg = gtk.gdk.color_parse("white")
    self.text_view.modify_text(gtk.STATE_NORMAL, fg)
    self.text_view.modify_base(gtk.STATE_NORMAL, bg)

    # Add the text view to the scrolled window.
    self.add(self.text_view)

  def DisplayText(self, time, nick, text, highlight=False):
    ''' Called to display msg in the buffer.  'nickends' go around the nick. '''
    buffer = self.text_view.get_buffer()

    if time:
      time = '[' + string.join([str(field) for field in time],':') + '] '
    else:
      time = ''

    buffer.insert(buffer.get_end_iter(),time + nick + text + '\n')

    self.text_view.scroll_to_iter(buffer.get_end_iter(),0.0)

    if nick.find('<') != -1:
      bounds = buffer.get_end_iter().backward_search(nick.strip('< >'), 1)
    elif nick.find('*') != -1:
      bounds = buffer.get_end_iter().backward_search(nick.strip('* '), 1)
    else:
      bounds = None

    # Apply highlighting tags.
    if highlight:
      buffer.apply_tag_by_name('highlight', bounds[0], bounds[1])

    if bounds:
      start = bounds[0].copy()
      start.backward_chars(2)
      end = bounds[1].copy()
      end.forward_char()
      buffer.apply_tag_by_name('nickends', start,bounds[0])
      buffer.apply_tag_by_name('nickends', bounds[1], end)
