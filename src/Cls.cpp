#include <Cls.h>
#include <ClsI.h>

#include <COSUser.h>
#include <CTerm.h>
#include <CStrUtil.h>
#include <CStrFmt.h>
#include <CDir.h>
#include <CFileUtil.h>
#include <CTime.h>
#include <CGlob.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <iostream>

#ifndef major
# define major(rdev) ((rdev) >> 8)
# define minor(rdev) ((rdev) & 0xFF)
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

class ClsCompareNames {
 private:
  bool nocase;
  bool reverse;

 public:
  ClsCompareNames(bool nocase1, bool reverse1) :
   nocase(nocase1), reverse(reverse1) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);
};

class ClsCompareTimes {
 private:
  bool ctime;
  bool utime;
  bool mtime;
  bool reverse;

 public:
  ClsCompareTimes(bool ctime1, bool utime1, bool mtime1, bool reverse1) :
   ctime(ctime1), utime(utime1), mtime(mtime1), reverse(reverse1) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);
};

class ClsCompareSizes {
 private:
  bool reverse;

 public:
  ClsCompareSizes(bool reverse1) :
   reverse(reverse1) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);
};

class ClsCompareExtensions {
 public:
  int operator()(ClsFile *file1, ClsFile *file2);
};

Cls::
Cls()
{
  init();
}

Cls::
~Cls()
{
}

void
Cls::
init()
{
  a_flag = false;
  b_flag = false;
  c_flag = false;
  d_flag = false;
  f_flag = false;
  g_flag = false;
  i_flag = false;
  k_flag = false;
  l_flag = false;
  m_flag = false;
  n_flag = false;
  o_flag = false;
  p_flag = false;
  q_flag = false;
  r_flag = false;
  s_flag = false;
  t_flag = false;
  u_flag = false;
  x_flag = false;
  A_flag = false;
  C_flag = false;
  F_flag = false;
  L_flag = false;
  M_flag = false;
  R_flag = false;
  T_flag = false;

  nlink_flag      = false;
  user_uid_       = COSUser::getEffectiveUserId();
  user_gid_       = COSUser::getGroupId();
  show_links      = false;
  show_bad        = false;
  show_bad_user   = false;
  show_bad_other  = false;
  show_empty_dirs = false;
  sort_type       = SORT_NAME;
  force_width     = 0;
  my_umask        = COSUser::getUMask();
  full_path       = false;
  no_path         = false;
  show_secs       = false;
  nocase          = false;
  exec_init_cmd_  = "";
  exec_term_cmd_  = "";
  exec_cmd_       = "";
  quiet           = false;
  html            = false;
//type_escape     = false;
  datatype        = false;
  bad_names       = false;

#if 0
  if (getenv("CTERM_VERSION"))
    type_escape = true;
#endif

  num_columns = 0;

  ls_format = "";

  max_len  = -1;
  max_size = 0;

  if (! CTerm::getCharSize(&screen_rows, &screen_cols)) {
    if (getenv("COLUMNS")) screen_cols = atoi(getenv("COLUMNS"));
    if (getenv("ROWS"   )) screen_rows = atoi(getenv("ROWS"   ));
  }

  if (screen_cols <= 0) screen_cols = 80;
  if (screen_rows <= 0) screen_rows = 60;

  if (getenv("CLS_DEBUG_SCREEN_SIZE"))
    cerr << "Columns: " << screen_cols << " Rows: " << screen_rows << endl;

  list_pos     = 0;
  list_max_pos = screen_cols - 1;

  base_dir = CDir::getCurrent();

  current_time_ = new CTime();

  if (my_umask != 0022 && my_umask != 0002)
    cerr << "Bad umask " << my_umask << endl;

  read_files = false;

  use_colors = true;
}

void
Cls::
process_args(int argc, char **argv)
{
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (argv[i][1] != '-') {
        if (argv[i][1] == '\0')
          read_files = true;

        for (int j = 1; argv[i][j] != '\0'; ++j) {
          switch (argv[i][j]) {
            case 'a':
              a_flag = true;
              A_flag = false;
              break;
            case 'b':
              b_flag = true;
              q_flag = false;
            case 'c':
              c_flag = true;
              u_flag = false;
              break;
            case 'd':
              d_flag = true;
              break;
            case 'f':
              f_flag = true;
              a_flag = true;
              l_flag = false;
              s_flag = false;
              r_flag = false;

              sort_type = SORT_NONE;

              break;
            case 'g':
              g_flag = true;
              l_flag = true;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'i':
              i_flag = true;
              break;
            case 'k':
              k_flag = true;
              break;
            case 'l':
              l_flag = true;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'm':
              m_flag = true;
              l_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'n':
              n_flag = true;
              l_flag = true;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'o':
              o_flag = true;
              l_flag = true;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'p':
              p_flag = true;
              break;
            case 'q':
              q_flag = true;
              b_flag = false;
              break;
            case 'r':
              r_flag = true;
              break;
            case 's':
              s_flag = true;
              break;
            case 't':
              t_flag = true;
              sort_type = SORT_TIME;
              break;
            case 'u':
              u_flag = true;
              c_flag = false;
              break;
            case 'x':
              x_flag = true;
              l_flag = false;
              m_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'z':
              filter_data_.setShowNonZero(false);
              break;
            case 'A':
              A_flag = true;
              a_flag = false;
              break;
            case 'C':
              C_flag = true;
              l_flag = false;
              m_flag = false;
              x_flag = false;
              T_flag = false;
              break;
            case 'F':
              F_flag = true;
              break;
            case 'L':
              L_flag = true;
              break;
            case 'M':
              M_flag = true;
              break;
            case 'N':
              nlink_flag = true;
              break;
            case 'R':
              R_flag = true;
              break;
            case 'S':
              sort_type = SORT_SIZE;
              break;
            case 'T':
              T_flag = true;
              l_flag = false;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              break;
            case 'X':
              sort_type = SORT_EXTENSION;
              break;
            case 'Z':
              filter_data_.setShowZero(false);
              break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              num_columns = argv[i][j] - '0';

              x_flag = true;
              l_flag = false;
              m_flag = false;
              C_flag = false;
              T_flag = false;

              break;
            case '?':
              cerr << usage << endl;

              exit(0);
            default:
              cerr << "Invalid switch -" << argv[i][j] << endl;
              break;
          }
        }
      }
      else {
        if      (strcmp(&argv[i][2], "format") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --format" << endl;
            continue;
          }

          ls_format = argv[i];
        }
        else if (strcmp(&argv[i][2], "color" ) == 0 ||
                 strcmp(&argv[i][2], "colour") == 0) {
          use_colors = true;
        }
        else if (strcmp(&argv[i][2], "nocolor" ) == 0 ||
                 strcmp(&argv[i][2], "nocolour") == 0) {
          use_colors = false;
        }
        else if (strncmp(&argv[i][2], "special", 7) == 0) {
          int color = COLOR_SPECIAL;

          if (argv[i][9] != '\0') {
            int color_num = atoi(&argv[i][9]);

            if      (color_num == 0)
              color = COLOR_NORMAL;
            else if (color_num == 1)
              color = COLOR_DIRECTORY;
            else if (color_num == 2)
              color = COLOR_EXECUTABLE;
            else if (color_num == 3)
              color = COLOR_LINK;
            else if (color_num == 4)
              color = COLOR_PIPE;
            else if (color_num == 5)
              color = COLOR_SPECIAL;
            else if (color_num == 6)
              color = COLOR_BAD_FILE;
            else
              color = COLOR_BAD_FILE;
          }

          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --special" << endl;
            continue;
          }

          CGlob *glob = new CGlob(argv[i]);

          special_globs[color].push_back(glob);
        }
        else if (strcmp(&argv[i][2], "nlink") == 0) {
          nlink_flag = true;
        }
        else if (strcmp(&argv[i][2], "user") == 0) {
          filter_data_.setOnlyUser(true);
        }
        else if (strcmp(&argv[i][2], "links") == 0) {
          show_links = true;
        }
        else if (strcmp(&argv[i][2], "nolinks") == 0) {
          show_links = false;
        }
        else if (strcmp(&argv[i][2], "empty_dirs") == 0) {
          show_empty_dirs = true;
        }
        else if (strcmp(&argv[i][2], "bad") == 0) {
          show_bad       = true;
          show_bad_user  = true;
          show_bad_other = true;
        }
        else if (strcmp(&argv[i][2], "mybad") == 0) {
          show_bad       = true;
          show_bad_user  = true;
          show_bad_other = false;
        }
        else if (strcmp(&argv[i][2], "nobad") == 0) {
          show_bad       = false;
          show_bad_user  = false;
          show_bad_other = false;
        }
        else if (strcmp(&argv[i][2], "zero") == 0) {
          filter_data_.setShowNonZero(false);
        }
        else if (strcmp(&argv[i][2], "nonzero") == 0) {
          filter_data_.setShowZero(false);
        }
        else if (strcmp(&argv[i][2], "width") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --width" << endl;
            continue;
          }

          force_width = atoi(argv[i]);

          if (force_width == 0) {
            cerr << "Invalid value for --width" << endl;
            continue;
          }
        }
        else if (strcmp(&argv[i][2], "screen_cols") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --screen_cols" << endl;
            continue;
          }

          screen_cols = atoi(argv[i]);

          if (screen_cols <= 0) {
            cerr << "Invalid value for --screen_cols" << endl;
            continue;
          }
        }
        else if (strcmp(&argv[i][2], "sort") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --sort" << endl;
            continue;
          }

          if      (strcmp(argv[i], "none") == 0)
            sort_type = SORT_NONE;
          else if (strcmp(argv[i], "name") == 0)
            sort_type = SORT_NAME;
          else if (strcmp(argv[i], "time") == 0)
            sort_type = SORT_TIME;
          else if (strcmp(argv[i], "size") == 0)
            sort_type = SORT_SIZE;
          else if (strcmp(argv[i], "extension") == 0)
            sort_type = SORT_EXTENSION;
          else
            cerr << "Invalid value '" << argv[i] << "' for --sort" << endl;
        }
