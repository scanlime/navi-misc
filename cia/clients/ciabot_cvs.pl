#!/usr/bin/perl -w
#
# ciabot -- Mail a CVS log message to a given address, for the purposes of CIA
#
# Loosely based on cvslog by Russ Allbery <rra@stanford.edu>
# Copyright 1998  Board of Trustees, Leland Stanford Jr. University
#
# Copyright 2001, 2003  Petr Baudis <pasky@ucw.cz>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 2, as published by the
# Free Software Foundation.
#
# The master location of this file is
# http://pasky.ji.cz/~pasky/dev/cvs/ciabot.pl.
#
# This program is designed to run from the loginfo CVS administration file. It
# takes a log message, massaging it and mailing it to the address given below.
#
# Its record in the loginfo file should look like:
#
#       ALL        $CVSROOT/CVSROOT/ciabot.pl %s $USER project from_email dest_email
#
# Note that the last three parameters are optional, you can alternatively change
# the defaults below in the configuration section.
#
# $Id: ciabot.pl,v 1.62 2003/09/30 11:06:12 pasky Exp $

use strict;
use vars qw ($project $from_email $dest_email @sendmail $max_lines $max_files
		$sync_delay $xml $commit_template $branch_template
		$trimmed_template);




### Configuration

# Project name (as known to CIA).
$project = 'YOUR PROJECT NAME HERE';

# The from address in the generated mails.
$from_email = 'YOUR EMAIL ADDRESS HERE';

# Mail all reports to this address.
$dest_email = 'commits@picogui.org';

# Path to your sendmail binary. If you have it at a different place (and
# outside of $PATH), add your location at the start of the list. By all means
# keep the trailing empty string in the array.
@sendmail = ('sendmail', '/usr/lib/sendmail', '/usr/sbin/sendmail', '');

# The maximal number of lines the log message should have.
$max_lines = 6;

# Number of files to show at once before an abbreviation (m files in n dirs) is
# used.
$max_files = 2;

# Number of seconds to wait for possible concurrent instances. CVS calls up
# this script for each involved directory separately and this is the sync
# delay. 5s looks as a safe value, but feel free to increase if you are running
# this on a slower (or overloaded) machine or if you have really a lot of
# directories.
$sync_delay = 5;

# Shall we use XML format for the commit messages. Note that this is
# unsupported by the server for now, thus you do not want to do it.
$xml = 0;

# The template string describing how the commit message should look like.
# Expansions:
#  %user%   - who committed it
#  %tag%    - expands to the branch tag template ($branch_template), if the
#             commit hapenned in a branch
#  %module% - the module where the commit happenned
#  %path%   - the longest common path of all the committed files
#  %file%   - the file name or number of files (and possibly number of dirs)
#  %trimmed%- a notice about the log message being trimmed, if it is
#             ($trimmed_template)
#  %logmsg% - the log message
$commit_template = '{green}%user%{normal}%tag% * {light blue}%module%{normal}/%path% (%file%): %trimmed%%logmsg%';

# The template string describing how the branch tag name should look like.
# Expansions:
#  %tag%    - the tag name
$branch_template = ' {yellow}%tag%{normal}';

# The template string describing how the trimming notice should look like.
# Expansions:
#  none
$trimmed_template = '(log message trimmed)';




### The code itself

use vars qw ($user $module $tag @files $logmsg);

my @dir; # This array stores all the affected directories
my @dirfiles;  # This array is mapped to the @dir array and contains files
               # affected in each directory
my $logmsg_lines;



### Input data loading


# These arguments are from %s; first the relative path in the repository
# and then the list of files modified.

@files = split (' ', ($ARGV[0] or ''));
$dir[0] = shift @files or die "$0: no directory specified\n";
$dirfiles[0] = "@files" or die "$0: no files specified\n";


# Guess module name.

$module = $dir[0]; $module =~ s#/.*##;


# Figure out who is doing the update.

$user = $ARGV[1];


# Use the optional parameters, if supplied.

$project = $ARGV[2] if $ARGV[2];
$from_email = $ARGV[3] if $ARGV[3];
$dest_email = $ARGV[4] if $ARGV[4];


# Parse stdin (what's interesting is the tag and log message)

while (<STDIN>) {
  $tag = $1 if /^\s*Tag: ([a-zA-Z0-9_-]+)/;
  last if /^Log Message/;
}

$logmsg_lines = 0;
while (<STDIN>) {
  next unless ($_ and $_ ne "\n" and $_ ne "\r\n");
  $logmsg_lines++;
  last if ($logmsg_lines > $max_lines);
  $logmsg .= $_;
}



### Sync between the multiple instances potentially being ran simultanously

my $sum; # _VERY_ simple hash of the log message. It is really weak, but I'm
         # lazy and it's really sorta exceptional to even get more commits
         # running simultanously anyway.
