''' GtkChatBuffer.py

This module contains the object for the chat buffer.

  W. Evan Sheehan <evan@navi.cx>
'''

import gtk, string

class GtkChatBuffer(gtk.TextView):
  ''' Objects for creating text buffers for displaying text from the channels.
      This will hold a reference to the name of the channel the buffer is showing.
      '''
  def __init__(self):
    gtk.TextView.__init__(self)
    self.set_wrap_mode(gtk.WRAP_WORD)
    highlight = self.get_buffer().create_tag('highlight', foreground='yellow')
    nickEndsColor = self.get_buffer().create_tag('nickends', foreground='purple')
    self.background = gtk.DrawingArea()
    self.background.show()
    self.add_child_in_window(self.background, gtk.TEXT_WINDOW_WIDGET, 0, 0)
    #self.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(235, 235, 235))
    #self.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color(235, 235, 235))


  def DisplayText(self, time, nick, msg, nickends=(' ',' '),highlight=False):
    buffer = self.get_buffer()
    buffer.insert(buffer.get_end_iter(), time + nickends[0] + nick + nickends[1] + msg + '\n')

    if highlight:
      bounds = buffer.get_end_iter().backward_search(nick.strip(string.join(nickends,(' '))), gtk.TEXT_SEARCH_TEXT_ONLY)
      buffer.apply_tag_by_name('highlight', bounds[0], bounds[1])
    self.scroll_to_iter(buffer.get_end_iter(), 0.0)

    iter = buffer.get_end_iter()
    iter.backward_line()
    for end in nickends:
      bounds = iter.forward_search(end.strip(), gtk.TEXT_SEARCH_TEXT_ONLY)
      if bounds: buffer.apply_tag_by_name('nickends', bounds[0], bounds[1])