#if 0
        else if (strcmp(&argv[i][2], "exclude_type") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --exclude_type" << endl;
            continue;
          }

          char *exclude_type = argv[i];

          filter_data_.addExcludeFileType(exclude_type);
        }
#endif
        else if (strcmp(&argv[i][2], "excl"   ) == 0 ||
                 strcmp(&argv[i][2], "exclude") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --exclude" << endl;
            continue;
          }

          char *exclude = argv[i];

          for (uint j = 0; exclude[j] != '\0'; ++j) {
            int type = decode_type_char("exclude", exclude[j]);

            if (type != 0)
              filter_data_.addExcludeType(type);
          }
        }
#if 0
        else if (strcmp(&argv[i][2], "include_type") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --include_type" << endl;
            continue;
          }

          char *include_type = argv[i];

          filter_data_.addIncludeFileType(include_type);
        }
#endif
        else if (strcmp(&argv[i][2], "incl"   ) == 0 ||
                 strcmp(&argv[i][2], "include") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --include" << endl;
            continue;
          }

          char *include = argv[i];

          for (uint j = 0; include[j] != '\0'; ++j) {
            int type = decode_type_char("include", include[j]);

            if (type != 0)
              filter_data_.addIncludeType(type);
          }
        }
        else if (strcmp(&argv[i][2], "newer") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --newer" << endl;
            continue;
          }

          filter_data_.setNewer(atoi(argv[i]));
        }
        else if (strcmp(&argv[i][2], "older") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --older" << endl;
            continue;
          }

          filter_data_.setOlder(atoi(argv[i]));
        }
        else if (strcmp(&argv[i][2], "larger") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --larger" << endl;
            continue;
          }

          filter_data_.setLarger(atoi(argv[i]));
        }
        else if (strcmp(&argv[i][2], "smaller") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --smaller" << endl;
            continue;
          }

          filter_data_.setSmaller(atoi(argv[i]));
        }
        else if (strcmp(&argv[i][2], "match") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --match" << endl;
            continue;
          }

          vector<string> words;

          CStrUtil::addWords(argv[i], words);

          uint num_words = words.size();

          for (uint j = 0; j < num_words; ++j)
            filter_data_.addMatch(words[j]);
        }
        else if (strcmp(&argv[i][2], "nomatch") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --nomatch" << endl;
            continue;
          }

          vector<string> words;

          CStrUtil::addWords(argv[i], words);

          uint num_words = words.size();

          for (uint j = 0; j < num_words; ++j)
            filter_data_.addNoMatch(words[j]);
        }
        else if (strcmp(&argv[i][2], "full_path") == 0 ||
                 strcmp(&argv[i][2], "fullpath" ) == 0) {
          full_path = true;
        }
        else if (strcmp(&argv[i][2], "no_path") == 0 ||
                 strcmp(&argv[i][2], "nopath" ) == 0) {
          no_path = true;
        }
        else if (strcmp(&argv[i][2], "show_secs") == 0) {
          show_secs = true;
        }
        else if (strcmp(&argv[i][2], "nocase") == 0) {
          nocase = true;
        }
        else if (strcmp(&argv[i][2], "exec_init") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --exec_init" << endl;
            continue;
          }

          exec_init_cmd_ = argv[i];
        }
        else if (strcmp(&argv[i][2], "exec_term") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --exec_term" << endl;
            continue;
          }

          exec_term_cmd_ = argv[i];
        }
        else if (strcmp(&argv[i][2], "exec") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --exec" << endl;
            continue;
          }

          exec_cmd_ = argv[i];
        }
        else if (strcmp(&argv[i][2], "filter") == 0) {
          ++i;

          if (i >= argc) {
            cerr << "Missing argument for --filter" << endl;
            continue;
          }

          filter_data_.setExec(argv[i]);
        }
        else if (strcmp(&argv[i][2], "quiet") == 0) {
          quiet = true;
        }
        else if (strcmp(&argv[i][2], "html") == 0) {
          html = true;
        }
#if 0
        else if (strcmp(&argv[i][2], "type_escape") == 0) {
          type_escape = true;
        }
        else if (strcmp(&argv[i][2], "no_type_escape") == 0) {
          type_escape = false;
        }
#endif
#if 0
        else if (strcmp(&argv[i][2], "datatype" ) == 0 ||
                 strcmp(&argv[i][2], "data_type") == 0) {
          datatype = true;
        }
#endif
        else if (strcmp(&argv[i][2], "bad_names") == 0) {
          bad_names = true;
        }
        else if (strcmp(&argv[i][2], "h") == 0 ||
                 strcmp(&argv[i][2], "help") == 0) {
          cerr << usage << endl;

          exit(0);
        }
        else
          cerr << "Invalid switch --" << &argv[i][2] << endl;
      }
    }
    else {
      ClsFile *file = new ClsFile(L_flag, argv[i]);

      files.push_back(file);
    }
  }
}

