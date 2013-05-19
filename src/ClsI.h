#include <Cls.h>

#define SECONDS_PER_DAY (24*60*60)

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
   [--screen_width <width>]\n\
   [--sort {name,time,size,extension}]\n\
   [--excl/ude <file_flags>]\n\
   [--incl/ude <file_flags>]\n\
   [--newer <days>]\n\
   [--older <days>]\n\
   [--larger <bytes>]\n\
   [--smaller <bytes>]\n\
   [--match <pattern>]\n\
   [--nomatch <pattern>]\n\
   [--full_path/--fullpath]\n\
   [--no_path/--nopath]\n\
   [--show_secs]\n\
   [--nocase]\n\
   [--exec_init <cmd>]\n\
   [--exec_term <cmd>]\n\
   [--exec <cmd>]\n\
   [--quiet]\n\
   [--html]\n\
   [--type_escape]\n\
   [--datatype]\n\
   [--bad_names]\n\
   [--h/elp]\
";
