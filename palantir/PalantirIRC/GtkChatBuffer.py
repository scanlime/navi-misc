''' GtkChatBuffer.py

This module contains the object for the chat buffer.

  W. Evan Sheehan <evan@navi.cx>
'''

import gtk, string

class GtkChatBuffer(gtk.ScrolledWindow):
  ''' Objects for creating text buffers for displaying text from the channels.
      This will hold a reference to the name of the channel the buffer is showing.
      '''
  def __init__(self):
    # Initialize the scrolled window.
    gtk.ScrolledWindow.__init__(self)
    self.set_policy(2,0)

    # Create the text view.
    self.text_view = gtk.TextView()
    self.text_view.show()
    self.text_view.set_wrap_mode(gtk.WRAP_WORD)
    self.text_view.get_buffer().create_tag('highlight', foreground='yellow')
    self.text_view.get_buffer().create_tag('nickends', foreground='purple')
   
    # Add the text view to the scrolled window.
    self.add(self.text_view)

    #self.background = gtk.DrawingArea()
    #self.background.show()
    #self.add_child_in_window(self.background, gtk.TEXT_WINDOW_WIDGET, 0, 0)
    #self.modify_fg(gtk.STATE_NORMAL, gtk.gdk.Color(235, 235, 235))
    #self.modify_bg(gtk.STATE_NORMAL, gtk.gdk.Color(235, 235, 235))


  def DisplayText(self, time, nick, msg, nickends=(' ',' '), highlight=False):
    ''' Called to display msg in the buffer.  'nickends' go around the nick. '''
    buffer = self.text_view.get_buffer()
    buffer.insert(buffer.get_end_iter(), time + nickends[0] + nick + nickends[1] + msg + '\n')

    #adjust = self.get_vadjustment()
    #if adjust.value + adjust.page_size <= adjust.upper:
      #adjust.set_value(adjust.upper + adjust.page_size)
      #self.set_vadjustment(adjust)

    self.text_view.scroll_to_iter(buffer.get_end_iter(),0.0)

    # Apply highlighting tags.
    if highlight:
      bounds = buffer.get_end_iter().backward_search(nick.strip(string.join(nickends,(' '))), 1)
      buffer.apply_tag_by_name('highlight', bounds[0], bounds[1])

    iter = buffer.get_end_iter()
    iter.backward_line()

    for end in nickends:
      bounds = iter.forward_search(end.strip(), 1)
      if bounds:
	buffer.apply_tag_by_name('nickends', bounds[0], bounds[1])