void
Cls::
exec()
{
  if (read_files) {
    string file_string;

    int c = fgetc(stdin);

    while (c != EOF) {
      if (isspace(c))
        c = ' ';

      file_string += c;

      c = fgetc(stdin);
    }

    if (use_colors) {
      for (uint i = 0; i < 7; ++i) {
        string color = color_to_string((ClsColorType) i);

        uint color_len = color.size();

        string::size_type pos = file_string.find(color);

        if (pos != string::npos) {
          string file_string1 = file_string.substr(0, pos);

          for (uint j = 0; j < color_len; ++j)
            file_string1 += " ";

          file_string1 += file_string.substr(pos + color_len);
        }
      }
    }

    vector<string> words;

    CStrUtil::addWords(file_string, words);

    uint num_words = words.size();

    for (uint i = 0; i < num_words; ++i) {
      ClsFile *file = new ClsFile(L_flag, words[i]);

      files.push_back(file);
    }
  }

  uint num_files = files.size();

  for (uint i = 0; i < num_files; ++i)
    files[i]->setLinkName(encode_name(files[i]->getName()));

  if (ls_format != "") {
    l_flag = false;
    m_flag = false;
    x_flag = false;
    C_flag = false;
    T_flag = false;
  }

  if (C_flag || x_flag || m_flag) {
    list_pos     = 0;
    list_max_pos = screen_cols;
  }

  if (html) {
    outputLine("<html>");
    outputLine("<head>");
    outputLine("</head>");
    outputLine("<body>");
    outputLine("<pre>");
  }

  if (exec_init_cmd_ != "")
    system(exec_init_cmd_.c_str());

  dir_depth = 0;

  enter_dir(".");

  if (num_files > 0) {
    vector<ClsFile *> dfiles;
    vector<ClsFile *> rfiles;

    split_files(files, dfiles, rfiles);

    set_max_filelen(rfiles);

    output_files(rfiles);

    set_max_filelen(dfiles);

    if (d_flag) {
      output_files(dfiles);
    }
    else {
      uint num_dfiles = dfiles.size();
      uint num_rfiles = rfiles.size();

      if (num_dfiles == 1 && num_rfiles == 0)
        list_dir(dfiles[0]);
      else {
        for (uint i = 0; i < num_dfiles; ++i)
          list_dir_entry(dfiles[i]);
      }
    }
  }
  else {
    ClsFile *file = new ClsFile(L_flag, ".", ".");

    files.push_back(file);

    set_max_filelen(files);

    if (d_flag)
      output_files(files);
    else {
      vector<ClsFile *> files1;

      get_dir_files(files[0], files1);

      list_dir_files(files1);

      if (R_flag)
        recurse_files(files1);

      free_dir_files(files1);
    }
  }

  if (exec_term_cmd_ != "")
    system(exec_term_cmd_.c_str());

  if (html) {
    outputLine("</pre>");
    outputLine("</body>");
    outputLine("</html>");
  }
}

void
Cls::
list_dir_entry(ClsFile *file)
{
  if (! file->isDir())
    return;

  if (! enter_dir(file->getName()))
    return;

  vector<ClsFile *> files;

  get_dir_files(file, files);

  if (show_empty_dirs) {
    if (files.empty()) {
      if (html)
        output("<a href='" + relative_dir1 + "'>" + relative_dir1 + "</a><br>");
      else {
//      if (type_escape)
//        outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

        outputColored(COLOR_DIRECTORY, relative_dir1);

        outputLine("");
      }
    }
  }
  else {
    uint num_files = files.size();

    int num_files1 = 0;

    for (uint i = 0; i < num_files; ++i)
      if (files[i]->getIsOutput())
        ++num_files1;

    if (num_files1 > 0) {
      if (! quiet) {
        if (! full_path) {
          if (html)
            output("<br>");

          if (! T_flag)
            outputLine("");
        }

        if (! full_path) {
          if      (html)
            output("<a href='" + relative_dir1 + "'>" + relative_dir1);
          else {
            if (T_flag) {
              for (int j = 0; j < dir_depth - 1; ++j)
                output(" ");

              output("|- ");
            }

//          if (type_escape)
//            outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

            outputColored(COLOR_DIRECTORY, relative_dir1);
          }

          output(":");

          if      (html)
            output("</a><br>");

          outputLine("");
        }
      }

      list_dir_files(files);
    }
  }

  if (R_flag)
    recurse_files(files);

  free_dir_files(files);

  leave_dir();
}

void
Cls::
list_dir(ClsFile *file)
{
  if (! enter_dir(file->getName()))
    return;

  vector<ClsFile *> files;

  get_dir_files(file, files);

  list_dir_files(files);

  if (R_flag)
    recurse_files(files);

  free_dir_files(files);

  leave_dir();
}

void
Cls::
get_dir_files(ClsFile *, vector<ClsFile *> &files)
{
  vector<string> dir_files;

  DIR *dir = opendir(".");
  if (dir == NULL) return;

  string filename;

  struct dirent *dirent = readdir(dir);

  while (dirent != NULL) {
    if (dirent->d_name[0] == '.' &&
         (dirent->d_name[1] == '\0' ||
          (dirent->d_name[1] == '.' && dirent->d_name[2] == '\0')))
      goto next;

    dir_files.push_back(dirent->d_name);

 next:
    dirent = readdir(dir);
  }

  closedir(dir);

  if (a_flag) {
    add_dir_files("." , files);
    add_dir_files("..", files);
  }

  uint num_dir_files = dir_files.size();

  for (uint i = 0; i < num_dir_files; ++i) {
    if (dir_files[i][0] == '.') {
      if (! a_flag && ! A_flag)
        continue;
    }

    add_dir_files(dir_files[i], files);
  }
}

void
Cls::
add_dir_files(const string &name, vector<ClsFile *> &files)
{
  ClsFile *file = new ClsFile(L_flag, name, encode_name(name));

  ClsFilterType filter = filter_file(file);

  if (filter == FILTER_OUT) {
    delete file;
    return;
  }

  if (filter == FILTER_DIR)
    file->setIsOutput(false);

  files.push_back(file);
}

ClsFilterType
Cls::
filter_file(ClsFile *file)
{
  if (filter_data_.checkFile(this, file))
    return FILTER_IN;

  if (! (R_flag && file->isDir()))
    return FILTER_OUT;

  return FILTER_DIR;
}

void
Cls::
list_dir_files(vector<ClsFile *> &files)
{
  if (show_empty_dirs) {
    if (files.empty()) {
      if (html)
        output("<a href='" + relative_dir1 + "'>" + relative_dir1 + "</a><br>");
      else {
//      if (type_escape)
//        outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

        outputColored(COLOR_DIRECTORY, relative_dir1);

        outputLine("");
      }
    }

    return;
  }

  //------

  if (C_flag || x_flag || m_flag) {
    list_pos     = 0;
    list_max_pos = screen_cols;
  }

  if (! f_flag)
    sort_files(files);

  set_max_filelen(files);

  output_files(files);
}

void
Cls::
recurse_files(vector<ClsFile *> &files)
{
  uint num_files = files.size();

  for (uint i = 0; i < num_files; ++i) {
    const string &name = files[i]->getName();

    if (name[0] == '.') {
      if (! a_flag && ! A_flag)
        continue;

      if (name == "." || name == "..")
        continue;

      list_dir_entry(files[i]);
    }
    else
      list_dir_entry(files[i]);
  }
}

void
Cls::
free_dir_files(vector<ClsFile *> &files)
{
  uint num_files = files.size();

  for (uint i = 0; i < num_files; ++i)
    delete files[i];
}

