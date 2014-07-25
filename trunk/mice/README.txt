This is MICE, which is an acronym for Metro Internet Chat Event.

Technically its not free software, since it I do it at work, so I shall seek to make it as disgusting and unreadable as possible so nobody in their right mind would want to use it.  That said, more than anything else, its just a fun little project for me.

Mice is actually a suite of programs that work together.
There are six conceptual elements.  Server, Submission, Moderation, Interview, Administration and Updating.

The server is written in Java, and facilitates the function of the other elements.
Submission is what happens with questions, in that they are put into the server.
Moderation takes the questions, and either approves or rejects them.
Interview answers questions that have been approved.
Updating sends out the answered questions to the world for viewing.

Administration sits outside and tells the server how to bind all these things together.


Submission happens via a Java Applet, HTML CGI and an AIM bot.
Moderation happens via a Java Applet.
Interview happens via a Java Applet, or AIM Bot.
Updating happens via a Web Page or AIM Bot.
Administration happens via a Java Applet.

Ok, now that things have been described as such, the directory structure is done by method.  Really, if you can't look at it and figure it out, I probably can't help you in the README :)
