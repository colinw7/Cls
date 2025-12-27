#include <Cls.h>
#include <ClsHelp.h>
#include <ClsFilterData.h>

#include <COSUser.h>
#include <COSTerm.h>
#include <COSFile.h>
#include <COSProcess.h>
#include <CEnv.h>
#ifdef CTERM_SUPPORT
#include <CEscape.h>
#endif
#include <CStrUtil.h>
#include <CStrFmt.h>
#include <CDir.h>
#include <CFileUtil.h>
#include <CTime.h>
#include <CGlob.h>

#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <sys/sysmacros.h>

#include <algorithm>
#include <cassert>

#ifndef major
# define major(rdev) ((rdev) >> 8)
# define minor(rdev) ((rdev) & 0xFF)
#endif

class ClsCompareNames {
 public:
  ClsCompareNames(bool nocase_, bool reverse_, bool locale_) :
   nocase(nocase_), reverse(reverse_), locale(locale_) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);

 private:
  bool nocase  { false };
  bool reverse { false };
  bool locale  { false };
};

//---

class ClsCompareTimes {
 public:
  ClsCompareTimes(bool ctime_, bool utime_, bool mtime_, bool reverse_) :
   ctime(ctime_), utime(utime_), mtime(mtime_), reverse(reverse_) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);

 private:
  bool ctime   { false };
  bool utime   { false };
  bool mtime   { false };
  bool reverse { false };
};

//---

class ClsCompareSizes {
 public:
  explicit ClsCompareSizes(bool reverse_) :
   reverse(reverse_) {
  }

  int operator()(ClsFile *file1, ClsFile *file2);

 private:
  bool reverse { false };
};

//---

class ClsCompareExtensions {
 public:
  int operator()(ClsFile *file1, ClsFile *file2);
};

//-------------------

Cls::
Cls()
{
  init();
}

Cls::
~Cls()
{
  delete filterData_;
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
  h_flag = false;
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
  G_flag = false;
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
  sort_type       = ClsSortType::NAME;
  force_width     = 0;
  filterData_     = new ClsFilterData;
  my_umask        = COSUser::getUMask();
  full_path       = false;
  no_path         = false;
  show_secs       = false;
  rel_time        = false;
  nocase          = false;
  locale          = true;
  exec_init_cmd_  = "";
  exec_term_cmd_  = "";
  exec_cmd_       = "";

  setTestFlags ();
  setQuiet     (false);
  setHtml      (false);
  setTypeEscape(false);
  setDataType  (false);
  setBadNames  (false);
  setRenameBad (false);
  setPreview   (false);
  setSilent    (false);
  setUseColors (true);

  if (CEnvInst.exists("CTERM_VERSION"))
    setTypeEscape(true);

  num_columns = 0;

  lsFormat_ = "";

  max_len_  = -1;
  max_size_ = 0;

  if (! COSTerm::getCharSize(&screen_rows, &screen_cols)) {
    CEnvInst.get("COLUMNS", screen_cols);
    CEnvInst.get("LINES"  , screen_rows);
  }

  if (screen_cols <= 0) screen_cols = 80;
  if (screen_rows <= 0) screen_rows = 60;

  if (CEnvInst.exists("CLS_DEBUG_SCREEN_SIZE"))
    std::cerr << "Columns: " << screen_cols << " Rows: " << screen_rows << "\n";

  list_pos     = 0;
  list_max_pos = screen_cols - 1;

  base_dir = CDir::getCurrent();

  current_time_ = new CTime();

  if (my_umask != 0022 && my_umask != 0002)
    std::cerr << "Bad umask " << my_umask << "\n";

  readFiles_ = false;
}

