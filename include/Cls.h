#ifndef CLS_H
#define CLS_H

class CTime;
class CGlob;
class Cls;

#include <string>
#include <vector>

#include <CFileType.h>

enum ClsFileType {
  FILE_TYPE_NONE    = 0,
  FILE_TYPE_FIFO    = (1<<0),
  FILE_TYPE_CHR     = (1<<1),
  FILE_TYPE_DIR     = (1<<2),
  FILE_TYPE_BLK     = (1<<3),
  FILE_TYPE_REG     = (1<<4),
  FILE_TYPE_LNK     = (1<<5),
  FILE_TYPE_SOCK    = (1<<6),
  FILE_TYPE_EXEC    = (1<<7),
  FILE_TYPE_BAD     = (1<<8),
  FILE_TYPE_SPECIAL = (1<<9),
  FILE_TYPE_ELF     = (1<<10)
};

enum ClsFilterType {
  FILTER_OUT = 0,
  FILTER_IN  = 1,
  FILTER_DIR = 2
};

enum ClsSortType {
  SORT_NONE      = 0,
  SORT_NAME      = 1,
  SORT_TIME      = 2,
  SORT_SIZE      = 3,
  SORT_EXTENSION = 4
};

enum ClsColorType {
  COLOR_NORMAL     = 0,
  COLOR_DIRECTORY  = 3,
  COLOR_EXECUTABLE = 1,
  COLOR_LINK       = 6,
  COLOR_DEVICE     = 6,
  COLOR_PIPE       = 5,
  COLOR_SPECIAL    = 2,
  COLOR_BAD_FILE   = 4,
  COLOR_CLIPPED    = 3
};

#include <ClsFile.h>

struct ClsData {
  ClsFile      *file;
  int           type;
  std::string   u_perm;
  std::string   g_perm;
  std::string   o_perm;
  size_t        size;
  uint          uid;
  uint          gid;
  ulong         ino;
  int           nlink;
  int           dev;
  int           rdev;
  ulong         blocks;
  CTime        *ctime;
  CTime        *mtime;
  CTime        *atime;
  std::string   name;
  ClsColorType  color;
  std::string   link_name;
  ClsColorType  link_color;
};

class ClsFilterData {
 private:
  typedef std::vector<CGlob *> PatternList;

  bool filtered_;

  uint        includeFlags_;
  PatternList includeFileTypes_;
  uint        excludeFlags_;
  PatternList excludeFileTypes_;
  PatternList match_patterns_;
  PatternList nomatch_patterns_;

  bool show_zero_;
  bool show_non_zero_;

  bool only_user_;

  int newer_;
  int older_;
  int larger_;
  int smaller_;

  std::string exec_;

 public:
  ClsFilterData();

  void addIncludeType(int type) {
    includeFlags_ |= type;

    filtered_ = true;
  }

  //void addIncludeFileType(const std::string &fileType);

  void addExcludeType(int type) {
    excludeFlags_ |= type;

    filtered_ = true;
  }

  //void addExcludeFileType(const std::string &fileType);

  void addMatch(const std::string &pattern);
  void addNoMatch(const std::string &pattern);

  void setShowZero(bool show) {
    show_zero_ = show;

    if (! show_zero_) show_non_zero_ = true;

    filtered_ = true;
  }

  void setShowNonZero(bool show) {
    show_non_zero_ = show;

    if (! show_non_zero_) show_zero_ = true;

    filtered_ = true;
  }

  void setOnlyUser(bool only_user) {
    only_user_ = only_user;

    filtered_ = true;
  }

  void setNewer  (int newer  ) { newer_   = newer  ; filtered_ = true; }
  void setOlder  (int older  ) { older_   = older  ; filtered_ = true; }
  void setLarger (int larger ) { larger_  = larger ; filtered_ = true; }
  void setSmaller(int smaller) { smaller_ = smaller; filtered_ = true; }

  void setExec(const std::string &exec) { exec_ = exec; filtered_ = true; }

  bool checkFile(Cls *cls, ClsFile *file) const;

 private:
  bool checkIncludeFile(Cls *cls, ClsFile *file) const;
  bool checkExcludeFile(Cls *cls, ClsFile *file) const;
};

class Cls {
 private:
  typedef std::vector<std::string> DirStack;
  typedef std::vector<CGlob *>     GlobArray;
  typedef std::vector<ClsFile *>   FileArray;