void
Cls::
list_file(ClsFile *file)
{
  static char link_name[MAXPATHLEN + 1];

  if (bad_names && CFileUtil::isBadFilename(file->getName())) {
    if (! quiet) {
      if (full_path)
        cerr << "Bad filename '" << current_dir << "/" <<
                     file->getName() << "'" << endl;
      else
        cerr << "Bad filename '" << relative_dir << "/" <<
                     file->getName() << "'" << endl;
    }
    else {
      if (full_path)
        cout << "'" << current_dir << "/" << file->getName() << "'" << endl;
      else
        cout << "'" << relative_dir << "/" << file->getName() << "'" << endl;
    }
  }

  if (! file->exists())
    return;

  ClsData list_data;

  list_data.file = file;

  if      (file->isFIFO   ())
    list_data.type = 'p';
  else if (file->isChar   ())
    list_data.type = 'c';
  else if (file->isDir    ())
    list_data.type = 'd';
  else if (file->isBlock  ())
    list_data.type = 'b';
  else if (file->isRegular())
    list_data.type = '-';
  else if (file->isLink   ())
    list_data.type = 'l';
  else if (file->isSocket ())
    list_data.type = 's';
  else
    list_data.type = '?';

  int u_perm = file->getUPerm();
  int g_perm = file->getGPerm();
  int o_perm = file->getOPerm();

  set_perm(list_data.u_perm, u_perm, S_IRUSR);
  set_perm(list_data.g_perm, g_perm, S_IRGRP);
  set_perm(list_data.o_perm, o_perm, S_IROTH);

  list_data.size   = file->getSize();
  list_data.uid    = file->getUid();
  list_data.gid    = file->getGid();
  list_data.ino    = file->getINode();
  list_data.nlink  = file->getNLink();
  list_data.dev    = file->getDev();
  list_data.rdev   = file->getRDev();
  list_data.blocks = file->getNumBlocks();
  list_data.mtime  = new CTime(*file->getMTime());
  list_data.ctime  = new CTime(*file->getCTime());
  list_data.atime  = new CTime(*file->getATime());
  list_data.name   = file->getLinkName();

  if ((i_flag || s_flag) && ! l_flag) {
    if (s_flag && i_flag)
      CStrUtil::sprintf(list_data.name, "%7ld %4ld ", list_data.ino, list_data.blocks);
    else if (s_flag)
      CStrUtil::sprintf(list_data.name, "%4ld ", list_data.blocks);
    else
      CStrUtil::sprintf(list_data.name, "%7ld ", list_data.ino);

    list_data.name += file->getLinkName();
  }

  if (F_flag || p_flag) {
    if      (list_data.type == 'd')
      list_data.name += "/";
    else if (list_data.type == 'l')
      list_data.name += "@";
    else if (file->isUserExecutable())
      list_data.name += "*";
    else if (list_data.type == 'p' || list_data.type == 's')
      list_data.name += "|";
    else
      list_data.name += " ";
  }

  if      (show_bad && is_bad_file(file))
    list_data.color = COLOR_BAD_FILE;
  else if (list_data.type == 'd')
    list_data.color = COLOR_DIRECTORY;
  else if (list_data.type == 'p' || list_data.type == 's')
    list_data.color = COLOR_PIPE;
  else if (list_data.type == 'l') {
    if (file->hasLinkStat())
      list_data.color = COLOR_LINK;
    else
      list_data.color = COLOR_BAD_FILE;
  }
  else if (list_data.type == 'b' || list_data.type == 'c')
    list_data.color = COLOR_DEVICE;
  else if (file->isUserExecutable())
    list_data.color = COLOR_EXECUTABLE;
  else {
    ClsColorType color;

    if (special_glob_match(file->getName(), &color))
      list_data.color = color;
    else
      list_data.color = COLOR_NORMAL;
  }

  if (list_data.type == 'l') {
    int len = readlink(file->getName().c_str(), link_name, MAXPATHLEN);

    if (len == -1)
      len = 0;

    link_name[len] = '\0';

    list_data.link_name = link_name;

    if      (file->isDir())
      list_data.link_color = COLOR_DIRECTORY;
    else if (file->isFIFO() || file->isSocket())
      list_data.link_color = COLOR_PIPE;
    else if (file->isUserExecutable())
      list_data.link_color = COLOR_EXECUTABLE;
    else
      list_data.link_color = COLOR_NORMAL;
  }
  else
    list_data.link_name = "";

  if (! quiet)
    print_list_data(&list_data);

  if (exec_cmd_ != "")
    exec_file(file, exec_cmd_);
}

bool
Cls::
is_bad_file(ClsFile *file)
{
  int u_perm = file->getUPerm();
  int g_perm = file->getGPerm();
  int o_perm = file->getOPerm();

  if (file->getUid() == getUserUid()) {
    if (! show_bad_user)
      return false;

    if      (file->isDir()) {
      if (u_perm != 7)
        return true;

      if (g_perm > u_perm || (my_umask == 0022 && g_perm == 7) ||
          (g_perm != 7 && g_perm != 5 && g_perm != 0))
        return true;

      if (o_perm > g_perm ||
          (o_perm != 5 && o_perm != 0))
        return true;
    }
    else if (file->isRegular()) {
      if (u_perm != 7 && u_perm != 6)
        return true;

      if (g_perm > u_perm ||
          (my_umask == 0022 && (g_perm == 7 || g_perm == 6)) ||
          (g_perm != 7 && g_perm != 6 && g_perm != 5 &&
           g_perm != 4 && g_perm != 0))
        return true;

      if (o_perm > g_perm ||
          (o_perm != 5 && o_perm != 4 && o_perm != 0))
        return true;
    }
  }
  else {
    if (! show_bad_other)
      return false;

    if      (file->isDir()) {
      if (user_gid_ == file->getGid() && g_perm == 7)
        return true;

      if (o_perm == 7)
        return true;
    }
    else if (file->isRegular()) {
      if (user_gid_ == file->getGid() && (g_perm == 7 || g_perm == 6))
        return true;

      if (o_perm == 7 || o_perm == 6)
        return true;
    }
  }

  return false;
}