void
Cls::
processArgs(int argc, char **argv)
{
  bool allowOptions = true;

  for (int i = 1; i < argc; ++i) {
    if (allowOptions && argv[i][0] == '-') {
      if (argv[i][1] != '-') {
        if (argv[i][1] == '\0')
          readFiles_ = true;

        for (int j = 1; argv[i][j] != '\0'; ++j) {
          switch (argv[i][j]) {
            case 'a':
              a_flag = true;
              A_flag = false;
              break;
            case 'b':
              b_flag = true;
              q_flag = false;
              break;
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

              sort_type = ClsSortType::NONE;

              break;
            case 'g':
              g_flag = true;
              l_flag = true;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              T_flag = false;
              break;
            case 'h':
              h_flag = true;
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
              sort_type = ClsSortType::TIME;
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
              filterData_->setShowNonZero(false);
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
            case 'G':
              G_flag = true;
              break;
            case 'K':
              K_flag = true;
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
              sort_type = ClsSortType::SIZE;
              break;
            case 'T':
              T_flag = true;
              l_flag = false;
              m_flag = false;
              x_flag = false;
              C_flag = false;
              break;
            case 'X':
              sort_type = ClsSortType::EXTENSION;
              break;
            case 'Z':
              filterData_->setShowZero(false);
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
              std::cerr << usage << "\n";

              exit(0);
            default:
              std::cerr << "Invalid switch -" << argv[i][j] << "\n";
              break;
          }
        }
      }
      else {
        if (argv[i][2] == '\0') {
          allowOptions = false;
          continue;
        }

        std::string arg = std::string(&argv[i][2]);

        if      (arg == "format") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --format\n";
            continue;
          }

          lsFormat_ = argv[i];
        }
        else if (arg == "color" || arg == "colour") {
          setUseColors(true);
        }
        else if (arg == "nocolor" || arg == "nocolour") {
          setUseColors(false);
        }
        else if (arg.substr(0, 7) == "special") {
          ClsColorType color = ClsColorType::SPECIAL;

          if (argv[i][9] != '\0') {
            int color_num = atoi(&argv[i][9]);

            if      (color_num == 0) color = ClsColorType::NORMAL;
            else if (color_num == 1) color = ClsColorType::DIRECTORY;
            else if (color_num == 2) color = ClsColorType::EXECUTABLE;
            else if (color_num == 3) color = ClsColorType::LINK;
            else if (color_num == 4) color = ClsColorType::PIPE;
            else if (color_num == 5) color = ClsColorType::SPECIAL;
            else if (color_num == 6) color = ClsColorType::BAD_FILE;
            else                     color = ClsColorType::BAD_FILE;
          }

          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --special\n";
            continue;
          }

          auto *glob = new CGlob(argv[i]);

          special_globs[int(color)].push_back(glob);
        }
        else if (arg == "nlink") {
          nlink_flag = true;
        }
        else if (arg == "user") {
          filterData_->setOnlyUser(true);
        }
        else if (arg == "links") {
          show_links = true;
        }
        else if (arg == "nolinks") {
          show_links = false;
        }
        else if (arg == "empty_dirs") {
          show_empty_dirs = true;
        }
        else if (arg == "bad") {
          show_bad       = true;
          show_bad_user  = true;
          show_bad_other = true;
        }
        else if (arg == "mybad") {
          show_bad       = true;
          show_bad_user  = true;
          show_bad_other = false;
        }
        else if (arg == "nobad") {
          show_bad       = false;
          show_bad_user  = false;
          show_bad_other = false;
        }
        else if (arg == "zero") {
          filterData_->setShowNonZero(false);
        }
        else if (arg == "nonzero") {
          filterData_->setShowZero(false);
        }
        else if (arg == "width") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --width\n";
            continue;
          }

          force_width = atoi(argv[i]);

          if (force_width == 0) {
            std::cerr << "Invalid value for --width\n";
            continue;
          }
        }
        else if (arg == "clip_left") {
          clip_left = true;
        }
        else if (arg == "screen_cols") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --screen_cols\n";
            continue;
          }

          screen_cols = atoi(argv[i]);

          if (screen_cols <= 0) {
            std::cerr << "Invalid value for --screen_cols\n";
            continue;
          }
        }
        else if (arg == "sort") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --sort\n";
            continue;
          }

          std::string arg1 = argv[i];

          if      (arg1 == "none"     ) sort_type = ClsSortType::NONE;
          else if (arg1 == "name"     ) sort_type = ClsSortType::NAME;
          else if (arg1 == "time"     ) sort_type = ClsSortType::TIME;
          else if (arg1 == "size"     ) sort_type = ClsSortType::SIZE;
          else if (arg1 == "extension") sort_type = ClsSortType::EXTENSION;
          else {
            std::cerr << "Invalid value '" << argc << "' for --sort\n";
          }
        }
        else if (arg == "exclude_type") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --exclude_type\n";
            continue;
          }

          const char *exclude_type = argv[i];

          filterData_->addExcludeFileType(exclude_type);
        }
        else if (arg == "excl" || arg == "exclude") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --exclude\n";
            continue;
          }

          const char *exclude = argv[i];

          for (uint j = 0; exclude[j] != '\0'; ++j) {
            ClsFileType type = decodeTypeChar("exclude", exclude[j]);

            if (type != ClsFileType::NONE)
              filterData_->addExcludeType(static_cast<uint>(type));
          }
        }
        else if (arg == "include_type") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --include_type\n";
            continue;
          }

          const char *include_type = argv[i];

          filterData_->addIncludeFileType(include_type);
        }
        else if (arg == "incl" || arg == "include") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --include\n";
            continue;
          }

          const char *include = argv[i];

          for (uint j = 0; include[j] != '\0'; ++j) {
            ClsFileType type = decodeTypeChar("include", include[j]);

            if (type != ClsFileType::NONE)
              filterData_->addIncludeType(static_cast<uint>(type));
          }
        }
        else if (arg == "prefix") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --prefix\n";
            continue;
          }

          const char *prefix = argv[i];

          filterData_->addPrefix(prefix);
        }
        else if (arg == "newer") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --newer\n";
            continue;
          }

          filterData_->setNewer(atoi(argv[i]));
        }
        else if (arg == "older") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --older\n";
            continue;
          }

          filterData_->setOlder(atoi(argv[i]));
        }
        else if (arg == "larger") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --larger\n";
            continue;
          }

          filterData_->setLarger(decodeSizeArg(argv[i]));
        }
        else if (arg == "smaller") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --smaller\n";
            continue;
          }

          filterData_->setSmaller(decodeSizeArg(argv[i]));
        }
        else if (arg == "match" || arg == "nomatch") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --" << arg << "\n";
            continue;
          }

          std::vector<std::string> words;
          CStrUtil::addWords(argv[i], words);

          for (const auto &word : words) {
            if (arg == "match")
              filterData_->addMatch(word);
            else
              filterData_->addNoMatch(word);
          }
        }
        else if (arg == "dirmatch" || arg == "nodirmatch") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --" << arg << "\n";
            continue;
          }

          std::vector<std::string> words;
          CStrUtil::addWords(argv[i], words);

          for (const auto &word : words) {
            if (arg == "dirmatch")
              filterData_->addDirMatch(word);
            else
              filterData_->addNoDirMatch(word);
          }
        }
        else if (arg == "linkmatch" || arg == "nolinkmatch") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --" << arg << "\n";
            continue;
          }

          std::vector<std::string> words;
          CStrUtil::addWords(argv[i], words);

          for (const auto &word : words) {
            if (arg == "linkmatch")
              filterData_->addLinkMatch(word);
            else
              filterData_->addNoLinkMatch(word);
          }
        }
        else if (arg == "full_path" || arg == "fullpath") {
          full_path = true;
        }
        else if (arg == "no_path" || arg == "nopath") {
          no_path = true;
        }
        else if (arg == "show_secs") {
          show_secs = true;
        }
        else if (arg == "rel_time") {
          rel_time = true;
        }
        else if (arg == "case" || arg == "nocase") {
          nocase = (arg == "nocase");
        }
        else if (arg == "locale") {
          locale = true;
        }
        else if (arg == "nolocale") {
          locale = false;
        }
        else if (arg == "exec_init" || arg == "exec_term" || arg == "exec") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --" << arg << "\n";
            continue;
          }

          if      (arg == "exec_init")
            exec_init_cmd_ = argv[i];
          else if (arg == "exec_term")
            exec_term_cmd_ = argv[i];
          else
            exec_cmd_ = argv[i];
        }
        else if (arg == "filter") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --filter\n";
            continue;
          }

          filterData_->setExec(argv[i]);
        }
        else if (arg == "test" || arg == "testf") {
          ++i;

          if (i >= argc) {
            std::cerr << "Missing argument for --test\n";
            continue;
          }

          setTestFlags(argv[i], arg == "testf");
        }
        else if (arg == "quiet") {
          setQuiet(true);
        }
        else if (arg == "html") {
          setHtml(true);
        }
        else if (arg == "type_escape") {
          setTypeEscape(true);
        }
        else if (arg == "no_type_escape") {
          setTypeEscape(false);
        }
        else if (arg == "datatype" || arg == "data_type") {
          setDataType(true);
        }
        else if (arg == "bad_names") {
          setBadNames(true);
        }
        else if (arg == "rename_bad") {
          setRenameBad(true);
        }
        else if (arg == "preview") {
          setPreview(true);
        }
        else if (arg == "silent") {
          setSilent(true);
        }
        else if (arg == "h" || arg == "help") {
          std::cerr << usage << "\n";
          exit(0);
        }
        else if (arg == "help_format") {
          std::cerr << "%b : file blocks\n";
          std::cerr << "%c : specified color\n";
          std::cerr << "%d : add date\n";
          std::cerr << "%e : exec command with file\n";
          std::cerr << "%f : add filename\n";
          std::cerr << "%g : add group name\n";
          std::cerr << "%i : add inode number\n";
          std::cerr << "%l : add link number\n";
          std::cerr << "%n : add number of links\n";
          std::cerr << "%p : add permission\n";
          std::cerr << "%s : add size\n";
          std::cerr << "%t : add type\n";
          std::cerr << "%u : add user name\n";
          std::cerr << "%D : add relative directory\n";
          std::cerr << "%G : add gid\n";
          std::cerr << "%L : add link indicator\n";
          std::cerr << "%U : add uid\n";
          exit(0);
        }
        else if (arg == "help_file_type") {
          std::cerr << "p: FIFO\n";
          std::cerr << "c: CHR\n";
          std::cerr << "d: DIR\n";
          std::cerr << "b: BLK\n";
          std::cerr << "f: REG\n";
          std::cerr << "l: LNK\n";
          std::cerr << "s: SOCK\n";
          std::cerr << "x: EXEC\n";
          std::cerr << "X: ELF\n";
          std::cerr << "B: BAD\n";
          std::cerr << "S: SPECIAL\n";
          exit(0);
        }
        else if (arg == "help_exec") {
          std::cerr << "%d : add directory\n";
          std::cerr << "%f : add filename\n";
          std::cerr << "%g : add group name\n";
          std::cerr << "%l : add link name\n";
          std::cerr << "%p : add full path\n";
          std::cerr << "%P : add permissions\n";
          std::cerr << "%s : add size\n";
          std::cerr << "%t : add change (c), modify (m) or access (a) time\n";
          std::cerr << "%u : add user name\n";
          exit(0);
        }
        else {
          std::cerr << "Invalid switch --" << arg << "\n";
        }
      }
    }
    else {
      auto *file = new ClsFile(this, L_flag, argv[i]);

      files_.push_back(file);
    }
  }
}

long
Cls::
decodeSizeArg(const std::string &arg) const
{
  auto string_to_size = [](const std::string &str) {
    auto i = std::stoi(str);
    if (i < 0) return long(-1);
    return long(i);
  };

  auto len = arg.size();

  if (len < 2)
    return string_to_size(arg);

  if      (arg[len - 1] == 'k')
    return string_to_size(arg.substr(0, len - 1))*1024;
  else if (arg[len - 1] == 'm')
    return string_to_size(arg.substr(0, len - 1))*1024*1024;
  else if (arg[len - 1] == 'g')
    return string_to_size(arg.substr(0, len - 1))*1024*1024*1024;
  else
    return string_to_size(arg);
}

void
Cls::
setTestFlags(const std::string &str, bool names)
{
  testNames_ = names;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if      (str[i] == 'b') testFlags_ |= uint(ClsFileType::BLK);
    else if (str[i] == 'c') testFlags_ |= uint(ClsFileType::CHR);
    else if (str[i] == 'd') testFlags_ |= uint(ClsFileType::DIR);
    else if (str[i] == 'e') testFlags_ |= uint(ClsFileType::EXISTS);
    else if (str[i] == 'f') testFlags_ |= uint(ClsFileType::REG);
    // g : set-group-ID
    // G : owned by the effective group ID
    else if (str[i] == 'h') testFlags_ |= uint(ClsFileType::LNK);
    // k : sticky bit set
    else if (str[i] == 'L') testFlags_ |= uint(ClsFileType::LNK);
    // O : owned by the effective user ID
    else if (str[i] == 'p') testFlags_ |= uint(ClsFileType::FIFO);
    // r : readable
    // s : size greater than zero
    else if (str[i] == 'S') testFlags_ |= uint(ClsFileType::SOCK);
    // t : is tty
    // u : set-user-ID bit is sets tty
    // w : writable
    else if (str[i] == 'x') testFlags_ |= uint(ClsFileType::EXEC);

    else if (str[i] == 'B') testFlags_ |= uint(ClsFileType::BAD);
    else if (str[i] == 'E') testFlags_ |= uint(ClsFileType::ELF);
    else if (str[i] == 'G') testFlags_ |= uint(ClsFileType::SPECIAL); // glob special

    else {
      std::cerr << "Invalid test flag '" << str[i] << "'\n";
    }
  }
}

