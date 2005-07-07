#!/usr/bin/perl
#
# Example script showing how to use Term::VT102 with an SSH command. SSHs to
# localhost and runs a shell, and dumps what Term::VT102 thinks should be on
# the screen.
#

use Term::VT102;
use IO::Handle '_IONBF';
use POSIX ':sys_wait_h';
use IO::Pty;
use strict;

$| = 1;

my $cmd = 'sh';

# Create a pty for the SSH command to run on.
#
my $pty = new IO::Pty;
my $tty_name = $pty->ttyname ();
if (not defined $tty_name) {
	die "Could not assign a pty";
}
$pty->autoflush ();

# Run the SSH command in a child process.
#
my $pid = fork;
if (not defined $pid) {
	die "Cannot fork: $!";
} elsif ($pid == 0) {
	#
	# Child process - set up stdin/out/err and run the command.
	#

	# Become process group leader.
	#
	if (not POSIX::setsid ()) {
		warn "Couldn't perform setsid: $!";
	}

	# Get details of the slave side of the pty.
	#
	my $tty = $pty->slave ();
	$tty_name = $tty->ttyname();

	# Set the pty's terminal parameters.
	#
	IO::Stty::stty ($tty, 'raw');

	# File descriptor shuffling - close the pty master, then close
	# stdin/out/err and reopen them to point to the pty slave.
	#
	close ($pty);
	close (STDIN);
	close (STDOUT);
	open (STDIN, "<&" . $tty->fileno ())
	|| die "Couldn't reopen " . $tty_name . " for reading: $!";
	open (STDOUT, ">&" . $tty->fileno())
	|| die "Couldn't reopen " . $tty_name . " for writing: $!";
	close (STDERR);
	open (STDERR, ">&" . $tty->fileno())
	|| die "Couldn't redirect STDERR: $!";

	# Finally, run the command, and die if we can't.
	#
	exec $cmd;
	die "Cannot exec '$cmd': $!";
}

my $vt = Term::VT102->new (
  'cols' => 80,
  'rows' => 24,
);

# Convert linefeeds to linefeed + carriage return.
#
$vt->option_set ('LFTOCRLF', 1);

# Make sure line wrapping is switched on.
#
$vt->option_set ('LINEWRAP', 1);

my ($cmdbuf, $stdinbuf, $iot, $eof);

# IO::Handle for standard input - unbuffered.
#
$iot = new IO::Handle;
$iot->fdopen (fileno(STDIN), 'r');
#$iot->setvbuf(undef, _IONBF, 0);

# Set up the callback for OUTPUT; this callback function simply sends
# whatever the Term::VT102 module wants to send back to the terminal and
# sends it to the child process - see its definition below.
#
$vt->callback_set ('OUTPUT', \&vt_output, $pty);
$vt->callback_set ('ROWCHANGE', \&vt_rowchange, $pty);
$vt->callback_set ('CLEAR', \&vt_clear, $pty);
$vt->callback_set ('SCROLL_UP', \&vt_scrollup, $pty);
$vt->callback_set ('SCROLL_DOWN', \&vt_scrolldown, $pty);

open BUFFER_FILE, ">/tmp/term-output";
BUFFER_FILE->autoflush();

$eof = 0;

# Set stdin's terminal to raw mode so we can pass all keypresses straight
# through immediately.
#
IO::Stty::stty ($iot, 'raw', '-echo');

while (not $eof) {
	my ($rin, $win, $ein, $rout, $wout, $eout, $nr);

	($rin, $win, $ein) = ('', '', '');
	vec ($rin, $pty->fileno, 1) = 1;
	vec ($rin, $iot->fileno, 1) = 1;

	select ($rout=$rin, $wout=$win, $eout=$ein, 1);

	# Read from the SSH command if there is anything coming in, and
	# pass any data on to the Term::VT102 object.
	#
	$cmdbuf = '';
	$nr = 0;
	if (vec ($rout, $pty->fileno, 1)) {
		$nr = $pty->sysread ($cmdbuf, 16);
		$eof = 1 if ((defined $nr) && ($nr == 0));
		$vt->process ($cmdbuf) if ((defined $nr) && ($nr > 0));
	}

# Do your stuff here - use $vt->row_plaintext() to see what's on various
# rows of the screen, for instance, or before this main loop you could set
# up a ROWCHANGE callback which checks the changed row, or whatever.
#
# In this example, we just pass standard input to the SSH command, and we
# take the data coming back from SSH and pass it to the Term::VT102 object,
# and then we repeatedly dump the Term::VT102 screen.

	# Read key presses from standard input and pass them to the command
	# running in the child process.
	#
	$stdinbuf = '';
	if (vec ($rout, $iot->fileno, 1)) {
		$nr = $iot->sysread ($stdinbuf, 16);
		$eof = 1 if ((defined $nr) && ($nr == 0));
		$pty->syswrite ($stdinbuf, $nr) if ((defined $nr) && ($nr > 0));
	}

	# Make sure the child process has not died.
	#
	$eof = 1 if (waitpid ($pid, &WNOHANG) > 0);
}

print "\r\n";
$pty->close;

IO::Stty::stty ($iot, 'sane');

sub vt_rowchange {
    my ($vtobject, $type, $row) = @_;
    my $text = $vt->row_plaintext($row);
    $row -= 1;
    print BUFFER_FILE "draw\t0\t$row\t$text\n";
}

sub vt_clear {
    print BUFFER_FILE "clear\n";
}

sub vt_scrollup {
	my ($vtobject, $type, $origin, $count) = @_;
	print BUFFER_FILE "scroll\tup\t$origin\t$count\n";
}

sub vt_scrolldown {
	my ($vtobject, $type, $origin, $count) = @_;
	print BUFFER_FILE "scroll\tdown\t$origin\t$count\n";
}

# Callback for OUTPUT events - for Term::VT102.
#
sub vt_output {
	my ($vtobject, $type, $arg1, $arg2, $private) = @_;

	if ($type eq 'OUTPUT') {
		$pty->syswrite ($arg1, length $arg1);
	}
}

# EOF
