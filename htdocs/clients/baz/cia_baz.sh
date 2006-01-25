#!/bin/sh

## Hook script showing CIA client usage.
## See the very bottom for selection of archives/categories to inform
## CIA of
## Copyright (C) 2003 Robert Collins
## Copyright (C) 2005 Canonical Limited
##
## Portions adapted from :
##
## Copyright (C) 2003 Ethan Benson
##
## Portions adapted from larch:
##
## Copyright (C) 2001-2003 Tom Lord
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of version 2 of the GNU General Public License
## as published by the Free Software Foundation.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
###############################################################################


MAILX=sendmail_wrapper

sendmail_wrapper()
{
   if [ $# -ne 3 ] ; then
       echo 1>&2 "sendmail usage error: need 3 arguments"
       exit 1
   fi
   if [ "$1" != "-s" ] ; then
       echo 1>&2 "sendmail usage error: first argument must be -s"
       exit 1
   fi
   ( printf "To: %s\\n" "$3" ; \
     printf "Subject: %s\\n" "$2" ; \
     printf "\\n" ; \
     cat ) \
   | /usr/sbin/sendmail -t
}


# CIA bot client script for baz hooks, delivering via email
# -- svn version by Micah Dowty <micah@navi.cx>
# -- adapted by Robert Collins <robert.collins@canonical.com>
#
# See http://cia.navi.cx
# for more information on what the CIA bot is and how it works.
#
# To use the CIA bot in your Arch repository...
#
# 1. Customize the parameters below, specifically the ones under
#    the "Project information" section
#
# 2. This script should be sourced and run from your baz hook script
#    for revisions you want to tell cia about. You need the MAILX
#    macro commonly used in baz hook scripts.
#
# See below for parameters that can be customised

cia_entry()
{
 awk -v archive="$archive" \
     -v version="$version" \
     -v category="$category" \
     -v branch="$branch" \
     -v patch="$patch" \
     -v no_files="$no_files" \
   'BEGIN { getline;
            while (!match ($0, "^$"))
              {
                field = tolower ($1);
                sub (":.*", "", field);
                headers[field] = $0;
                sub ("^[^:]*:[ \t]*", "", headers[field]);
                getline;
                while (match ($0, "^[ \t]"))
                  {
                    headers[field] = headers[field] $0;
                    getline;
                  }
              }
          }

    { if (body == "") body = "    " $0; else body = body "\n    " $0; }

    END {
          date = headers["standard-date"];
          author = headers["creator"];
          summary = headers["summary"];

          #sub("[[:space:]].*GMT.*", " GMT", date);
          #print date "\t" headers["creator"] "\t" patch;
          #print "";
          gsub("&", "\\&amp;", author);
          gsub("<", "\\&lt;", author);
          gsub(">", "\\&gt;", author);
          print "<author>" author "</author>";
          gsub("&", "\\&amp;", summary);
          gsub("<", "\\&lt;", summary);
          gsub(">", "\\&gt;", summay);
          print "<log>" summary "</log>";
          #print body;
          #print "";
          if (no_files == "")
            {
              print "<files>"
              file_list(0, "new-files", "{arch}/" category "/" branch "/" version "/" archive "/patch-log/" patch);
              file_list(0, "removed-files");
              file_list(0, "modified-files");
              file_pair_list("renamed-files");
              file_list(0, "new-directories");
              file_list(0, "removed-directories");
              file_list(0, "modified-directories");
              file_pair_list("renamed-directories");
              file_list(0, "new-patches", archive "/" version "--" patch);
              print "</files>"
            }
        }

    function file_list (base_only, field_name, exclude)
    {
      for (x in items)
        delete items[x];

      n_items = split (headers[field_name], items, "[ \t]+");

      if ((n_items == 0) || ((exclude != "") && (n_items == 1)))
        return;

      sub("-", " ", field_name);
      # print "    " field_name ":"
      # printf("    ");
      width = 0;
      items_on_line = 0;

      for (x = 1; x <= n_items; ++x)
        {
          if (exclude == items[x])
            continue;
          if (base_only)
            sub (".*/", "", items[x]);
          if ((items_on_line == 0) || ((width + length (items[x]) + 1) < 64))
            {
              width += length (items[x]) + 1;
              ++items_on_line;
              printf(" %s", items[x]);
            }
          else
            {
              printf("\n");
              printf("    ");
              printf(" %s", items[x]);
              width = length(items[x]) + 1;
              items_on_line = 1;
            }
        }
      printf "\n"
      printf "\n"
    }
   function file_pair_list (field_name)
    {
      for (x in items)
        delete items[x];

      n_items = split (headers[field_name], items, "[ \t]+");

      if (n_items == 0)
        return;

      sub("-", " ", field_name);
      print "    " field_name ":"

      for (x = 1; x <= n_items; ++x)
        {
          printf("     %s\n", items[x]);
          printf("       ==> %s\n", items[x + 1]);
          x = x + 1;
        }
      printf "\n"
      printf "\n"
    }'
}

cia_revision()
{
   local archive="$ARCH_ARCHIVE"
   local version="$(v=${ARCH_REVISION%--*};echo ${v##*--})"
   local category="${ARCH_REVISION%%--*}"
   local branch="$(v=${ARCH_REVISION%--*--*};echo ${v##*--})"
   local patch="${ARCH_REVISION##*--}"

   local details="$(baz cat-archive-log
${ARCH_ARCHIVE}/${ARCH_REVISION} | cia_entry)"
(cat <<EOF
<message>
   <generator>
       <name>Baz CIA Bot client shell script</name>
       <version>1.0</version>
   </generator>
   <source>
       <project>$project_name</project>
   </source>
   <body>
       <commit>
           <revision>${ARCH_ARCHIVE}/${ARCH_REVISION}</revision>
           ${details}
       </commit>
   </body>
</message>
EOF
)
           #<diffLines>$diff_lines</diffLines>
}

inform_cia()
{

### There are some parameters for this script that you can customize:
# Project information
#
# NOTE: This shouldn't be a long description of your project. Ideally
#       it is a short identifier with no local spaces, punctuation, or
#       unnecessary capitalization. This will be used in URLs related
#       to your project, as an internal identifier, and in IRC messages.
#       If you want a longer name shown for your project on the web
#       interface, please use the "title" metadata key rather than
#       putting that here.
#
   local project_name="$1"
   local return_address="YOUREMAIL"

############# Below this line you shouldn't have to change anything

# The email address CIA lives at
   local cia_address="cia@cia.navi.cx"

# Send an email with the final XML message
   cia_revision | $MAILX -s "DeliverXML" $cia_address
}
### The End of CIA support section ####

case "$1" in
   commit|import|tag)
           ARCHIVENAMEGOESHERE)
               case "${ARCH_REVISION%%--*}" in
                   CATEGORYNAMEGOESHERE)
                   inform_cia PROJECTTOREPORTAS
                   ;;
                   *)
                   ;;
               esac
               ;;
           *)
               ;;
       esac
#       update_library
       ;;
esac