bool
Cls::
exec()
{
  // read filenames fron stdin
  if (readFiles_) {
    // read input
    std::string file_string;

    int c = fgetc(stdin);

    while (c != EOF) {
      if (isspace(c))
        c = ' ';

      file_string += char(c);

      c = fgetc(stdin);
    }

    //----

    // remove colors from input string
    if (isUseColors()) {
      static uint NUM_COLORS = 7;

      for (uint i = 0; i < NUM_COLORS; ++i) {
        std::string color = colorToString(static_cast<ClsColorType>(i));

        auto color_len = color.size();

        auto pos = file_string.find(color);

        if (pos != std::string::npos) {
          std::string file_string1 = file_string.substr(0, pos);

          for (uint j = 0; j < color_len; ++j)
            file_string1 += " ";

          file_string1 += file_string.substr(pos + color_len);
        }
      }
    }

    //----

    // split into word (filenames)
    std::vector<std::string> words;

    CStrUtil::addWords(file_string, words);

    for (const auto &word : words) {
      auto *file = new ClsFile(this, L_flag, word);

      files_.push_back(file);
    }
  }

  //----

  // set encoded name (remove special characters)
  for (const auto &file : files_)
    file->setLinkName(encodeName(file->getName()));

  //----

  // if has output format then turn off other format flags
  if (lsFormat_ != "") {
    l_flag = false;
    m_flag = false;
    x_flag = false;
    C_flag = false;
    T_flag = false;
  }

  //----

  // if column output set current position and max position
  if (C_flag || x_flag || m_flag) {
    list_pos     = 0;
    list_max_pos = screen_cols;
  }

  //----

  // output header for html
  if (isHtml()) {
    outputLine("<html>");
    outputLine("<head>");
    outputLine("</head>");
    outputLine("<body>");
    outputLine("<pre>");
  }

  //----

  // run init command if specified
  if (exec_init_cmd_ != "") {
    std::string res;
    int         status;

    runCommand(exec_init_cmd_, res, status);
  }

  //----

  // start at current directory
  dir_depth = 0;

  enterDir(".");

  //----

  // output specified files
  bool rc = true;

  // split input files into directory and regular files
  // (if no files then just add current directory)
  FileArray dfiles, rfiles;

  if (files_.empty()) {
    auto *file = new ClsFile(this, L_flag, ".", ".");

    dfiles.push_back(file);
  }
  else {
    splitFiles(files_, dfiles, rfiles);
  }

  //---

  // set max file len for regular files and output
  setMaxFilelen(rfiles);

  if (! outputFiles(rfiles))
    rc = false;

  //---

  // set max file len for directory files and output
  setMaxFilelen(dfiles);

  if (d_flag) {
    // output as normal files for -d flag
    if (! outputFiles(dfiles))
      rc = false;
  }
  else {
    // if single directory and no regular files then list directory
    auto num_dfiles = dfiles.size();
    auto num_rfiles = rfiles.size();

    if (num_dfiles == 1 && num_rfiles == 0) {
      listDir(dfiles[0]);
    }
    else {
      // output each directory contents
      for (const auto &dfile : dfiles)
        listDirEntry(dfile);
    }
  }

  //---

  // run term command if specified
  if (exec_term_cmd_ != "") {
    std::string res;
    int         status;

    runCommand(exec_term_cmd_, res, status);
  }

  //---

  // output html footer
  if (isHtml()) {
    outputLine("</pre>");
    outputLine("</body>");
    outputLine("</html>");
  }

  //---

  leaveDir();

  return rc;
}

bool
Cls::
testFiles(const FileArray &files, FileSet &fileSet)
{
  auto testAddFile = [&](bool b, ClsFile *file) {
    if (b) fileSet.insert(file);
  };

  for (const auto &file : files) {
    if (testFlags_ & uint(ClsFileType::FIFO))
      testAddFile(file->isFIFO(), file);

    if (testFlags_ & uint(ClsFileType::CHR))
      testAddFile(file->isChar(), file);

    if (testFlags_ & uint(ClsFileType::DIR))
      testAddFile(file->isDir(), file);

    if (testFlags_ & uint(ClsFileType::BLK))
      testAddFile(file->isBlock(), file);

    if (testFlags_ & uint(ClsFileType::REG))
      testAddFile(file->isRegular(), file);

    if (testFlags_ & uint(ClsFileType::LNK))
      testAddFile(file->isLink(), file);

    if (testFlags_ & uint(ClsFileType::SOCK))
      testAddFile(file->isSocket(), file);

    if (testFlags_ & uint(ClsFileType::BAD))
      testAddFile(isBadFile(file) || isBadLink(file), file);

    if (testFlags_ & uint(ClsFileType::EXISTS))
      testAddFile(file->exists(), file);
  }

  return ! fileSet.empty();
}

bool
Cls::
listDirEntry(ClsFile *file)
{
  bool rc = true;

  if (! file->isDir())
    return false;

  if (! enterDir(file->getName()))
    return false;

  FileArray files;

  if (! getDirFiles(file, files))
    rc = false;

  if (show_empty_dirs) {
    if (files.empty()) {
      if (isHtml()) {
        output("<a href='" + relative_dir1 + "'>" + relative_dir1 + "</a><br>");
      }
      else {
        if (isTypeEscape())
          outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

        outputColored(ClsColorType::DIRECTORY, relative_dir1);

        outputLine("");
      }
    }
  }
  else {
    int num_files1 = 0;

    for (const auto &file1 : files) {
      if (file1->getIsOutput())
        ++num_files1;
    }

    if (num_files1 > 0) {
      if (! isQuiet()) {
        if (! full_path) {
          if (isHtml())
            output("<br>");

          if (! T_flag)
            outputLine("");
        }

        if (! full_path) {
          if (isHtml()) {
            output("<a href='" + relative_dir1 + "'>" + relative_dir1);
          }
          else {
            if (T_flag) {
              for (int j = 0; j < dir_depth - 1; ++j)
                output(" ");

              output("|- ");
            }

            if (isTypeEscape())
              outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

            outputColored(ClsColorType::DIRECTORY, relative_dir1);
          }

          output(":");

          if (isHtml())
            output("</a><br>");

          outputLine("");
        }
      }

      if (! listDirFiles(files))
        rc = false;
    }
  }

  if (R_flag)
    recurseFiles(files);

  freeDirFiles(files);

  leaveDir();

  return rc;
}

bool
Cls::
listDir(ClsFile *file)
{
  bool rc = true;

  if (! enterDir(file->getName()))
    return false;

  if (! listCurrentDir(file))
    rc = false;

  leaveDir();

  return rc;
}

bool
Cls::
listCurrentDir(ClsFile *file)
{
  bool rc = true;

  FileArray files;

  if (! getDirFiles(file, files))
    rc = false;

  if (! listDirFiles(files))
    rc = false;

  if (R_flag)
    recurseFiles(files);

  freeDirFiles(files);

  return rc;
}

bool
Cls::
getDirFiles(ClsFile *, FileArray &files)
{
  std::vector<std::string> dir_files;

  CDir dir(".");

  if (! dir.getFilenames(dir_files)) {
    if (! isSilent())
      std::cerr << ".: " << dir.getErrorMsg() << "\n";
    return false;
  }

  if (a_flag) {
    addDirFiles("." , files);
    addDirFiles("..", files);
  }

  for (const auto &dir_file : dir_files) {
    if (dir_file[0] == '.') {
      if (! a_flag && ! A_flag)
        continue;
    }

    addDirFiles(dir_file, files);
  }

  return true;
}

void
Cls::
addDirFiles(const std::string &name, FileArray &files)
{
  auto *file = new ClsFile(this, L_flag, name, encodeName(name));

  ClsFilterType filter = filterFile(file);

  // skip filter out file
  if (filter == ClsFilterType::OUT) {
    delete file;
    return;
  }

  // mark filter out recurse directory
  if (filter == ClsFilterType::OUT_DIR)
    file->setIsOutput(false);

  // add directory file to process
  files.push_back(file);
}

void
Cls::
splitFiles(const FileArray &files, FileArray &dfiles, FileArray &rfiles)
{
  for (const auto &file : files) {
    ClsFilterType filter = filterFile(file);

    // skip filter out file
    if (filter == ClsFilterType::OUT)
      continue;

    // mark filter out recurse directory
    if (filter == ClsFilterType::OUT_DIR)
      file->setIsOutput(false);

    // add file to process
    if (f_flag || file->isDir())
      dfiles.push_back(file);
    else
      rfiles.push_back(file);
  }

  // sort unless -f flag specified
  if (! f_flag) {
    sortFiles(dfiles);
    sortFiles(rfiles);
  }
}