  bool               a_flag;
  bool               b_flag;
  bool               c_flag;
  bool               d_flag;
  bool               f_flag;
  bool               g_flag;
  bool               i_flag;
  bool               k_flag;
  bool               l_flag;
  bool               m_flag;
  bool               n_flag;
  bool               o_flag;
  bool               p_flag;
  bool               q_flag;
  bool               r_flag;
  bool               s_flag;
  bool               t_flag;
  bool               u_flag;
  bool               x_flag;
  bool               A_flag;
  bool               C_flag;
  bool               F_flag;
  bool               L_flag;
  bool               M_flag;
  bool               R_flag;
  bool               T_flag;

  bool               nlink_flag;
  uint               user_uid_;
  uint               user_gid_;
  bool               show_links;
  bool               show_bad;
  bool               show_bad_user;
  bool               show_bad_other;
  bool               show_empty_dirs;
  ClsSortType        sort_type;
  int                force_width;
  ClsFilterData      filter_data_;
  mode_t             my_umask;
  bool               full_path;
  bool               no_path;
  bool               show_secs;
  bool               nocase;
  std::string        exec_init_cmd_;
  std::string        exec_term_cmd_;
  std::string        exec_cmd_;
  bool               quiet;
  bool               html;
//bool               type_escape;
  bool               datatype;
  bool               bad_names;

  int                num_columns;

  std::string        ls_format;

  int                max_len;
  size_t             max_size;
  uint               max_size_len;

  int                list_pos;
  int                list_max_pos;

  int                screen_rows;
  int                screen_cols;

  std::string        base_dir;

  DirStack           previous_dir_stack;
  std::string        current_dir;
  std::string        relative_dir;
  std::string        relative_dir1;
  int                dir_depth;

  CTime             *current_time_;

  GlobArray          special_globs[7];

  FileArray          files;
  bool               read_files;

  bool               use_colors;

 public:
  Cls();
 ~Cls();

  CTime *getCurrentTime() const { return current_time_; }

  uint getUserUid() const { return user_uid_; }

  void           init();
  void           process_args(int argc, char **argv);
  void           exec();
  void           list_dir_entry(ClsFile *file);
  void           list_dir(ClsFile *file);
  void           get_dir_files(ClsFile *file, std::vector<ClsFile *> &files);
  void           add_dir_files(const std::string &name, std::vector<ClsFile *> &files);
  ClsFilterType  filter_file(ClsFile *file);
  void           list_dir_files(std::vector<ClsFile *> &files);
  void           recurse_files(std::vector<ClsFile *> &files);
  void           free_dir_files(std::vector<ClsFile *> &files);
  void           list_file(ClsFile *file);
  bool           is_bad_filename(ClsFile *file);
  bool           is_bad_file(ClsFile *file);
  void           print_list_data(ClsData *file);
  void           set_max_filelen(std::vector<ClsFile *> &files);
  void           set_perm(std::string &str, int, int);
  std::string    type_to_string(int);
  std::string    size_to_string(ClsData *);
  std::string    uid_to_string(int uid);
  std::string    gid_to_string(int gid);
  std::string    inode_to_string(int);
  std::string    blocks_to_string(int);
  std::string    time_to_string(CTime *time);
  std::string    color_to_string(ClsColorType color);
  void           split_files(const std::vector<ClsFile *> &files, std::vector<ClsFile *> &dfiles,
                             std::vector<ClsFile *> &rfiles);
  void           sort_files(std::vector<ClsFile *> &files);
  void           output_files(std::vector<ClsFile *> &files);
  std::string    encode_name(const std::string &name);
  void           get_screen_size();
  int            get_term_cols();
  ClsFileType    decode_type_char(const std::string &opt, int c);
  bool           enter_dir(const std::string &dir);
  void           leave_dir();
  std::string    exec_to_string(const std::string &command);
  bool           special_glob_match(const std::string &file, ClsColorType *color);
  int            exec_file(ClsFile *file, const std::string &command);
//void           outputTypeEscape(ClsData *list_data);
//void           outputTypeEscape(ClsData *list_data, const std::string &str);
//void           outputTypeEscape(CFileType type, const std::string &str);
//CFileType      get_data_type(ClsFile *file);
//std::string    get_data_type_str(ClsFile *file);
  void           outputColored(ClsColorType color, const std::string &str);
  void           outputLine(const std::string &str);
  void           output(const std::string &str);
};

#endif
