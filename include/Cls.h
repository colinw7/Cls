#ifndef CLS_H
#define CLS_H

class ClsFilterData;
class CTime;
class CGlob;
class Cls;

#include <CFile.h>
#include <set>

enum class ClsFileType {
  NONE    = 0,
  FIFO    = (1<<0),
  CHR     = (1<<1),
  DIR     = (1<<2),
  BLK     = (1<<3),
  REG     = (1<<4),
  LNK     = (1<<5),
  SOCK    = (1<<6),
  EXEC    = (1<<7),
  BAD     = (1<<8),
  SPECIAL = (1<<9),
  ELF     = (1<<10),
  EXISTS  = (1<<11)
};

enum class ClsFilterType {
  IN      = 0,
  OUT     = 1,
  OUT_DIR = 2
};

enum class ClsSortType {
  NONE      = 0,
  NAME      = 1,
  TIME      = 2,
  SIZE      = 3,
  EXTENSION = 4
};

enum class ClsColorType {
  NORMAL     = 0,
  DIRECTORY  = 3,
  EXECUTABLE = 1,
  LINK       = 6,
  DEVICE     = 6,
  PIPE       = 5,
  SPECIAL    = 2,
  BAD_FILE   = 4,
  CLIPPED    = 3
};

//------

#include <ClsFile.h>

//------

struct ClsData {
  ClsFile      *file { nullptr };
  int           type { 0 };
  std::string   u_perm;
  std::string   g_perm;
  std::string   o_perm;
  size_t        size { 0 };
  uint          uid { 0 };
  uint          gid { 0 };
  ulong         ino { 0 };
  int           nlink { 0 };
  int           dev { 0 };
  int           rdev { 0 };
  ulong         blocks { 0 };
  CTime        *ctime { nullptr };
  CTime        *mtime { nullptr };
  CTime        *atime { nullptr };
  std::string   name;
  ClsColorType  color;
  std::string   link_name;
  ClsColorType  link_color;
};

//------

class Cls {
 public:
  typedef std::vector<ClsFile *> FileArray;
  typedef std::set<ClsFile *>    FileSet;

 public:
  Cls();
 ~Cls();

  CTime *getCurrentTime() const { return current_time_; }

  uint getUserUid() const { return user_uid_; }

  bool isTest() const { return testFlags_ != 0; }
  void setTestFlags(const std::string &str="", bool names=false);

  bool isQuiet() const { return quiet_; }
  void setQuiet(bool b) { quiet_ = b; }

  bool isHtml() const { return html_; }
  void setHtml(bool b) { html_ = b; }

  bool isTypeEscape() const { return typeEscape_; }
  void setTypeEscape(bool b) { typeEscape_ = b; }

  bool isDataType() const { return dataType_; }
  void setDataType(bool b) { dataType_ = b; }

  bool isBadNames() const { return badNames_; }
  void setBadNames(bool b) { badNames_ = b; }

  bool isRenameBad() const { return renameBad_; }
  void setRenameBad(bool b) { renameBad_ = b; }

  bool isPreview() const { return preview_; }
  void setPreview(bool b) { preview_ = b; }

  bool isSilent() const { return silent_; }
  void setSilent(bool b) { silent_ = b; }

  const std::string &lsFormat() const { return lsFormat_; }
  void setLsFormat(const std::string &v) { lsFormat_ = v; }

  bool isUseColors() const { return useColors_; }
  void setUseColors(bool b) { useColors_ = b; }