void
Cls::
print_list_data(ClsData *list_data)
{
  if (ls_format != "") {
    uint i = 0;

    string output_string;

    uint len = ls_format.size();

    while (i < len) {
      if (ls_format[i] != '%' || i >= len - 1) {
        output_string += ls_format[i++];
        continue;
      }

      int i1 = i;

      ++i;

      char justify = ' ';

      if (ls_format[i] == '-' || ls_format[i] == '+')
        justify = ls_format[++i];

      int field_width = 0;

      while (i < len && isdigit(ls_format[i]))
        field_width = 10*field_width + (ls_format[++i] - '0');

      string str;

      switch (ls_format[i]) {
        case 'b': {
          if (field_width == 0)
            field_width = 6;

          str = blocks_to_string(list_data->blocks);

          break;
        }
        case 'c': {
          ++i;

          int color = 0;

          while (i < len && isdigit(ls_format[i]))
            color = 10*color + (ls_format[++i] - '0');

          str = color_to_string((ClsColorType) color);

          --i;

          break;
        }
        case 'd': {
          if (field_width == 0)
            field_width = 12;

          if      (ls_format[i + 1] == 'a') {
            str = time_to_string(list_data->atime);

            ++i;
          }
          else if (ls_format[i + 1] == 'c') {
            str = time_to_string(list_data->ctime);

            ++i;
          }
          else if (ls_format[i + 1] == 'm') {
            str = time_to_string(list_data->mtime);

            ++i;
          }
          else {
            if      (c_flag)
              str = time_to_string(list_data->ctime);
            else if (u_flag)
              str = time_to_string(list_data->atime);
            else
              str = time_to_string(list_data->mtime);
          }

          break;
        }
        case 'e': {
          string temp_str1;

          while (i < len - 1 && ! isspace(ls_format[i + 1])) {
            if (ls_format[i + 1] == '\\' && i < len - 2)
              ++i;

            temp_str1 += ls_format[++i];
          }

          temp_str1 += ' ';

          temp_str1 += list_data->name;

          str = exec_to_string(temp_str1);

          break;
        }
        case 'f': {
          if (field_width == 0 && max_len > 0)
            field_width = max_len;

          if (justify == ' ')
            justify = '-';

          if (html)
            str = list_data->name;
          else
            str = color_to_string(list_data->color) + list_data->name +
                  color_to_string(COLOR_NORMAL);

          break;
        }
        case 'g': {
          if (field_width == 0)
            field_width = 8;

          str = gid_to_string(list_data->gid);

          break;
        }
        case 'i': {
          if (field_width == 0)
            field_width = 8;

          str = inode_to_string(list_data->ino);

          break;
        }
        case 'l': {
          if (list_data->link_name != "") {
            if (html)
              str = list_data->link_name;
            else
              str = color_to_string(list_data->link_color) + list_data->link_name +
                    color_to_string(COLOR_NORMAL);
          }
          else
            str = "";

          break;
        }
        case 'n': {
          if (field_width == 0)
            field_width = 4;

          str = CStrUtil::toString(list_data->nlink);

          break;
        }
        case 'p': {
          if      (ls_format[i + 1] == 'u') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->u_perm;

            ++i;
          }
          else if (ls_format[i + 1] == 'g') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->g_perm;

            ++i;
          }
          else if (ls_format[i + 1] == 'o') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->o_perm;

            ++i;
          }
          else {
            if (field_width == 0)
              field_width = 9;

            str = list_data->u_perm + list_data->g_perm + list_data->o_perm;
          }

          break;
        }
        case 's': {
          if (field_width == 0)
            field_width = 8;

          str = size_to_string(list_data);

          break;
        }
        case 't': {
          if (field_width == 0)
            field_width = 1;

          str = type_to_string(list_data->type);

          break;
        }
        case 'u': {
          if (field_width == 0)
            field_width = 8;

          str = uid_to_string(list_data->uid);

          break;
        }
        case 'D': {
          if (field_width == 0)
            field_width = 0;

          str = relative_dir1;

          break;
        }
        case 'G': {
          if (field_width == 0)
            field_width = 6;

          str = CStrUtil::toString(list_data->gid);

          break;
        }
        case 'L': {
          if (list_data->link_name != "")
            str = "->";
          else
            str = "";

          break;
        }
        case 'U': {
          if (field_width == 0)
            field_width = 6;

          str = CStrUtil::toString(list_data->uid);

          break;
        }
        default:
          break;
      }

      ++i;

      if (str != "") {
        if (field_width > 0) {
          if (justify == '-')
            output_string += CStrFmt::align(str, field_width, CSTR_FMT_ALIGN_RIGHT);
          else
            output_string += CStrFmt::align(str, field_width, CSTR_FMT_ALIGN_LEFT);
        }
        else
          output_string += str;
      }
      else {
        for (uint k = i1; k < i; k++)
          output_string += ls_format[k];
      }
    }

    if (html)
      outputLine(output_string + "<br>");
    else
      outputLine(output_string);
  }
  else {
#if 0
    if (datatype) {
      string type = get_data_type_str(list_data->file);

      output(CStrFmt::align(type, 12, CSTR_FMT_ALIGN_LEFT));
    }
#endif

    if       (l_flag) {
      if (i_flag)
        CStrUtil::printf("%7ld ", list_data->ino);

      if (s_flag)
        CStrUtil::printf("%4ld ", list_data->blocks);

      if (! datatype) {
        char c = (char) list_data->type;

        output(string(&c, 1));
      }

      output(list_data->u_perm);
      output(list_data->g_perm);
      output(list_data->o_perm);

      if (nlink_flag)
        CStrUtil::printf(" %3d", list_data->nlink);

      if (! o_flag)
        output(" " + CStrFmt::align(uid_to_string(list_data->uid), 8, CSTR_FMT_ALIGN_LEFT));

      if (! g_flag)
        output(" " + CStrFmt::align(gid_to_string(list_data->gid), 8, CSTR_FMT_ALIGN_LEFT));

      if (list_data->type == 'b' || list_data->type == 'c')
        CStrUtil::printf(" %3d,%3d", major(list_data->rdev), minor(list_data->rdev));
      else {
        if      (M_flag) {
          size_t size;

          double size1 = list_data->size/1024.0/1024.0;

          if (size1 > 0.05)
            CStrUtil::printf(" %4.1fM", list_data->size/1024.0/1024.0);
          else {
            size = list_data->size >> 10;

            if (size > 0)
              CStrUtil::printf(" %4ldK", size);
            else
              CStrUtil::printf(" %5ld", list_data->size);
          }
        }
        else if (k_flag) {
          size_t size = list_data->size >> 10;

          if (size > 0)
            CStrUtil::printf(" %5ldK", size);
          else
            CStrUtil::printf(" %6ld", list_data->size);
        }
        else {
#if _FILE_OFFSET_BITS == 64
          CStrUtil::printf(" %*lld", max_size_len, (long long) list_data->size);
#else
          CStrUtil::printf(" %*ld", max_size_len, (long) list_data->size);
#endif
        }
      }

      if      (c_flag)
        output(" " + CStrFmt::align(time_to_string(list_data->ctime), 12, CSTR_FMT_ALIGN_RIGHT));
      else if (u_flag)
        output(" " + CStrFmt::align(time_to_string(list_data->atime), 12, CSTR_FMT_ALIGN_RIGHT));
      else
        output(" " + CStrFmt::align(time_to_string(list_data->mtime), 12, CSTR_FMT_ALIGN_RIGHT));

      output(" ");

      string name = list_data->name;

      uint len = name.size();

      if (force_width != 0 && (int) len > force_width)
        name = CStrFmt::align(name, force_width - 1, CSTR_FMT_ALIGN_LEFT, ' ', true) + ">";

      if (html)
        output("<a href='" + relative_dir1 + "/" + name + "'>" + name + "</a>");
      else {
//      if (type_escape)
//        outputTypeEscape(list_data);

        outputColored(list_data->color, name);
      }

      if (show_links && list_data->link_name != "") {
        output(" -> ");

        if (html)
          output("<a href='" + relative_dir1 + "/" + list_data->link_name + "'>" +
                 list_data->link_name + "</a>");
        else {
//        if (type_escape)
//          outputTypeEscape(list_data, list_data->link_name);

          outputColored(list_data->link_color, list_data->link_name);
        }
      }

      if (html)
        output("<br>");

      outputLine("");
    }
    else if (C_flag || x_flag) {
      uint len = list_data->name.size();

      if (full_path)
        len += relative_dir1.size() + 1;

      if (force_width == 0 || (int) len <= force_width) {
        if (full_path) {
          string relative_dir2 =
            relative_dir1 + "/" + list_data->name;

          if (html)
            output("<a href='" + relative_dir2 + "'>" + relative_dir2 + "</a>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
          }
        }
        else {
          if      (html)
            output("<a href='" + relative_dir1 + "/" + list_data->name +
                   "'>" + list_data->name + "</a>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, list_data->name);
          }
        }
      }
      else {
        if (full_path) {
          uint relative_dir1_len = relative_dir1.size();

          if ((int) relative_dir1_len + 1 <= force_width) {
            string relative_dir2 =
              CStrFmt::align(relative_dir1, force_width - 1, CSTR_FMT_ALIGN_RIGHT, ' ', true);

            if (html)
              output("<a href='" + relative_dir1 + "'>" + relative_dir2 + "></a>");
            else {
//            if (type_escape)
//              outputTypeEscape(list_data);

              outputColored(list_data->color, relative_dir2);
              outputColored(COLOR_CLIPPED   , ">");
            }
          }
          else {
            string relative_dir2 = relative_dir1 + "/" +
              CStrFmt::align(list_data->name, force_width - relative_dir1_len - 2,
                             CSTR_FMT_ALIGN_RIGHT, ' ', true);

            if (html)
              output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
                     relative_dir2 + "></a>");
            else {
//            if (type_escape)
//              outputTypeEscape(list_data);

              outputColored(list_data->color, relative_dir2);
              outputColored(COLOR_CLIPPED   , ">");
            }
          }
        }
        else {
          string relative_dir2 =
            CStrFmt::align(list_data->name, force_width - 1, CSTR_FMT_ALIGN_RIGHT, ' ', true);

          if (html)
            output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
                   relative_dir2 + "></a>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
            outputColored(COLOR_CLIPPED   , ">");
          }
        }
      }

      for (uint i = 0; i < max_len - len; ++i)
        output(" ");
    }
    else if (m_flag) {
      uint len = list_data->name.size();

      int list_pos1 = list_pos;

      if (list_pos > 0)
        list_pos1 += 2;

      list_pos1 += len;

      if (list_pos1 >= list_max_pos) {
        if (html)
          output("<br>");

        outputLine("");

        list_pos  = 0;
        list_pos1 = len;
      }

      if (list_pos > 0)
        output(", ");

      if (html)
        output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
               list_data->name + "</a>");
      else {
//      if (type_escape)
//        outputTypeEscape(list_data);

        outputColored(list_data->color, list_data->name);
      }

      list_pos = list_pos1;
    }
    else {
      uint len = list_data->name.size();

      if (force_width == 0 || (int) len <= force_width) {
        if (full_path) {
          string relative_dir2 = relative_dir1 + "/" + list_data->name;

          if (html)
            output("<a href='" + relative_dir2 + "'>" + relative_dir2 + "</a><br>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
          }
        }
        else {
          if (html)
            output("<a href='" + relative_dir1 + "/" + list_data->name +
                   "'>" + list_data->name + "</a><br>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, list_data->name);
          }
        }
      }
      else {
        if (full_path) {
          uint relative_dir1_len = relative_dir1.size();

          if ((int) relative_dir1_len + 1 <= force_width) {
            string relative_dir2 =
              CStrFmt::align(relative_dir1, force_width - 1, CSTR_FMT_ALIGN_RIGHT, ' ', true);

            if (html)
              output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
                     relative_dir2 + "</a><br>");
            else {
//            if (type_escape)
//              outputTypeEscape(list_data);

              outputColored(list_data->color, relative_dir2);
              outputColored(COLOR_CLIPPED   , ">");
            }
          }
          else {
            string relative_dir2 = relative_dir1 + "/" +
              CStrFmt::align(list_data->name, force_width - relative_dir1_len - 2,
                             CSTR_FMT_ALIGN_RIGHT, ' ', true);

            if (html)
              output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
                     relative_dir2 + "</a><br>");
            else {
//            if (type_escape)
//              outputTypeEscape(list_data);

              outputColored(list_data->color, relative_dir2);
              outputColored(COLOR_CLIPPED   , ">");
            }
          }
        }
        else {
          string relative_dir2 =
            CStrFmt::align(list_data->name, force_width - 1, CSTR_FMT_ALIGN_RIGHT, ' ', true);

          if (html)
            output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
                   relative_dir2 + "</a><br>");
          else {
//          if (type_escape)
//            outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
            outputColored(COLOR_CLIPPED   , ">");
          }
        }
      }

      outputLine("");
    }
  }
}

