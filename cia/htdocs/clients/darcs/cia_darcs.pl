#!/usr/bin/perl -w
use Cwd;

# CIA Client for Darcs
# Copyright (C) 2005 Patrick McFarland <diablod3@gmail.com>

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 2, as published by the
# Free Software Foundation.

# Based on a darcs mailing list log script written by:
# (C) 2004-12-10 Thomas Radke <tradke@aei.mpg.de>
# Some changes by Erik Schnetter <schnetter@aei.mpg.de>
# http://www.mail-archive.com/darcs-devel@darcs.net/msg00831/darcs

# Also based on a CIA CVS bot written by:
# Loosely based on cvslog by Russ Allbery <rra@stanford.edu>
# Copyright 1998  Board of Trustees, Leland Stanford Jr. University
# Copyright 2001, 2003, 2004  Petr Baudis <pasky@ucw.cz>
# http://cia.vc/clients/cvs/ciabot_cvs.pl

# Bug tracker: https://sourceforge.net/tracker/?group_id=37810

# Notes:
#
# This script acts as a wrapper for darcs. Simply rename darcs (to, say,
# 'darcs.real'), and then put this script where darcs used to be. You
# execute this script instead of the real darcs, and everything works fine.
# 
# Darcs does not execute ssh as a login shell, which causes a bug that
# the remote shell won't load .profile/.bash_profile. If you're trying to put
# the darcs wrapper in a directory that is added to the path via any custom
# environment files (such as ~/bin added via .profile, .bash_profile,
# .bashrc, etc), then darcs never executes the wrapper. There are three ways
# to fix this:
#
# 1) to configure your sshd to always use login shells
# 2) use zsh instead of bash, and add the directory to your path in .zshenv
# 3) move /usr/bin/darcs to /usr/bin/darcs.real and install the wrapper as
#    /usr/bin/darcs

### some constants

# NOTE: This shouldn't be a long description of your project. Ideally
#       it is a short identifier with no spaces, punctuation, or
#       unnecessary capitalization. This will be used in URLs related
#       to your project, as an internal identifier, and in IRC messages.
#       If you want a longer name shown for your project on the web
#       interface, please use the "title" metadata key rather than
#       putting that here.
my $project = "YourProjectNameGoesHere";

# 0 uses xml-rpc, 1 uses email
my $use_method = 0;

# Path to your real darcs executable
my $darcs = '/usr/bin/darcs.real';

# If using XML-RPC, connect to this URI.
my $rpc_uri = 'http://cia.vc/RPC2';

# If using email, the From: address in generated mails
my $from_email = 'you@your.host';

# If using email, the To: address in generated mails
my $dest_email = 'cia@cia.vc';

# If using email, path to your USCD sendmail compatible binary 
my $sendmail = '/usr/sbin/sendmail';

### nothing below should be changed

# patch database
my %submitters = ();
my %timestamps = ();
my %comments   = ();
my %files = ();

my ($VERSION) = '0.1';
my ($URL) = 'http://www.shadowconflict.com/darcs/darcs-cia/darcs';
my $ts = time;

