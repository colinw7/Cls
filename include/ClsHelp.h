static const char *
usage = "\
ls [-abcdfgiklmnopqrstuxzACFLMNRSXZ123456789?]\n\
   [--format <format>]\n\
   [--color|--colour]\n\
   [--nocolor|--nocolour]\n\
   [--special[<color>] <pattern>]\n\
   [--nlink]\n\
   [--user]\n\
   [--links|--nolinks]\n\
   [--empty_dirs]\n\
   [--bad|--my_bad|--nobad]\n\
   [--zero|--nonzero]\n\
   [--width <width>]\n\
   [--screen_cols <cols>]\n\
   [--sort {name,time,size,extension}]\n\
   [--excl/ude <file_flags>]\n\
   [--incl/ude <file_flags>]\n\
   [--prefix <prefix>]\n\
   [--newer <days>]\n\
   [--older <days>]\n\
   [--larger <bytes>]\n\
   [--smaller <bytes>]\n\
   [--match <pattern>]\n\
   [--nomatch <pattern>]\n\
   [--full_path/--fullpath]\n\
   [--no_path/--nopath]\n\
   [--show_secs]\n\
   [--case|--nocase]\n\
   [--exec_init <cmd>]\n\
   [--exec_term <cmd>]\n\
   [--exec <cmd>]\n\
   [--filter <exec>]\n\
   [--test <flags>]\n\
   [--quiet]\n\
   [--html]\n\
   [--type_escape|--no_type_escape]\n\
   [--datatype|--data_type]\n\
   [--bad_names]\n\
   [--silent]\n\
   [--h/elp]\n\
   [--help_format]\n\
   [--help_file_type]\n\
   [--help_exec]\n\
\n\
  -a     : do not ignore entries starting with .\n\
  -A     : do not list implied . and ..\n\
  -b     : print C-style escapes for nongraphic characters\n\
  -c     : with -lt: sort by, and show, ctime (time of last\n\
           modification of file status information);\n\
           with -l: show ctime and sort by name;\n\
           otherwise: sort by ctime, newest first\n\
  -C     : list entries by columns\n\
  -d     : list directories themselves, not their contents\n\
  -f     : do not sort, enable -aU, disable -ls --color\n\
  -F     : append indicator (one of */=>@|) to entries\n\
  -g     : like -l, but do not list owner\n\
  -G     : in a long listing, don't print group names\n\
  -h     : with -l and -s, print sizes like 1K 234M 2G etc.\n\
  -i     : print the index number of each file\n\
  -k     : default to 1024-byte blocks for disk usage;\n\
           used only with -s and per directory totals\n\
  -l     : use a long listing format\n\
  -L     : when showing file information for a symbolic\n\
           link, show information for the file the link\n\
           references rather than for the link itself\n\
  -m     : fill width with a comma separated list of entries\n\
  -n     : like -l, but list numeric user and group IDs\n\
  -o     : like -l, but do not list group information\n\
  -p     : append / indicator to directories\n\
  -q     : print ? instead of nongraphic characters\n\
  -r     : reverse order while sorting\n\
  -R     : list subdirectories recursively\n\
  -s     : print the allocated size of each file, in blocks\n\
  -t     : sort by time, newest first\n\
  -T     : assume tab stops at each COLS instead of 8\n\
  -u     : with -lt: sort by, and show, access time;\n\
           with -l: show access time and sort by name;\n\
           otherwise: sort by access time, newest first\n\
  -x     : list entries by lines instead of by columns\n\
  -[1-9] : list N files per line. Avoid '\\n' with -q or -b\
";