map { $sum += ord $_ } split(//, $logmsg);

my $syncfile; # Name of the file used for syncing
$syncfile = "/tmp/cvscia.$project.$module.$sum";


if (-f $syncfile) {
  # The synchronization file for this file already exists, so we are not the
  # first ones. So let's just dump what we know and exit.

  open(FF, ">>$syncfile") or die "aieee... can't log, can't log! $syncfile blocked!";
  print FF "$dirfiles[0]!@!$dir[0]\n";
  close(FF);
  exit;

} else {
  # We are the first one! Thus, we'll fork, exit the original instance, and
  # wait a bit with the new one. Then we'll grab what the others collected and
  # go on.

  # We don't need to care about permissions since all the instances of the one
  # commit will obviously live as the same user.

  system("touch $syncfile");

  exit if (fork);
  sleep($sync_delay);

  open(FF, $syncfile);
  my ($dirnum) = 1; # 0 is the one we got triggerred for
  while (<FF>) {
    chomp;
    ($dirfiles[$dirnum], $dir[$dirnum]) = split(/!@!/);
    $dirnum++;
  }
  close(FF);

  unlink($syncfile);
}



### Send out the mail


# Open our mail program

foreach my $sendmail (@sendmail) {
  die "$0: cannot fork sendmail: $!\n" unless ($sendmail);
  open (MAIL, "| $sendmail -t -oi -oem") and last;
}


# The mail header

my $subject;
$subject = "Announce $project";
my $ctype;
$ctype = 'text/' . ($xml ? 'xml' : 'plain');

print MAIL <<EOM;
From: $from_email
To: $dest_email
Content-type: $ctype
Subject: $subject

EOM


# Skip all this nonsense if we're doing XML output.

if ($xml) {
  # TODO: DTD
  print MAIL "<commit>\n";
  print MAIL " <author>$user</author>\n";
  print MAIL " <module>$module</module>\n";
  print MAIL " <branch>$tag</branch>\n" if ($tag);
  print MAIL " <objects>\n";

  for (my $dirnum = 0; $dirnum < @dir; $dirnum++) {
    map {
      $dir[$dirnum] . '/' . $_;
      s/ /&nbsp;/g;
      s/</&lt;/g;
      s/>/&gt;/g;
      print "  <file>$_</file>\n";
    } split(/ /, $dirfiles[$dirnum]);
  }

  print MAIL " </objects>\n";
  print MAIL " <message>$logmsg</message>\n";
  print MAIL "</commit>\n";

  goto body_finished;
}


# Compute the longest common path, plus make up the file and directory count

my (@commondir, $files, @showfiles, $dirnum);

for ($dirnum = 0; $dirnum < @dir; $dirnum++) {
  my ($dir) = $dir[$dirnum];

  # Update the @commondir array...

  my (@currdir) = split(/\//, $dir);
  for (my $cdirnum = 0; $cdirnum < @currdir; $cdirnum++) {

    # Cut the part which is not common(@commondir,@currdir)
    if (defined $commondir[$cdirnum]
        and $commondir[$cdirnum] ne $currdir[$cdirnum]) {
      splice(@commondir, $cdirnum);
      last;
    }

    if ($dirnum == 0) {
      # This is our first run, fill @commondir with @currdir
      $commondir[$cdirnum] = $currdir[$cdirnum];
    } elsif (not defined $commondir[$cdirnum]) {
      # @commondir is over, no need to go on and we can't make it longer,
      # obviously (it would not be common w/ some of the previous dirs)
      last;
    }
  }

  # Update the files count

  my (@currdirfiles) = split(/ /, $dirfiles[$dirnum]);
  $files += @currdirfiles;

  # Fill @showfiles
  for (my $toshowfiles = $max_files;
       $toshowfiles > 0 and @currdirfiles;
       $toshowfiles--) {
    push(@showfiles, [ shift(@currdirfiles), \@currdir ]);
  }
}

die "No files!" unless ($files > 0);

shift(@commondir); # Throw away the module name.


# Send out the mail body


my ($path) = join('/', @commondir);


my ($filestr); # the file name or file count or whatever

if ($files > $max_files) {
  # Too many files to show their full list
  $filestr = $files . ' files';
  if ($dirnum > 1) {
    $filestr .= ' in ' . $dirnum . ' dirs';
  }

} else {
  # Show files list bravely
  $filestr = '';
  my @filestr;
  while ($_ = shift(@showfiles)) {
    my $filename = $_->[0];
    my @currdir = @{$_->[1]};

    # commondir will be already displayed (and module too)
    splice(@currdir, 0, scalar(@commondir) + 1);
    push(@currdir, '') if (@currdir); # trailing slash

    push(@filestr, join('/', @currdir) . $filename);
  }
  $filestr = join(' ', @filestr);
}


my ($trimmedstr); # the trimmed string, if any at all
if ($logmsg_lines > $max_lines) {
  $trimmedstr = $trimmed_template;
} else {
  $trimmedstr = '';
}

my ($tagstr); # the branch name, if any at all
if ($tag) {
  $tagstr = $branch_template;
  $tagstr =~ s/\%tag\%/$tag/g;
} else {
  $tagstr = '';
}

$logmsg = "\n" . $logmsg if ($logmsg_lines > 1);

my ($bodystr) = $commit_template; # the message to be sent
$bodystr =~ s/\%user\%/$user/g;
$bodystr =~ s/\%tag\%/$tagstr/g;
$bodystr =~ s/\%module\%/$module/g;
$bodystr =~ s/\%path\%/$path/g;
$bodystr =~ s/\%file\%/$filestr/g;
$bodystr =~ s/\%trimmed\%/$trimmedstr/g;
$bodystr =~ s/\%logmsg\%/$logmsg/g;

print MAIL $bodystr."\n";


body_finished:

# Close the mail

close MAIL;
die "$0: sendmail exit status " . $? >> 8 . "\n" unless ($? == 0);