void
Cls::
set_max_filelen(vector<ClsFile *> &files)
{
  uint num_files = files.size();

  if (force_width == 0) {
    max_len = 0;

    for (uint i = 0; i < num_files; ++i) {
      uint len = files[i]->getLinkName().size();

      if ((int) len > max_len)
        max_len = len;
    }
  }
  else
    max_len = force_width;

  if (i_flag && ! l_flag)
    max_len += 8;

  if (s_flag && ! l_flag)
    max_len += 5;

  if (F_flag || p_flag)
    max_len++;

  max_size = 0;

  for (uint i = 0; i < num_files; ++i) {
    size_t size = files[i]->getSize();

    max_size = std::max(max_size, size);
  }

  max_size_len = uint(log10(max_size)) + 1;
}

void
Cls::
set_perm(string &str, int perm, int type)
{
  str = "";

  if (perm & S_IROTH)
    str += 'r';
  else
    str += '-';

  if (perm & S_IWOTH)
    str += 'w';
  else
    str += '-';

  if (perm & S_IXOTH)
    str += 'x';
  else
    str += '-';

  if (type == S_IRUSR && ((perm & S_ISUID) == S_ISUID)) {
    if (perm & S_IXOTH)
      str[2] = 's';
    else
      str[2] = 'S';
  }

  if (type == S_IRGRP && ((perm & S_ISGID) == S_ISGID)) {
    if (perm & S_IXOTH)
      str[2] = 's';
    else
      str[2] = 'S';
  }

  if (type == S_IROTH && ((perm & S_ISVTX) == S_ISVTX)) {
    if (perm & S_IXOTH)
      str[2] = 't';
    else
      str[2] = 'T';
  }
}

string
Cls::
type_to_string(int type)
{
  char c = (char) type;

  return string(&c, 1);
}

string
Cls::
size_to_string(ClsData *list_data)
{
  static char size_string[64];

  if (list_data->type == 'b' || list_data->type == 'c')
    sprintf(size_string, "%d,%d", major(list_data->rdev), minor(list_data->rdev));
  else {
    if      (M_flag)
      sprintf(size_string, "%.1fM", list_data->size/1024.0/1024.0);
    else if (k_flag)
      sprintf(size_string, "%ldK", list_data->size >> 10);
    else {
#if _FILE_OFFSET_BITS == 64
      sprintf(size_string, "%lld", (long long) list_data->size);
#else
      sprintf(size_string, "%ld", (long) list_data->size);
#endif
    }
  }

  return size_string;
}

string
Cls::
uid_to_string(int uid)
{
  if (! n_flag)
    return COSUser::getUserName(uid);
  else
    return CStrUtil::toString(uid);
}

string
Cls::
gid_to_string(int gid)
{
  if (! n_flag)
    return COSUser::getGroupName(gid);
  else
    return CStrUtil::toString(gid);
}

string
Cls::
inode_to_string(int inode)
{
  return CStrUtil::toString(inode);
}

string
Cls::
blocks_to_string(int inode)
{
  return CStrUtil::toString(inode);
}

string
Cls::
time_to_string(CTime *time)
{
  return time->getLsTime(show_secs);
}

string
Cls::
color_to_string(ClsColorType color)
{
  if (! use_colors)
    return "";

  return CTerm::getColorStr(color);
}

int
ClsCompareNames::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp;

  if (nocase)
    cmp = CStrUtil::casecmp(file1->getName(), file2->getName());
  else
    cmp = CStrUtil::cmp(file1->getName(), file2->getName());

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareTimes::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp;

  if      (ctime) {
    CTime *ctime1 = file1->getCTime();
    CTime *ctime2 = file2->getCTime();

    if (ctime1 != NULL && ctime2 != NULL)
      cmp = (int) ctime2->diff(*ctime1);
    else
      cmp = ctime1 - ctime2;
  }
  else if (utime) {
    CTime *atime1 = file1->getATime();
    CTime *atime2 = file2->getATime();

    if (atime1 != NULL && atime2 != NULL)
      cmp = (int) atime2->diff(*atime1);
    else
      cmp = atime1 - atime2;
  }
  else {
    CTime *mtime1 = file1->getMTime();
    CTime *mtime2 = file2->getMTime();

    if (mtime1 != NULL && mtime2 != NULL)
      cmp = (int) mtime2->diff(*mtime1);
    else
      cmp = mtime1 - mtime2;
  }

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareSizes::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp = file2->getSize() - file1->getSize();

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareExtensions::
operator()(ClsFile *file1, ClsFile *file2)
{
  string::size_type pos1 = file1->getName().find('.');
  string::size_type pos2 = file2->getName().find('.');

  if (pos1 != string::npos) {
    while (pos1 > 0 && file1->getName()[pos1 - 1] == '.')
      --pos1;

    if (pos1 == 0)
      pos1 = string::npos;
  }

  if (pos2 != string::npos) {
    while (pos2 > 0 && file2->getName()[pos2 - 1] == '.')
      --pos2;

    if (pos2 == 0)
      pos2 = string::npos;
  }

  int cmp;

  if      (pos1 != string::npos && pos2 != string::npos) {
    cmp = CStrUtil::cmp(file1->getName().substr(pos1 + 1), file2->getName().substr(pos2 + 1));

    if (cmp == 0)
      cmp = CStrUtil::cmp(file1->getName(), file2->getName());
  }
  else if (pos1 != string::npos)
    cmp =  1;
  else if (pos2 != string::npos)
    cmp = -1;
  else
    cmp = CStrUtil::cmp(file1->getName(), file2->getName());

  return (cmp < 0);
}

void
Cls::
split_files(const vector<ClsFile *> &files, vector<ClsFile *> &dfiles, vector<ClsFile *> &rfiles)
{
  uint num_files = files.size();

  for (uint i = 0; i < num_files; ++i) {
    ClsFilterType filter = filter_file(files[i]);

    if (filter == FILTER_OUT)
      continue;

    if (filter == FILTER_DIR)
      files[i]->setIsOutput(false);

    if (f_flag || files[i]->isDir())
      dfiles.push_back(files[i]);
    else
      rfiles.push_back(files[i]);
  }

  if (! f_flag) {
    sort_files(dfiles);
    sort_files(rfiles);
  }
}

void
Cls::
sort_files(vector<ClsFile *> &files)
{
  if      (sort_type == SORT_NAME)
    std::sort(files.begin(), files.end(), ClsCompareNames(nocase, r_flag));
  else if (sort_type == SORT_TIME)
    std::sort(files.begin(), files.end(), ClsCompareTimes(c_flag, u_flag, m_flag, r_flag));
  else if (sort_type == SORT_SIZE)
    std::sort(files.begin(), files.end(), ClsCompareSizes(r_flag));
  else if (sort_type == SORT_EXTENSION)
    std::sort(files.begin(), files.end(), ClsCompareExtensions());
}

