#!/usr/bin/perl
use IO::Socket;

$name = "John Jacobs";
$tag = "Whooble wooble";
$pregunta = "How exactly does all of this junk work%3F%2F%2F%5C%5C%3A%29%26%26%25";

print "Content-type: text/html\n\n";
print "<HTML> \n";
print "Hello World<br>\n";
read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

@pairs = split(/&/, $buffer);

($foo,$name) = split(/=/,$pairs[0]);
($foo,$tag) = split(/=/,$pairs[1]);
($foo,$pregunta) = split(/=/,$pairs[2]);

my $sock = new IO::Socket::INET (
                                 PeerAddr => 'studentactivities.mscd.edu',
#                                PeerAddr => 'localhost',
                                 PeerPort => '8080',
                                 Proto => 'tcp',
                                );
die "Could not create socket: $!<br>Try your submission again<br>\n" unless $sock;
print "Question submitted successfully.<br>
Use the back button to return to the chat.
</HTML>";

print $sock "PSUB\r\n";

print $sock $name,"\r\n";
print $sock $tag,"\r\n";
print $sock $pregunta,"\r\n";

close($sock);