ClsFilterType
Cls::
filterFile(ClsFile *file)
{
  // if filter allows file then keep
  if (filterData_->checkFile(this, file))
    return ClsFilterType::IN;

  // if filter doesn't not allow file but directory and allow process children
  if (R_flag && file->isDir()) {
    if (filterData_->checkDirFile(this, file))
      return ClsFilterType::OUT_DIR;
  }

  return ClsFilterType::OUT;
}

bool
Cls::
listDirFiles(FileArray &files)
{
  if (show_empty_dirs) {
    if (files.empty()) {
      if (isHtml()) {
        output("<a href='" + relative_dir1 + "'>" + relative_dir1 + "</a><br>");
      }
      else {
        if (isTypeEscape())
          outputTypeEscape(CFILE_TYPE_INODE_DIR, relative_dir1);

        outputColored(ClsColorType::DIRECTORY, relative_dir1);

        outputLine("");
      }
    }

    return true;
  }

  //------

  bool rc = true;

  if (C_flag || x_flag || m_flag) {
    list_pos     = 0;
    list_max_pos = screen_cols;
  }

  if (! f_flag)
    sortFiles(files);

  setMaxFilelen(files);

  if (! outputFiles(files))
    rc = false;

  return rc;
}

void
Cls::
recurseFiles(FileArray &files)
{
  for (const auto &file : files) {
    const std::string &name = file->getName();

    if (name[0] == '.') {
      if (! a_flag && ! A_flag)
        continue;

      if (name == "." || name == "..")
        continue;

      listDirEntry(file);
    }
    else
      listDirEntry(file);
  }
}

void
Cls::
freeDirFiles(FileArray &files)
{
  for (auto &file : files)
    delete file;
}

bool
Cls::
listFile(ClsFile *file)
{
  if (isBadNames() || isRenameBad()) {
    if (CFileUtil::isBadFilename(file->getName())) {
      std::string pathName = (full_path ? current_dir : relative_dir) + "/" + file->getName();

      if (isBadNames()) {
        if (! isQuiet())
          std::cerr << "Bad filename '" << pathName << "'\n";
        else
          std::cerr <<  pathName << "\n";
      }
      else {
        std::string newName     = CFileUtil::fixBadFilename(file->getName());
        std::string newPathName = (full_path ? current_dir : relative_dir) + "/" + newName;

        std::cerr << "mv '" << pathName << "' " << newPathName << "\n";
      }
    }
  }

  if (! file->exists())
    return false;

  ClsData list_data;

  list_data.file = file;

  list_data.type = typeChar(file);

  int u_perm = file->getUPerm();
  int g_perm = file->getGPerm();
  int o_perm = file->getOPerm();

  setPerm(list_data.u_perm, u_perm, S_IRUSR);
  setPerm(list_data.g_perm, g_perm, S_IRGRP);
  setPerm(list_data.o_perm, o_perm, S_IROTH);

  list_data.size   = file->getSize();
  list_data.uid    = file->getUid();
  list_data.gid    = file->getGid();
  list_data.ino    = file->getINode();
  list_data.nlink  = static_cast<int>(file->getNLink());
  list_data.dev    = static_cast<int>(file->getDev());
  list_data.rdev   = static_cast<int>(file->getRDev());
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

  if      (show_bad && isBadFile(file))
    list_data.color = ClsColorType::BAD_FILE;
  else if (list_data.type == 'd')
    list_data.color = ClsColorType::DIRECTORY;
  else if (list_data.type == 'p' || list_data.type == 's')
    list_data.color = ClsColorType::PIPE;
  else if (list_data.type == 'l') {
    if (file->hasLinkStat())
      list_data.color = ClsColorType::LINK;
    else
      list_data.color = ClsColorType::BAD_FILE;
  }
  else if (list_data.type == 'b' || list_data.type == 'c')
    list_data.color = ClsColorType::DEVICE;
  else if (file->isUserExecutable())
    list_data.color = ClsColorType::EXECUTABLE;
  else {
    ClsColorType color;

    if (specialGlobMatch(file->getName(), &color))
      list_data.color = color;
    else
      list_data.color = ClsColorType::NORMAL;
  }

  if (list_data.type == 'l') {
    list_data.link_name = file->getFullLinkName();

    if      (file->isDir())
      list_data.link_color = ClsColorType::DIRECTORY;
    else if (file->isFIFO() || file->isSocket())
      list_data.link_color = ClsColorType::PIPE;
    else if (file->isUserExecutable())
      list_data.link_color = ClsColorType::EXECUTABLE;
    else
      list_data.link_color = ClsColorType::NORMAL;
  }
  else
    list_data.link_name = "";

  if (! isQuiet())
    printListData(&list_data);

  if (exec_cmd_ != "")
    execFile(file, exec_cmd_);

  return true;
}

char
Cls::
typeChar(ClsFile *file) const
{
  if      (file->isFIFO   ()) return 'p';
  else if (file->isChar   ()) return 'c';
  else if (file->isDir    ()) return 'd';
  else if (file->isBlock  ()) return 'b';
  else if (file->isRegular()) return '-';
  else if (file->isLink   ()) return 'l';
  else if (file->isSocket ()) return 's';
  else                        return '?';
}

bool
Cls::
isBadFile(ClsFile *file) const
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

      if (o_perm > g_perm || (o_perm != 5 && o_perm != 0))
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

      if (o_perm > g_perm || (o_perm != 5 && o_perm != 4 && o_perm != 0))
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

bool
Cls::
isBadLink(ClsFile *file) const
{
  return (file->isLink() && ! file->hasLinkStat());
}