void
Cls::
output_files(vector<ClsFile *> &files)
{
  if       (C_flag) {
    vector<ClsFile *> files1;

    uint num_files = files.size();

    for (uint i = 0; i < num_files; ++i)
      if (files[i]->getIsOutput())
        files1.push_back(files[i]);

    int columns = (list_max_pos + 1)/(max_len + 1);

    if (columns <= 0)
      columns = 1;

    uint num_files1 = files1.size();

    int rows = num_files1/columns;

    if (num_files1 % columns != 0)
      rows++;

    for (int i = 0; i < rows; ++i) {
      uint k = i;

      for (int j = 0; j < columns; ++j) {
        if (k >= num_files1)
          break;

        if (j > 0) {
          if (! quiet)
            output(" ");
        }

        list_file(files1[k]);

        k += rows;
      }

      if (! quiet) {
        if (html)
          output("<br>");

        outputLine("");
      }
    }
  }
  else if (x_flag) {
    int columns;

    if (num_columns == 0) {
      columns = (list_max_pos + 1)/(max_len + 1);

      if (columns <= 0)
        columns = 1;
    }
    else
      columns = num_columns;

    uint num_files = files.size();

    int j = 0;

    for (uint i = 0; i < num_files; ++i) {
      if (! files[i]->getIsOutput())
        continue;

      if (j > 0) {
        if (! quiet)
          output(" ");
      }

      list_file(files[i]);

      ++j;

      if (j == columns) {
        if (! quiet) {
          if (html)
            output("<br>");

          outputLine("");
        }

        j = 0;
      }
    }

    if (j > 0) {
      if (! quiet) {
        if (html)
          output("<br>");

        outputLine("");
      }
    }
  }
  else if (T_flag) {
    uint num_files = files.size();

    for (uint i = 0; i < num_files; ++i) {
      if (! files[i]->getIsOutput()) continue;

      for (int j = 0; j < dir_depth - 1; ++j)
        output(" ");

      output("|- ");

      list_file(files[i]);
    }
  }
  else {
    uint num_files = files.size();

    for (uint i = 0; i < num_files; ++i) {
      if (! files[i]->getIsOutput()) continue;

      list_file(files[i]);
    }
  }

  if (m_flag && list_pos > 0) {
    if (! quiet) {
      if (html)
        output("<br>");

      outputLine("");
    }

    list_pos = 0;
  }
}

string
Cls::
encode_name(const string &name)
{
  uint len = name.size();

  string name1;

  uint i = 0;

  if (no_path) {
    int i1 = 0;

    while (i < len) {
      if (name[i] == '/')
        i1 = i + 1;

      ++i;
    }

    i = i1;
  }

  while (i < len) {
    if (isgraph(name[i]))
      name1 += name[i];
    else {
      if      (b_flag) {
        name1 += '\\';

        int c = name[i];

        int digit1 = c/64;
        int digit2 = (c - digit1*64)/8;
        int digit3 = c - digit1*64 - digit2*8;

        name1 += digit1 + '0';
        name1 += digit2 + '0';
        name1 += digit3 + '0';
      }
      else if (q_flag)
        name1 += '?';
      else
        name1 += name[i];
    }

    ++i;
  }

  return name1;
}

ClsFileType
Cls::
decode_type_char(const string &opt, int c)
{
  ClsFileType type = FILE_TYPE_NONE;

  switch (c) {
    case 'p':
      type = FILE_TYPE_FIFO;
      break;
    case 'c':
      type = FILE_TYPE_CHR;
      break;
    case 'd':
      type = FILE_TYPE_DIR;
      break;
    case 'b':
      type = FILE_TYPE_BLK;
      break;
    case 'f':
      type = FILE_TYPE_REG;
      break;
    case 'l':
      type = FILE_TYPE_LNK;
      break;
    case 's':
      type = FILE_TYPE_SOCK;
      break;
    case 'x':
      type = FILE_TYPE_EXEC;
      break;
    case 'X':
      type = FILE_TYPE_ELF;
      break;
    case 'B':
      type = FILE_TYPE_BAD;
      break;
    case 'S':
      type = FILE_TYPE_SPECIAL;
      break;
    default:
      cerr << "Invalid --" << opt << " type " << (char) c << endl;
      break;
  }

  return type;
}

bool
Cls::
enter_dir(const string &dir)
{
  if (! CDir::enter(dir)) {
    cerr << CDir::getErrorMsg() << endl;
    return false;
  }

  ++dir_depth;

  current_dir = CDir::getCurrent();

  uint base_dir_len    = base_dir.size();
  uint current_dir_len = current_dir.size();

  if (current_dir_len >= base_dir_len &&
      current_dir.substr(0, base_dir_len) == base_dir) {
    int offset = base_dir_len;

    while (current_dir[offset] == '/')
      offset++;

    relative_dir = current_dir.substr(offset);

    if (relative_dir != "")
      relative_dir1 = "./" + relative_dir;
    else
      relative_dir1 = ".";
  }
  else {
    relative_dir  = current_dir;
    relative_dir1 = current_dir;
  }

  return true;
}

void
Cls::
leave_dir()
{
  if (! CDir::leave()) {
    cerr << CDir::getErrorMsg() << endl;
    return;
  }

  --dir_depth;

  current_dir = CDir::getCurrent();
}

string
Cls::
exec_to_string(const string &command)
{
  static string exec_string;

  exec_string = "";

  FILE *fp = popen(command.c_str(), "r");

  if (fp == NULL) return "";

  int c;

  while ((c = fgetc(fp)) != EOF)
    exec_string += c;

  (void) pclose(fp);

  uint len = exec_string.size();

  for (uint i = 0; i < len; ++i)
    if (exec_string[i] == '\n')
      exec_string = ' ';

  while (len > 0 && isspace(exec_string[len - 1]))
    len--;

  exec_string = exec_string.substr(0, len);

  return exec_string;
}

bool
Cls::
special_glob_match(const string &file, ClsColorType *color)
{
  for (uint i = 0; i <= 6; ++i) {
    uint num_special_globs = special_globs[i].size();

    *color = (ClsColorType) i;

    for (uint j = 0; j < num_special_globs; ++j)
      if (special_globs[i][j]->compare(file))
        return true;
  }

  return false;
}

int
Cls::
exec_file(ClsFile *file, const string &exec_cmd)
{
  string exec_cmd1;

  uint exec_cmd_len = exec_cmd.size();

  string::size_type pos1 = 0;
  string::size_type pos2 = exec_cmd.find('%');

  while (pos2 != string::npos) {
    exec_cmd1 += exec_cmd.substr(pos1, pos2 - pos1);

    ++pos2;

    char code = exec_cmd[pos2];

    switch (code) {
      case 'd':
        exec_cmd1 += current_dir;
        break;
      case 'f':
        exec_cmd1 += file->getName();
        break;
      case 'l':
        exec_cmd1 += file->getLinkName();
        break;
      case 'p':
        exec_cmd1 += current_dir;
        exec_cmd1 += "/";
        exec_cmd1 += file->getName();
        break;
      case '%':
        exec_cmd1 += "%";
        break;
      default:
        cerr << "Bad exec % code" << endl;
        break;
    }

    if (pos2 < exec_cmd_len)
      ++pos2;

    pos1 = pos2;
    pos2 = exec_cmd.find(pos1, '%');
  }

  exec_cmd1 += exec_cmd.substr(pos1);

  if (::system(exec_cmd1.c_str()) < 0) {
    cerr << "exec failed for " << exec_cmd1 << endl;
    return 1;
  }

  return 0;
}

#if 0
void
Cls::
outputTypeEscape(ClsData *list_data)
{
  outputTypeEscape(list_data, list_data->name);
}

void
Cls::
outputTypeEscape(ClsData *list_data, const string &str)
{
  CFileType type = get_data_type(list_data->file);

  outputTypeEscape(type, str);
}