# get repo name for this project
if($ARGV[3]) {
  chdir($ARGV[3]);
}
my $module = getcwd();
chomp $module;
foreach(split(/\//, $module)) {
  $module = $_;
}
$module =~ s/\///g;

my $line;

# sanity check
die "Couldn't find executable '$darcs'!\n\n" if (! -x $darcs);

#foreach(@ARGV) { print "$_ "; } # uncomment this to enable debugging
#print "\n";                     # of what darcs gets passed via script

# short cut for darcs commands other than 'apply --all'
exec ($darcs, @ARGV)
  if (! (($#ARGV == 1 || ($#ARGV == 3  && $ARGV[2] eq '--repodir')) && $ARGV[0] eq 'apply' && $ARGV[1] eq '--all'));

# open a pipe for running darcs on the other end
open (DARCS, "| $darcs @ARGV") || die "Couldn't open pipe to darcs !\n";

# skip everything before the 'New patches:' section
while (<STDIN>)
{
  print DARCS;
  last if (/^New patches:$/);
}

# separator for a patch's header and its contents
my $endmarker = '] {';

# now parse individual patches
while (<STDIN>)
{
  print DARCS;

  # each patch starts with a line '[<patch name>'
  next if (! /^\[(.+)$/);
  my $patch = $1;

  # on the next line follow the submitter's email address
  # and the timestamp of the patch
  $_ = <STDIN>;
  print DARCS;
  next if (! /^(.+)\*\*(\d{4})(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})($endmarker)??$/o);

  # add this patch to the database
  $submitters{$patch} = $1;
  # convert the timestamp into some readable form 'YYYY-MM-DD HH:MM:SS'
  $timestamps{$patch} = "$2-$3-$4 $5:$6:$7";

  # everything until an end-marker string belongs to
  # a long comment for this patch
  if (! $8)
  {
    while (<STDIN>)
    {
      print DARCS;
      last if (/^$endmarker$/);
      $comments{$patch} .= $_;
    }
  }

  # this grabs the file names from all the patches. If darcs gains any
  # new patch types that use filenames, those will have to be added here.
  while ($_ = <STDIN>) {
    print DARCS;
    $l = $_;
    $l =~ s/\.\///g;
    if($l =~ /adddir (.*)\n/) { push @{$files{$patch}}, "$1 " }
    if($l =~ /addfile (.*)\n/) { push @{$files{$patch}}, "$1 " }
    if($l =~ /rmdir (.*)\n/) { push @{$files{$patch}}, "$1 " }
    if($l =~ /rmfile (.*)\n/) { push @{$files{$patch}}, "$1 " }
    if($l =~ /binary (.*)\n/) { push @{$files{$patch}}, "$1 " }
    if($l =~ /hunk (.*)\n/) {
      $m = $1;
      $m .= "\n";
      $m =~ s/[0-9]\n/\n/;
      $m =~ s/[0-9]\n/\n/;
      $m =~ s/[0-9]\n/\n/;
      $m =~ s/[0-9]\n/\n/;
      $m =~ s/ \n//;
      push @{$files{$patch}}, "$m ";
    }
    if($l =~ /move (.*)\n/) {
      $m = $1;
      foreach(split(/ /, $m)) {
        push @{$files{$patch}}, "$_ ";
      }
    }
    last if (/^}$/);
  }
}

close (DARCS) || die "Failed to run darcs command '$darcs @ARGV'\n";

# now send out notification
foreach $patch (keys %submitters)
{
  ${submitters{$patch}} =~ s/&/&amp;/g;
  ${submitters{$patch}} =~ s/</&lt;/g;
  ${submitters{$patch}} =~ s/>/&gt;/g;

  ${project} =~ s/&/&amp;/g;
  ${project} =~ s/</&lt;/g;
  ${project} =~ s/>/&gt;/g;

  ${module} =~ s/&/&amp;/g;
  ${module} =~ s/</&lt;/g;
  ${module} =~ s/>/&gt;/g;

  $message = <<EM
<message>
  <generator>
    <name>CIA Perl client for Darcs</name>
    <version>$VERSION</version>
    <url>$URL</url>
  </generator>
  <source>
    <project>$project</project>
    <module>$module</module>
  </source>
  <timestamp>
    $ts
  </timestamp>
  <body>
    <commit>
      <author>${submitters{$patch}}</author>
      <files>
EM
;

  foreach (@{$files{$patch}}) {
  my $filename = $_;

  $filename =~ s/&/&amp;/g;
  $filename =~ s/</&lt;/g;
  $filename =~ s/>/&gt;/g;

  $message .= "<file>$filename</file>\n";
  }

$message .= "</files>\n<log>";

if ($comments{$patch}) {
  $patch =~ s/&/&amp;/g;
  $patch =~ s/</&lt;/g;
  $patch =~ s/>/&gt;/g;

  ${comments{$patch}} =~ s/&/&amp;/g;
  ${comments{$patch}} =~ s/</&lt;/g;
  ${comments{$patch}} =~ s/>/&gt;/g;

  $message .= "$patch:$comments{$patch}";
} else {
  $patch =~ s/&/&amp;/g;
  $patch =~ s/</&lt;/g;
  $patch =~ s/>/&gt;/g;

  $message .= "$patch";
}

$message .= <<EM
      </log>
    </commit>
  </body>
</message>
EM
;

  if($use_method == 1) {
    open (MAIL, "| $sendmail -t -oi") or die "Cannot execute $sendmail : " . ($?>>8);

print MAIL <<EOM;
From: $from_email
To: $dest_email
Content-type: text/xml
Subject: DeliverXML

EOM

  print MAIL $message;

  close MAIL;
  die "$0: sendmail exit status " . ($? >> 8) . "\n" unless ($? == 0);
  } else {
    $^W = 0;
    $RPC::XML::ERROR if (0);

    require RPC::XML;
    require RPC::XML::Client;

    my $rpc_client = new RPC::XML::Client $rpc_uri;
    my $rpc_request = RPC::XML::request->new('hub.deliver', $message);
    my $rpc_response = $rpc_client->send_request($rpc_request);

    unless (ref $rpc_response) {
    die "XML-RPC Error: $RPC::XML::ERROR\n";
    }
  }
}