void
Cls::
printListData(ClsData *list_data)
{
  if (lsFormat_ != "") {
    uint i = 0;

    std::string output_string;

    auto len = lsFormat_.size();

    while (i < len) {
      if (lsFormat_[i] != '%' || i >= len - 1) {
        output_string += lsFormat_[i++];
        continue;
      }

      auto i1 = i;

      ++i;

      char justify = ' ';

      if (lsFormat_[i] == '-' || lsFormat_[i] == '+')
        justify = lsFormat_[i++];

      uint field_width = 0;

      while (i < len && isdigit(lsFormat_[i]))
        field_width = 10*field_width + uint(lsFormat_[i++] - '0');

      std::string lstr, str, rstr;
      bool        force = false;

      switch (lsFormat_[i]) {
        // file blocks
        case 'b': {
          if (field_width == 0)
            field_width = 6;

          str = blocksToString(list_data->blocks);

          break;
        }
        // specified color
        case 'c': {
          ++i;

          // get color number
          int color = 0;

          while (i < len && isdigit(lsFormat_[i]))
            color = 10*color + (lsFormat_[i++] - '0');

          // get color
          if (color >= 0 && color <= 6)
            str = COSTerm::getColorStr(color);
          else {
            str   = "";
            force = true;
          }

          --i;

          break;
        }
        // add date
        case 'd': {
          if (field_width == 0)
            field_width = 12;

          // add access date
          if      (lsFormat_[i + 1] == 'a') {
            str = timeToString(list_data->atime);

            ++i;
          }
          // add change date
          else if (lsFormat_[i + 1] == 'c') {
            str = timeToString(list_data->ctime);

            ++i;
          }
          // add modify date
          else if (lsFormat_[i + 1] == 'm') {
            str = timeToString(list_data->mtime);

            ++i;
          }
          // add date (use options to pick default type)
          else {
            if      (c_flag)
              str = timeToString(list_data->ctime);
            else if (u_flag)
              str = timeToString(list_data->atime);
            else
              str = timeToString(list_data->mtime);
          }

          break;
        }
        // exec command with file
        case 'e': {
          // get text to next space
          std::string temp_str1;

          while (i < len - 1 && ! isspace(lsFormat_[i + 1])) {
            if (lsFormat_[i + 1] == '\\' && i < len - 2)
              ++i;

            temp_str1 += lsFormat_[++i];
          }

          // add filename
          temp_str1 += ' ' + list_data->name;

          str = execToString(temp_str1);

          break;
        }
        // add filename
        case 'f': {
          if (field_width == 0 && max_len_ > 0)
            field_width = uint(max_len_);

          if (isHtml())
            str = list_data->name;
          else {
            lstr = colorToString(list_data->color);
            str  = list_data->name;
            rstr = colorToString(ClsColorType::NORMAL);
          }

          break;
        }
        // add fully specified filename
        case 'F': {
          if (field_width == 0 && max_len_ > 0)
            field_width = uint(max_len_);

          if (isHtml())
            str = relative_dir1 + "/" + list_data->name;
          else {
            lstr = colorToString(list_data->color);
            str  = relative_dir1 + "/" + list_data->name;
            rstr = colorToString(ClsColorType::NORMAL);
          }

          break;
        }
        // add gid
        case 'g': {
          if (field_width == 0)
            field_width = 8;

          str = gidToString(static_cast<int>(list_data->gid));

          break;
        }
        // add inode number
        case 'i': {
          if (field_width == 0)
            field_width = 8;

          str = inodeToString(list_data->ino);

          break;
        }
        // add link number
        case 'l': {
          if (list_data->link_name != "") {
            if (isHtml())
              str = list_data->link_name;
            else {
              lstr = colorToString(list_data->link_color);
              str  = list_data->link_name;
              rstr = colorToString(ClsColorType::NORMAL);
            }
          }
          else {
            str   = "";
            force = true;
          }

          break;
        }
        // add number of links
        case 'n': {
          if (field_width == 0)
            field_width = 4;

          str = CStrUtil::toString(list_data->nlink);

          break;
        }
        // add permission
        case 'p': {
          // add user permission
          if      (lsFormat_[i + 1] == 'u') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->u_perm;

            ++i;
          }
          // add group permission
          else if (lsFormat_[i + 1] == 'g') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->g_perm;

            ++i;
          }
          // add other permission
          else if (lsFormat_[i + 1] == 'o') {
            if (field_width == 0)
              field_width = 3;

            str = list_data->o_perm;

            ++i;
          }
          // add all permissions
          else {
            if (field_width == 0)
              field_width = 9;

            str = list_data->u_perm + list_data->g_perm + list_data->o_perm;
          }

          break;
        }
        // add size
        case 's': {
          if (field_width == 0)
            field_width = 8;

          str = sizeToString(list_data);

          break;
        }
        // add type
        case 't': {
          if (field_width == 0)
            field_width = 1;

          str = typeToString(list_data->type);

          break;
        }
        // add uid
        case 'u': {
          if (field_width == 0)
            field_width = 8;

          str = uidToString(static_cast<int>(list_data->uid));

          break;
        }
        // add relative directory
        case 'D': {
          if (field_width == 0)
            field_width = 0;

          str = relative_dir1;

          full_path = true; // force full path

          break;
        }
        // add gid
        case 'G': {
          if (field_width == 0)
            field_width = 6;

          str = CStrUtil::toString(list_data->gid);

          break;
        }
        // add link indicator
        case 'L': {
          if (list_data->link_name != "")
            str = "->";
          else {
            str   = "";
            force = true;
          }

          break;
        }
        // add uid
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

      // add text to output (justified)
      if (str != "" || force) {
        if (field_width > 0) {
          if      (justify == '+')
            output_string +=
              lstr + CStrFmt::align(str, field_width, CStrFmt::AlignType::RIGHT) + rstr;
          else if (justify == '-')
            output_string +=
              lstr + CStrFmt::align(str, field_width, CStrFmt::AlignType::LEFT ) + rstr;
          else
            output_string += lstr + str + rstr;
        }
        else
          output_string += str;
      }
      else {
        for (size_t k = i1; k < i; k++)
          output_string += lsFormat_[k];
      }
    }

    if (isHtml())
      outputLine(output_string + "<br>");
    else
      outputLine(output_string);
  }
  else {
    if (isDataType()) {
      std::string type = getDataTypeStr(list_data->file);

      output(CStrFmt::align(type, 12, CStrFmt::AlignType::LEFT));
    }

    if       (l_flag) {
      if (i_flag)
        CStrUtil::printf("%7ld ", list_data->ino);

      if (s_flag)
        CStrUtil::printf("%4ld ", list_data->blocks);

      if (! isDataType()) {
        char c = char(list_data->type);

        output(std::string(&c, 1));
      }

      output(list_data->u_perm);
      output(list_data->g_perm);
      output(list_data->o_perm);

      if (nlink_flag)
        CStrUtil::printf(" %3d", list_data->nlink);

      if (! o_flag)
        output(" " + CStrFmt::align(uidToString(static_cast<int>(list_data->uid)),
                                    8, CStrFmt::AlignType::LEFT));

      if (! g_flag)
        output(" " + CStrFmt::align(gidToString(static_cast<int>(list_data->gid)),
                                    8, CStrFmt::AlignType::LEFT));

      if (list_data->type == 'b' || list_data->type == 'c')
        CStrUtil::printf(" %3d,%3d", major(list_data->rdev), minor(list_data->rdev));
      else
        formatSize(list_data->size);

      std::string tstr;

      if      (c_flag)
        tstr = timeToString(list_data->ctime);
      else if (u_flag)
        tstr = timeToString(list_data->atime);
      else
        tstr = timeToString(list_data->mtime);

      output(" " + CStrFmt::align(tstr, 12, CStrFmt::AlignType::RIGHT));

      output(" ");

      std::string name = list_data->name;

      if (full_path)
        name = relative_dir1 + "/" + list_data->name;

      auto len = name.size();

      if (force_width > 0 && len > uint(force_width)) {
        if (clip_left)
          name = "<" + CStrFmt::align(name, uint(force_width - 1), CStrFmt::AlignType::LEFT, ' ',
                                      CStrFmt::ClipType::LEFT);
        else
          name = CStrFmt::align(name, uint(force_width - 1), CStrFmt::AlignType::LEFT, ' ',
                                CStrFmt::ClipType::RIGHT) + ">";
      }

      if (isHtml()) {
        output("<a href='" + relative_dir1 + "/" + name + "'>" + name + "</a>");
      }
      else {
        if (isTypeEscape())
          outputTypeEscape(list_data);

        outputColored(list_data->color, name);
      }

      if (show_links && list_data->link_name != "") {
        output(" -> ");

        if (isHtml()) {
          output("<a href='" + relative_dir1 + "/" + list_data->link_name + "'>" +
                 list_data->link_name + "</a>");
        }
        else {
          if (isTypeEscape())
            outputTypeEscape(list_data, list_data->link_name);

          outputColored(list_data->link_color, list_data->link_name);
        }
      }

      if (isHtml())
        output("<br>");

      outputLine("");
    }
    else if (C_flag || x_flag) {
      auto len = list_data->name.size();

      if (full_path)
        len += relative_dir1.size() + 1;

      if (force_width == 0 || int(len) <= force_width) {
        if (full_path) {
          std::string relative_dir2 =
            relative_dir1 + "/" + list_data->name;

          if (isHtml()) {
            output("<a href='" + relative_dir2 + "'>" + relative_dir2 + "</a>");
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
          }
        }
        else {
          if (isHtml()) {
            output("<a href='" + relative_dir1 + "/" + list_data->name +
                   "'>" + list_data->name + "</a>");
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputColored(list_data->color, list_data->name);
          }
        }
      }
      else {
        if (full_path) {
          auto relative_dir1_len = relative_dir1.size();

          if (int(relative_dir1_len) + 1 <= force_width) {
            std::string relative_dir2 =
              CStrFmt::align(relative_dir1, uint(force_width - 1), CStrFmt::AlignType::RIGHT, ' ',
                             (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

            if (isHtml()) {
              outputHtmlClipped(relative_dir1, relative_dir2);
            }
            else {
              if (isTypeEscape())
                outputTypeEscape(list_data);

              outputClipped(list_data->color, relative_dir2);
            }
          }
          else {
            std::string relative_dir2 = relative_dir1 + "/" +
              CStrFmt::align(list_data->name, uint(force_width - int(relative_dir1_len) - 2),
                             CStrFmt::AlignType::RIGHT, ' ',
                             (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

            if (isHtml()) {
              outputHtmlClipped(relative_dir1 + "/" + list_data->name, relative_dir2);
            }
            else {
              if (isTypeEscape())
                outputTypeEscape(list_data);

              outputClipped(list_data->color, relative_dir2);
            }
          }
        }
        else {
          std::string relative_dir2 =
            CStrFmt::align(list_data->name, uint(force_width - 1), CStrFmt::AlignType::RIGHT, ' ',
                           (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

          if (isHtml()) {
            outputHtmlClipped(relative_dir1 + "/" + list_data->name, relative_dir2);
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputClipped(list_data->color, relative_dir2);
          }
        }
      }

      if (max_len_ > 0 && len < uint(max_len_)) {
        for (uint i = 0; i < uint(max_len_) - len; ++i)
          output(" ");
      }
    }
    else if (m_flag) {
      auto len = list_data->name.size();

      int list_pos1 = list_pos;

      if (list_pos > 0)
        list_pos1 += 2;

      list_pos1 += int(len);

      if (list_pos1 >= list_max_pos) {
        if (isHtml())
          output("<br>");

        outputLine("");

        list_pos  = 0;
        list_pos1 = int(len);
      }

      if (list_pos > 0)
        output(", ");

      if (isHtml()) {
        output("<a href='" + relative_dir1 + "/" + list_data->name + "'>" +
               list_data->name + "</a>");
      }
      else {
        if (isTypeEscape())
          outputTypeEscape(list_data);

        outputColored(list_data->color, list_data->name);
      }

      list_pos = list_pos1;
    }
    else {
      auto len = list_data->name.size();

      if (force_width == 0 || int(len) <= force_width) {
        if (full_path) {
          std::string relative_dir2 = relative_dir1 + "/" + list_data->name;

          if (isHtml()) {
            output("<a href='" + relative_dir2 + "'>" + relative_dir2 + "</a><br>");
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputColored(list_data->color, relative_dir2);
          }
        }
        else {
          if (isHtml()) {
            output("<a href='" + relative_dir1 + "/" + list_data->name +
                   "'>" + list_data->name + "</a><br>");
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputColored(list_data->color, list_data->name);
          }
        }
      }
      else {
        if (full_path) {
          auto relative_dir1_len = relative_dir1.size();

          if (int(relative_dir1_len) + 1 <= force_width) {
            std::string relative_dir2 =
              CStrFmt::align(relative_dir1, uint(force_width - 1), CStrFmt::AlignType::RIGHT, ' ',
                             (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

            if (isHtml()) {
              outputHtmlClipped(relative_dir1 + "/" + list_data->name, relative_dir2);
              output("<br>");
            }
            else {
              if (isTypeEscape())
                outputTypeEscape(list_data);

              outputClipped(list_data->color, relative_dir2);
            }
          }
          else {
            std::string relative_dir2 = relative_dir1 + "/" +
              CStrFmt::align(list_data->name, uint(force_width - int(relative_dir1_len) - 2),
                             CStrFmt::AlignType::RIGHT, ' ',
                             (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

            if (isHtml()) {
              outputHtmlClipped(relative_dir1 + "/" + list_data->name, relative_dir2);
              output("<br>");
            }
            else {
              if (isTypeEscape())
                outputTypeEscape(list_data);

              outputClipped(list_data->color, relative_dir2);
            }
          }
        }
        else {
          std::string relative_dir2 =
            CStrFmt::align(list_data->name, uint(force_width - 1), CStrFmt::AlignType::RIGHT, ' ',
                           (clip_left ? CStrFmt::ClipType::LEFT : CStrFmt::ClipType::RIGHT));

          if (isHtml()) {
            outputHtmlClipped(relative_dir1 + "/" + list_data->name, relative_dir2);
            output("<br>");
          }
          else {
            if (isTypeEscape())
              outputTypeEscape(list_data);

            outputClipped(list_data->color, relative_dir2);
          }
        }
      }

      outputLine("");
    }
  }
}

void
Cls::
setMaxFilelen(FileArray &files)
{
  if (force_width == 0) {
    max_len_ = 0;

    for (const auto &file : files) {
      auto len = file->getLinkName().size();

      if (int(len) > max_len_)
        max_len_ = int(len);
    }
  }
  else
    max_len_ = force_width;

  if (i_flag && ! l_flag)
    max_len_ += 8;

  if (s_flag && ! l_flag)
    max_len_ += 5;

  if (F_flag || p_flag)
    max_len_++;

  max_size_ = 0;

  for (const auto &file : files) {
    auto size = file->getSize();

    max_size_ = std::max(max_size_, size);
  }

  max_size_len_ = uint(std::log10(double(max_size_))) + 1;
}

void
Cls::
setPerm(std::string &str, int perm, int type)
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

  if (type == S_IRUSR && COSFile::stat_mode_is_uid_on_exec(uint(perm))) {
    if (perm & S_IXOTH)
      str[2] = 's';
    else
      str[2] = 'S';
  }

  if (type == S_IRGRP && COSFile::stat_mode_is_gid_on_exec(uint(perm))) {
    if (perm & S_IXOTH)
      str[2] = 's';
    else
      str[2] = 'S';
  }

  if (type == S_IROTH && COSFile::stat_mode_is_restrict_delete(uint(perm))) {
    if (perm & S_IXOTH)
      str[2] = 't';
    else
      str[2] = 'T';
  }
}

std::string
Cls::
typeToString(int type)
{
  char c = char(type);

  return std::string(&c, 1);
}

std::string
Cls::
sizeToString(ClsData *list_data) const
{
  if (list_data->type == 'b' || list_data->type == 'c') {
    static char size_string[64];

    sprintf(size_string, "%d,%d", major(list_data->rdev), minor(list_data->rdev));

    return size_string;
  }

  return sizeToString(list_data->size);
}

std::string
Cls::
sizeToString(size_t size) const
{
  static char size_string[64];

  if      (G_flag)
    sprintf(size_string, "%.1fG", double(size)/1024.0/1024.0/1024.0);
  else if (M_flag)
    sprintf(size_string, "%.1fM", double(size)/1024.0/1024.0);
  else if (k_flag || K_flag)
    sprintf(size_string, "%ldK", size >> 10);
  else {
#if _FILE_OFFSET_BITS == 64
    sprintf(size_string, "%lld", static_cast<long long>(size));
#else
    sprintf(size_string, "%ld", static_cast<long>(size));
#endif
  }

  return size_string;
}

void
Cls::
formatSize(size_t size) const
{
  if      (h_flag) {
    if      (size > 1024*1024*1024)
      CStrUtil::printf(" %4.1fG", double(size)/1024.0/1024.0/1024.0);
    else if (size > 1024*1024)
      CStrUtil::printf(" %4.1fM", double(size)/1024.0/1024.0);
    else if (size > 1024)
      CStrUtil::printf(" %4.1fK", double(size)/1024.0);
    else
      CStrUtil::printf(" %5ld", size);
  }
  else if (G_flag) {
    double sizeG = double(size)/1024.0/1024.0/1024.0;

    if (sizeG > 0.05)
      CStrUtil::printf(" %4.1fG", double(size)/1024.0/1024.0/1024.0);
    else {
      double sizeM = double(size)/1024.0/1024.0;

      if (sizeM > 0.05)
        CStrUtil::printf(" %4.1fM", double(size)/1024.0/1024.0);
      else {
        auto sizeK = size >> 10; // divide 1024

        if (sizeK > 0)
          CStrUtil::printf(" %4ldK", sizeK);
        else
          CStrUtil::printf(" %5ld", size);
      }
    }
  }
  else if (M_flag) {
    double sizeM = double(size)/1024.0/1024.0;

    if (sizeM > 0.05)
      CStrUtil::printf(" %4.1fM", double(size)/1024.0/1024.0);
    else {
      auto sizeK = size >> 10; // divide 1024

      if (sizeK > 0)
        CStrUtil::printf(" %4ldK", sizeK);
      else
        CStrUtil::printf(" %5ld", size);
    }
  }
  else if (k_flag || K_flag) {
    size_t sizeK = size >> 10;

    if (sizeK > 0)
      CStrUtil::printf(" %5ldK", sizeK);
    else
      CStrUtil::printf(" %6ld", size);
  }
  else {
#if _FILE_OFFSET_BITS == 64
    CStrUtil::printf(" %*lld", max_size_len_, static_cast<long long>(size));
#else
    CStrUtil::printf(" %*ld", max_size_len_, static_cast<long>(size));
#endif
  }
}

std::string
Cls::
uidToString(int uid)
{
  if (! n_flag)
    return COSUser::getUserName(uint(uid));
  else
    return CStrUtil::toString(uid);
}

std::string
Cls::
gidToString(int gid)
{
  if (! n_flag)
    return COSUser::getGroupName(uint(gid));
  else
    return CStrUtil::toString(gid);
}

std::string
Cls::
inodeToString(ulong inode)
{
  return CStrUtil::toString(inode);
}

std::string
Cls::
blocksToString(ulong blocks)
{
  return CStrUtil::toString(blocks);
}

std::string
Cls::
timeToString(CTime *time)
{
  if (rel_time)
    return time->getRelTime();
  else
    return time->getLsTime(show_secs);
}

std::string
Cls::
colorToString(ClsColorType color)
{
  if (! isUseColors())
    return "";

  return COSTerm::getColorStr(int(color));
}

int
ClsCompareNames::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp;

  if      (nocase)
    cmp = CStrUtil::casecmp(file1->getName(), file2->getName());
  else if (! locale)
    cmp = CStrUtil::cmp(file1->getName(), file2->getName());
  else
    cmp = strcoll(file1->getName().c_str(), file2->getName().c_str());

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareTimes::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp;

  if (! ctime && ! utime && ! mtime)
    mtime = true;

  if      (ctime) {
    auto *ctime1 = file1->getCTime();
    auto *ctime2 = file2->getCTime();

    if (ctime1 != nullptr && ctime2 != nullptr)
      cmp = static_cast<int>(ctime2->diff(*ctime1));
    else
      cmp = static_cast<int>(ctime1 - ctime2);
  }
  else if (utime) {
    auto *atime1 = file1->getATime();
    auto *atime2 = file2->getATime();

    if (atime1 != nullptr && atime2 != nullptr)
      cmp = static_cast<int>(atime2->diff(*atime1));
    else
      cmp = static_cast<int>(atime1 - atime2);
  }
  else if (mtime) {
    auto *mtime1 = file1->getMTime();
    auto *mtime2 = file2->getMTime();

    if (mtime1 != nullptr && mtime2 != nullptr)
      cmp = static_cast<int>(mtime2->diff(*mtime1));
    else
      cmp = static_cast<int>(mtime1 - mtime2);
  }
  else {
    assert(false);
  }

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareSizes::
operator()(ClsFile *file1, ClsFile *file2)
{
  int cmp = static_cast<int>(file2->getSize() - file1->getSize());

  if (reverse)
    cmp = -cmp;

  return (cmp < 0);
}

int
ClsCompareExtensions::
operator()(ClsFile *file1, ClsFile *file2)
{
  auto pos1 = file1->getName().find('.');
  auto pos2 = file2->getName().find('.');

  if (pos1 != std::string::npos) {
    while (pos1 > 0 && file1->getName()[pos1 - 1] == '.')
      --pos1;

    if (pos1 == 0)
      pos1 = std::string::npos;
  }

  if (pos2 != std::string::npos) {
    while (pos2 > 0 && file2->getName()[pos2 - 1] == '.')
      --pos2;

    if (pos2 == 0)
      pos2 = std::string::npos;
  }

  int cmp;

  if      (pos1 != std::string::npos && pos2 != std::string::npos) {
    cmp = CStrUtil::cmp(file1->getName().substr(pos1 + 1), file2->getName().substr(pos2 + 1));

    if (cmp == 0)
      cmp = CStrUtil::cmp(file1->getName(), file2->getName());
  }
  else if (pos1 != std::string::npos)
    cmp =  1;
  else if (pos2 != std::string::npos)
    cmp = -1;
  else
    cmp = CStrUtil::cmp(file1->getName(), file2->getName());

  return (cmp < 0);
}

void
Cls::
sortFiles(FileArray &files)
{
  if      (sort_type == ClsSortType::NAME)
    std::sort(files.begin(), files.end(), ClsCompareNames(nocase, r_flag, locale));
  else if (sort_type == ClsSortType::TIME)
    std::sort(files.begin(), files.end(), ClsCompareTimes(c_flag, u_flag, m_flag, r_flag));
  else if (sort_type == ClsSortType::SIZE)
    std::sort(files.begin(), files.end(), ClsCompareSizes(r_flag));
  else if (sort_type == ClsSortType::EXTENSION)
    std::sort(files.begin(), files.end(), ClsCompareExtensions());
}

bool
Cls::
outputFiles(const FileArray &files)
{
  using PrefixFiles = std::map<std::string,FileArray>;

  PrefixFiles prefixFiles;
  FileSet     fileSet;

  if (! filterData_->prefixes().empty()) {
    for (const auto &prefix : filterData_->prefixes()) {
      auto prefixLen = prefix.size();

      for (const auto &file : files) {
        if (fileSet.find(file) != fileSet.end())
          continue;

        auto nameLen = file->getLinkName().size();

        if (nameLen >= prefixLen && file->getLinkName().substr(0, prefixLen) == prefix) {
          file->setLinkName(file->getLinkName().substr(prefixLen));

          prefixFiles[prefix].push_back(file);

          fileSet.insert(file);
        }
      }
    }

    for (const auto &file : files) {
      if (fileSet.find(file) != fileSet.end())
        continue;

      prefixFiles[""].push_back(file);
    }

    for (const auto &p : prefixFiles) {
      outputColored(ClsColorType::CLIPPED, p.first + "...\n");

      outputFiles1(p.second);
    }

    return true;
  }
  else {
    return outputFiles1(files);
  }
}

bool
Cls::
outputFiles1(const FileArray &files)
{
  if (isTest()) {
    if (files.empty())
      return true;

    setSilent(true);

    FileSet fileSet;

    (void) testFiles(files, fileSet);

    if (testNames_) {
      if (! fileSet.empty()) {
        bool output = false;

        for (const auto &file : fileSet) {
          if (output)
            std::cout << " ";

          std::cout << file->getName();

          output = true;
        }

        std::cout << "\n";
      }
    }
    else {
      std::cout << fileSet.size() << "\n";
    }

    return true;
  }

  //---

  bool rc = true;

  if       (C_flag) {
    FileArray files1;

    for (const auto &file : files) {
      if (file->getIsOutput())
        files1.push_back(file);
    }

    uint columns = uint((list_max_pos + 1)/(max_len_ + 1));

    if (columns <= 0)
      columns = 1;

    auto num_files1 = files1.size();

    uint rows = uint(num_files1/columns);

    if (num_files1 % columns != 0)
      rows++;

    for (uint i = 0; i < rows; ++i) {
      uint k = i;

      for (uint j = 0; j < columns; ++j) {
        if (k >= num_files1)
          break;

        if (j > 0) {
          if (! isQuiet())
            output(" ");
        }

        if (! listFile(files1[k]))
          rc = false;

        k += rows;
      }

      if (! isQuiet()) {
        if (isHtml())
          output("<br>");

        outputLine("");
      }
    }
  }
  else if (x_flag) {
    int columns;

    if (num_columns == 0) {
      columns = (list_max_pos + 1)/(max_len_ + 1);

      if (columns <= 0)
        columns = 1;
    }
    else
      columns = num_columns;

    int j = 0;

    for (const auto &file : files) {
      if (! file->getIsOutput())
        continue;

      if (j > 0) {
        if (! isQuiet())
          output(" ");
      }

      if (! listFile(file))
        rc = false;

      ++j;

      if (j == columns) {
        if (! isQuiet()) {
          if (isHtml())
            output("<br>");

          outputLine("");
        }

        j = 0;
      }
    }

    if (j > 0) {
      if (! isQuiet()) {
        if (isHtml())
          output("<br>");

        outputLine("");
      }
    }
  }
  else if (T_flag) {
    for (const auto &file : files) {
      if (! file->getIsOutput())
        continue;

      for (int j = 0; j < dir_depth - 1; ++j)
        output(" ");

      output("|- ");

      if (! listFile(file))
        rc = false;
    }
  }
  else {
    for (const auto &file : files) {
      if (! file->getIsOutput())
        continue;

      if (! listFile(file))
        rc = false;
    }
  }

  if (m_flag && list_pos > 0) {
    if (! isQuiet()) {
      if (isHtml())
        output("<br>");

      outputLine("");
    }

    list_pos = 0;
  }

  return rc;
}

std::string
Cls::
encodeName(const std::string &name)
{
  auto len = name.size();

  std::string name1;

  uint i = 0;

  if (no_path) {
    uint i1 = 0;

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

        name1 += char(digit1 + '0');
        name1 += char(digit2 + '0');
        name1 += char(digit3 + '0');
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
decodeTypeChar(const std::string &opt, int c)
{
  ClsFileType type = ClsFileType::NONE;

  switch (c) {
    case 'p': type = ClsFileType::FIFO;    break;
    case 'c': type = ClsFileType::CHR;     break;
    case 'd': type = ClsFileType::DIR;     break;
    case 'b': type = ClsFileType::BLK;     break;
    case 'f': type = ClsFileType::REG;     break;
    case 'l': type = ClsFileType::LNK;     break;
    case 's': type = ClsFileType::SOCK;    break;
    case 'x': type = ClsFileType::EXEC;    break;
    case 'X': type = ClsFileType::ELF;     break;
    case 'B': type = ClsFileType::BAD;     break;
    case 'S': type = ClsFileType::SPECIAL; break;
    default: {
      std::cerr << "Invalid --" << opt << " type " << char(c) << "\n";
      break;
    }
  }

  return type;
}

bool
Cls::
enterDir(const std::string &dir)
{
  if (! CDir::enter(dir)) {
    if (! isSilent())
      std::cerr << CDir::getErrorMsg() << "\n";
    return false;
  }

  ++dir_depth;

  current_dir = CDir::getCurrent();

  auto base_dir_len    = base_dir.size();
  auto current_dir_len = current_dir.size();

  if (current_dir_len >= base_dir_len &&
      current_dir.substr(0, base_dir_len) == base_dir) {
    auto offset = base_dir_len;

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
leaveDir()
{
  if (! CDir::leave()) {
    if (! isSilent())
      std::cerr << CDir::getErrorMsg() << "\n";
    return;
  }

  --dir_depth;

  current_dir = CDir::getCurrent();
}

std::string
Cls::
execToString(const std::string &command)
{
  if (isPreview())
    return command;

  //---

  static std::string exec_string;

  if (! COSProcess::executeCommand(command, exec_string, nullptr))
    return "";

  auto len = exec_string.size();

  for (size_t i = 0; i < len; ++i) {
    if (exec_string[i] == '\n')
      exec_string[i] = ' ';
  }

  // skip to first non-space
  size_t i = 0;

  while (i < len && isspace(exec_string[i]))
    ++i;

  // skip trailing space
  while (len > 0 && isspace(exec_string[len - 1]))
    --len;

  exec_string = exec_string.substr(i, len - i);

  return exec_string;
}

bool
Cls::
specialGlobMatch(const std::string &file, ClsColorType *color)
{
  static uint NUM_COLORS = 7;

  for (uint i = 0; i < NUM_COLORS; ++i) {
    *color = static_cast<ClsColorType>(i);

    for (const auto &special_glob : special_globs[i]) {
      if (special_glob->compare(file))
        return true;
    }
  }

  return false;
}

int
Cls::
execFile(ClsFile *file, const std::string &exec_cmd)
{
  std::string exec_cmd1;

  auto exec_cmd_len = exec_cmd.size();

  // replace format characters with file data
  size_t pos1 = 0;
  auto   pos2 = exec_cmd.find('%');

  while (pos2 != std::string::npos) {
    exec_cmd1 += exec_cmd.substr(pos1, pos2 - pos1); // before %

    ++pos2;

    std::string dstr;

    while (pos2 < exec_cmd_len && isdigit(exec_cmd[pos2]))
      dstr += exec_cmd[pos2++];

    int format_width = (dstr.size() ? std::stoi(dstr) : -1);

    char code = exec_cmd[pos2];

    auto addStr = [&](const std::string &str) {
      auto str1 = str;

      if (format_width > 0) {
        while (int(str1.size()) < format_width)
          str1 = " " + str1;
      }

      exec_cmd1 += str1;
    };

    switch (code) {
      // add directory
      case 'd': {
        addStr(current_dir);
        break;
      }
      // add filename
      case 'f': {
        addStr(file->getName());
        break;
      }
      // add group name
      case 'g': {
        addStr(gidToString(file->getGid()));
        break;
      }
      // add link name
      case 'l': {
        addStr(file->getLinkName());
        break;
      }
      // add full path
      case 'p': {
        addStr(current_dir + "/" + file->getName());
        break;
      }
      // add quoted full path
      case 'q': {
        addStr("\"" + current_dir + "/" + file->getName() + "\"");
        break;
      }
      // permissions
      case 'P': {
        std::string str, str1;;
        setPerm(str, file->getUPerm(), S_IRUSR); str1 += str;
        setPerm(str, file->getGPerm(), S_IRGRP); str1 += str;
        setPerm(str, file->getOPerm(), S_IROTH); str1 += str;
        addStr(str1);
        break;
      }
      // add link name
      case 's': {
        addStr(sizeToString(file->getSize()));
        break;
      }
      case 't': {
        char code1 = '\0';

        if (pos2 < exec_cmd_len)
          code1 = exec_cmd[++pos2];

        if      (code1 == 'a')
          addStr(timeToString(file->getATime()));
        else if (code1 == 'c')
          addStr(timeToString(file->getCTime()));
        else if (code1 == 'm')
          addStr(timeToString(file->getMTime()));
        else {
          --pos2;
          addStr(timeToString(file->getMTime()));
        }

        break;
      }
      // add type
      case 'T': {
        auto c = typeChar(file);
        addStr(std::string(&c, 1));
        break;
      }
      // add user name
      case 'u': {
        addStr(uidToString(file->getUid()));
        break;
      }
      // add % char
      case '%': {
        exec_cmd1 += dstr + "%";
        break;
      }
      // handle bad format
      default: {
        std::cerr << "Bad exec code %" + std::string(&code, 1) + "\n";
        break;
      }
    }

    if (pos2 < exec_cmd_len)
      ++pos2;

    pos1 = pos2;
    pos2 = exec_cmd.find('%', pos1);
  }

  exec_cmd1 += exec_cmd.substr(pos1);

  ///---

  auto len1 = exec_cmd1.size();

  // echo shortcut
  if      (len1 > 5 && exec_cmd1.substr(0, 5) == "echo ") {
    // skip echo and space after
    size_t i = 5;

    while (i < len1 && isspace(exec_cmd1[i]))
      ++i;

    std::string args = exec_cmd1.substr(i);

    if (isPreview())
      std::cout << "echo " << args << "\n";
    else
      std::cout << args << "\n";

    return true;
  }
  // rm shortcut
  else if (len1 > 3 && exec_cmd1.substr(0, 3) == "rm ") {
    // skip rm and space after
    size_t i = 3;

    while (i < len1 && isspace(exec_cmd1[i]))
      ++i;

    std::string args = exec_cmd1.substr(i);

    if (isPreview()) {
      std::cout << "rm " << args << "\n";
    }
    else {
      int rc = unlink(args.c_str());

      if (rc != 0) {
        std::cerr << "rm failed for " << args << "\n";
        return false;
      }
    }

    return true;
  }
#if 0
  // touch shortcut
  else if (len1 > 5 && exec_cmd1.substr(0, 5) == "touch ") {
    // skip rm and space after
    size_t i = 5;

    while (i < len1 && isspace(exec_cmd1[i]))
      ++i;

    std::string args = exec_cmd1.substr(i);

    if (isPreview()) {
      std::cout << "touch " << args << "\n";
    }
    else {
      int rc = utimensat(0, args.c_str(), tspec); // TODO

      if (rc != 0) {
        std::cerr << "touch failed for " << args << "\n";
        return false;
      }
    }

    return true;
  }
#endif
  // run generic command
  else {
    bool silent = false;

    if (exec_cmd1[0] == '#') {
      exec_cmd1 = exec_cmd1.substr();
      silent    = true;
    }

    std::string res;
    int         status;

    if (! runCommand(exec_cmd1, res, status))
      return 1;

    if (! silent)
      std::cout << res;

    return status;
  }
}

bool
Cls::
runCommand(const std::string &cmd, std::string &result, int &status)
{
  if (isPreview()) {
    std::cout << cmd << "\n";
    return true;
  }

  status = 0;

  COSProcess::CommandState state;

  if (! COSProcess::executeCommand(cmd, result, &state)) {
    if (! isSilent())
      std::cerr << "exec failed for " << cmd << "\n";
    return false;
  }

  status = state.getStatus();

  return true;
}

void
Cls::
outputTypeEscape(ClsData *list_data)
{
  outputTypeEscape(list_data, list_data->name);
}

void
Cls::
outputTypeEscape(ClsData *list_data, const std::string &str)
{
  CFileType type = getDataType(list_data->file);

  outputTypeEscape(type, str);
}

void
Cls::
outputTypeEscape(CFileType type, const std::string &str)
{
  std::string type_str = CFileUtil::getTypeStr(type);

  if (type_str != "??") {
#ifdef CTERM_SUPPORT
    output(CEscape::APC("<link type=\"" + type_str + "\"" + " dir=\"" + current_dir + "\"" +
                        " name=\"" + str + "\"/>"));
#else
    assert(str.length());
#endif
  }
}

CFileType
Cls::
getDataType(ClsFile *file)
{
  if      (file->isFIFO   ()) return CFILE_TYPE_INODE_FIFO;
  else if (file->isChar   ()) return CFILE_TYPE_INODE_CHR;
  else if (file->isDir    ()) return CFILE_TYPE_INODE_DIR;
  else if (file->isBlock  ()) return CFILE_TYPE_INODE_BLK;
  else if (file->isLink   ()) return CFILE_TYPE_INODE_LNK;
  else if (file->isSocket ()) return CFILE_TYPE_INODE_SOCK;

  CFile cfile(file->getName());

  CFileType type = CFileUtil::getType(&cfile);

  return type;
}

std::string
Cls::
getDataTypeStr(ClsFile *file)
{
  CFileType type = getDataType(file);

  std::string type_str = CFileUtil::getTypeStr(type);

  return type_str + ']';
}

void
Cls::
outputHtmlClipped(const std::string &str1, const std::string &str2)
{
  if (clip_left)
    output("<a href='" + str1 + "'>" + str2 + "&gt;" + "</a>");
  else
    output("<a href='" + str1 + "'>" + "&lt;" + str2 + "</a>");
}

void
Cls::
outputClipped(ClsColorType color, const std::string &str)
{
  if (clip_left) {
    outputColored(ClsColorType::CLIPPED, "<");
    outputColored(color, str);
  }
  else {
    outputColored(color, str);
    outputColored(ClsColorType::CLIPPED, ">");
  }
}

void
Cls::
outputColored(ClsColorType color, const std::string &str)
{
  if (isUseColors())
    output(colorToString(color) + str + colorToString(ClsColorType::NORMAL));
  else
    output(str);
}

void
Cls::
outputLine(const std::string &str)
{
  if (! isQuiet())
    std::cout << str << "\n";
}

void
Cls::
output(const std::string &str)
{
  if (! isQuiet())
    std::cout << str;
}