void
Cls::
outputTypeEscape(CFileType type, const string &str)
{
  string type_str = CFileUtil::getTypeStr(type);

  if (type_str != "??")
    output(CEscape::APC("<link type=\"" + type_str + "\"" + " dir=\"" + current_dir + "\"" +
                        " name=\"" + str + "\"/>"));
}
#endif

#if 0
CFileType
Cls::
get_data_type(ClsFile *file)
{
  if      (file->isFIFO   ())
    return CFILE_TYPE_INODE_FIFO;
  else if (file->isChar   ())
    return CFILE_TYPE_INODE_CHR;
  else if (file->isDir    ())
    return CFILE_TYPE_INODE_DIR;
  else if (file->isBlock  ())
    return CFILE_TYPE_INODE_BLK;
  else if (file->isLink   ())
    return CFILE_TYPE_INODE_LNK;
  else if (file->isSocket ())
    return CFILE_TYPE_INODE_SOCK;

  CFileType type = CFileUtil::getType(file->getName());

  return type;
}
#endif

#if 0
string
Cls::
get_data_type_str(ClsFile *file)
{
  CFileType type = get_data_type(file);

  string type_str = CFileUtil::getTypeStr(type);

  return type_str + ']';
}
#endif

void
Cls::
outputColored(ClsColorType color, const string &str)
{
  if (use_colors)
    output(color_to_string(color) + str + color_to_string(COLOR_NORMAL));
  else
    output(str);
}

void
Cls::
outputLine(const string &str)
{
  if (! quiet)
    cout << str << endl;
}

void
Cls::
output(const string &str)
{
  if (! quiet)
    cout << str;
}

//------------

ClsFilterData::
ClsFilterData() :
 filtered_(false), includeFlags_(0), excludeFlags_(0), show_zero_(true), show_non_zero_(true),
 only_user_(false), newer_(-1), older_(-1), larger_(-1), smaller_(-1)
{
}

#if 0
void
ClsFilterData::
addIncludeFileType(const string &fileType)
{
  includeFileTypes_.push_back(new CGlob(fileType));

  filtered_ = true;
}
#endif

#if 0
void
ClsFilterData::
addExcludeFileType(const string &fileType)
{
  excludeFileTypes_.push_back(new CGlob(fileType));

  filtered_ = true;
}
#endif

void
ClsFilterData::
addMatch(const string &pattern)
{
  match_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

void
ClsFilterData::
addNoMatch(const string &pattern)
{
  nomatch_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

bool
ClsFilterData::
checkFile(Cls *cls, ClsFile *file) const
{
  if (! filtered_)
    return true;

  //-----

  if (! show_zero_ || ! show_non_zero_) {
    size_t size = file->getSize();

    if ((size == 0 && ! show_zero_) ||
        (size != 0 && ! show_non_zero_))
      return false;
  }

  //-----

  if (only_user_) {
    if (file->getUid() != cls->getUserUid())
      return false;
  }

  //-----

  // if not included fail
  if (! checkIncludeFile(cls, file))
    return false;

  //-----

  // if excluded fail
  if (  checkExcludeFile(cls, file))
    return false;

  //-----

  // if any match patterns it must match one
  uint num_match_patterns = match_patterns_.size();

  if (num_match_patterns > 0) {
    bool rc = false;

    for (uint i = 0; i < num_match_patterns; ++i) {
      if (match_patterns_[i]->compare(file->getName())) {
        rc = true;
        break;
      }
    }

    if (! rc)
      return false;
  }

  //-----

  // if any no-match patterns then fail if matches any
  uint num_nomatch_patterns = nomatch_patterns_.size();

  for (uint i = 0; i < num_nomatch_patterns; ++i) {
    if (nomatch_patterns_[i]->compare(file->getName()))
      return false;
  }

  //-----

  // if newer specified then fail if older
  if (newer_ >= 0) {
    long diff = (long) cls->getCurrentTime()->diff(*file->getMTime());

    if (diff > newer_*SECONDS_PER_DAY)
      return false;
  }

  //-----

  // if older specified then fail if newer
  if (older_ >= 0) {
    long diff = (long) cls->getCurrentTime()->diff(*file->getMTime());

    if (diff < older_*SECONDS_PER_DAY)
      return false;
  }

  //-----

  // if larger specified then fail if smaller
  if (larger_ >= 0) {
    if (file->getSize() < (uint) larger_)
      return false;
  }

  //-----

  // if smaller specified then fail if larger
  if (smaller_ >= 0) {
    if (file->getSize() > (uint) smaller_)
      return false;
  }

  //-----

  // run filter script/command
  if (exec_ != "") {
    int rc = cls->exec_file(file, exec_);

    if (rc)
      return false;
  }

  return true;
}

bool
ClsFilterData::
checkIncludeFile(Cls *cls, ClsFile *file) const
{
  if (includeFlags_ == 0 && includeFileTypes_.empty())
    return true;

  if ((file->isFIFO   () && (includeFlags_ & FILE_TYPE_FIFO)) ||
      (file->isChar   () && (includeFlags_ & FILE_TYPE_CHR )) ||
      (file->isDir    () && (includeFlags_ & FILE_TYPE_DIR )) ||
      (file->isBlock  () && (includeFlags_ & FILE_TYPE_BLK )) ||
      (file->isRegular() && (includeFlags_ & FILE_TYPE_REG )) ||
      (file->isLink   () && (includeFlags_ & FILE_TYPE_LNK )) ||
      (file->isSocket () && (includeFlags_ & FILE_TYPE_SOCK)))
    return true;

  if ((includeFlags_ & FILE_TYPE_EXEC) && file->isUserExecutable())
    return true;

  if ((includeFlags_ & FILE_TYPE_ELF) && file->isElf())
    return true;

  if ((includeFlags_ & FILE_TYPE_BAD) &&
      (cls->is_bad_file(file) || ! file->hasLinkStat()))
    return true;

  ClsColorType color;

  if ((includeFlags_ & FILE_TYPE_SPECIAL) &&
      cls->special_glob_match(file->getName(), &color))
    return true;

#if 0
  CFileType type = cls->get_data_type(file);

  string typeStr = CFileUtil::getTypeStr(type);

  PatternList::const_iterator p1 = includeFileTypes_.begin();
  PatternList::const_iterator p2 = includeFileTypes_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1)->compare(typeStr))
      return true;
#endif

  return false;
}

bool
ClsFilterData::
checkExcludeFile(Cls *cls, ClsFile *file) const
{
  if (excludeFlags_ == 0 && excludeFileTypes_.empty())
    return false;

  if ((file->isFIFO   () && (excludeFlags_ & FILE_TYPE_FIFO)) ||
      (file->isChar   () && (excludeFlags_ & FILE_TYPE_CHR )) ||
      (file->isDir    () && (excludeFlags_ & FILE_TYPE_DIR )) ||
      (file->isBlock  () && (excludeFlags_ & FILE_TYPE_BLK )) ||
      (file->isRegular() && (excludeFlags_ & FILE_TYPE_REG )) ||
      (file->isLink   () && (excludeFlags_ & FILE_TYPE_LNK )) ||
      (file->isSocket () && (excludeFlags_ & FILE_TYPE_SOCK)))
    return true;

  if ((excludeFlags_ & FILE_TYPE_EXEC) && file->isUserExecutable())
    return true;

  if ((excludeFlags_ & FILE_TYPE_ELF) && file->isElf())
    return true;

  if ((excludeFlags_ & FILE_TYPE_BAD) &&
      (cls->is_bad_file(file) || ! file->hasLinkStat()))
    return true;

  ClsColorType color;

  if ((excludeFlags_ & FILE_TYPE_SPECIAL) &&
      cls->special_glob_match(file->getName(), &color))
    return true;

#if 0
  CFileType type = cls->get_data_type(file);

  string typeStr = CFileUtil::getTypeStr(type);

  PatternList::const_iterator p1 = excludeFileTypes_.begin();
  PatternList::const_iterator p2 = excludeFileTypes_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1)->compare(typeStr))
      return true;
#endif

  return false;
}