  void          init();
  void          processArgs(int argc, char **argv);
  bool          exec();
  bool          testFiles(const FileArray &files, FileSet &fileSet);
  bool          listDirEntry(ClsFile *file);
  bool          listDir(ClsFile *file);
  bool          listCurrentDir(ClsFile *file);
  bool          getDirFiles(ClsFile *file, FileArray &files);
  void          addDirFiles(const std::string &name, FileArray &files);
  ClsFilterType filterFile(ClsFile *file);
  bool          listDirFiles(FileArray &files);
  void          recurseFiles(FileArray &files);
  void          freeDirFiles(FileArray &files);
  bool          listFile(ClsFile *file);
  bool          isBadFile(ClsFile *file);
  bool          isBadLink(ClsFile *file);
  void          printListData(ClsData *file);
  void          setMaxFilelen(FileArray &files);
  void          setPerm(std::string &str, int, int);
  std::string   typeToString(int);
  std::string   sizeToString(ClsData *);
  std::string   uidToString(int uid);
  std::string   gidToString(int gid);
  std::string   inodeToString(int);
  std::string   blocksToString(int);
  std::string   timeToString(CTime *time);
  std::string   colorToString(ClsColorType color);
  void          splitFiles(const FileArray &files, FileArray &dfiles, FileArray &rfiles);
  void          sortFiles(FileArray &files);
  bool          outputFiles(FileArray &files);
  std::string   encodeName(const std::string &name);
  void          getScreenSize();
  int           getTermCols();
  ClsFileType   decodeTypeChar(const std::string &opt, int c);
  bool          enterDir(const std::string &dir);
  void          leaveDir();
  std::string   execToString(const std::string &command);
  bool          specialGlobMatch(const std::string &file, ClsColorType *color);
  int           execFile(ClsFile *file, const std::string &command);
  bool          runCommand(const std::string &cmd, int &status);
  void          outputTypeEscape(ClsData *list_data);
  void          outputTypeEscape(ClsData *list_data, const std::string &str);
  void          outputTypeEscape(CFileType type, const std::string &str);
  CFileType     getDataType(ClsFile *file);
  std::string   getDataTypeStr(ClsFile *file);
  void          outputColored(ClsColorType color, const std::string &str);
  void          outputLine(const std::string &str);
  void          output(const std::string &str);

 private:
  typedef std::vector<std::string> DirStack;
  typedef std::vector<CGlob *>     GlobArray;

  bool               a_flag { false };
  bool               b_flag { false };
  bool               c_flag { false };
  bool               d_flag { false };
  bool               f_flag { false };
  bool               g_flag { false };
  bool               i_flag { false };
  bool               k_flag { false };
  bool               l_flag { false };
  bool               m_flag { false };
  bool               n_flag { false };
  bool               o_flag { false };
  bool               p_flag { false };
  bool               q_flag { false };
  bool               r_flag { false };
  bool               s_flag { false };
  bool               t_flag { false };
  bool               u_flag { false };
  bool               x_flag { false };
  bool               A_flag { false };
  bool               C_flag { false };
  bool               F_flag { false };
  bool               L_flag { false };
  bool               M_flag { false };
  bool               R_flag { false };
  bool               T_flag { false };

  bool               nlink_flag { false };
  uint               user_uid_ { 0 };
  uint               user_gid_ { 0 };
  bool               show_links { false };
  bool               show_bad { false };
  bool               show_bad_user { false };
  bool               show_bad_other { false };
  bool               show_empty_dirs { false };
  ClsSortType        sort_type { ClsSortType::NAME };
  int                force_width { 0 };
  ClsFilterData*     filterData_;
  mode_t             my_umask { 0777 };
  bool               full_path { false };
  bool               no_path { false };
  bool               show_secs { false };
  bool               nocase { false };
  bool               locale { true };
  std::string        exec_init_cmd_;
  std::string        exec_term_cmd_;
  std::string        exec_cmd_;

  uint               testFlags_  { 0 };
  bool               testNames_  { false };
  bool               quiet_      { false };
  bool               html_       { false };
  bool               typeEscape_ { false };
  bool               dataType_   { false };
  bool               badNames_   { false };
  bool               renameBad_  { false };
  bool               preview_    { false };
  bool               silent_     { false };

  int                num_columns { 0 };

  std::string        lsFormat_;

  int                max_len { -1 };
  size_t             max_size { 0 };
  uint               max_size_len { 1 };

  int                list_pos { 0 };
  int                list_max_pos { 99 };

  int                screen_rows { 60 };
  int                screen_cols { 80 };

  std::string        base_dir;

  DirStack           previous_dir_stack;

  std::string        current_dir;
  std::string        relative_dir;
  std::string        relative_dir1;
  int                dir_depth { 0 };

  CTime             *current_time_ { nullptr };

  GlobArray          special_globs[7];

  FileArray          files_;
  bool               readFiles_ { false };

  bool               useColors_ { true };
};

#endif
