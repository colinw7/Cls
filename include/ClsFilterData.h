#ifndef ClsFilterData_H
#define ClsFilterData_H

#include <string>
#include <vector>

class Cls;
class ClsFile;
class CGlob;

class ClsFilterData {
 public:
  using Prefixes = std::vector<std::string>;

 public:
  ClsFilterData();

  void addIncludeType(uint type) {
    includeFlags_ |= type;

    filtered_ = true;
  }

  void addIncludeFileType(const std::string &fileType);

  void addExcludeType(uint type) {
    excludeFlags_ |= type;

    filtered_ = true;
  }

  void addExcludeFileType(const std::string &fileType);

  void addPrefix(const std::string &prefix);

  void addMatch  (const std::string &pattern);
  void addNoMatch(const std::string &pattern);

  void addLinkMatch  (const std::string &pattern);
  void addNoLinkMatch(const std::string &pattern);

  void setShowZero(bool show) {
    show_zero_ = show;

    if (! show_zero_)
      show_non_zero_ = true;

    filtered_ = true;
  }

  void setShowNonZero(bool show) {
    show_non_zero_ = show;

    if (! show_non_zero_)
      show_zero_ = true;

    filtered_ = true;
  }

  void setOnlyUser(bool only_user) {
    only_user_ = only_user;

    filtered_ = true;
  }

  void setNewer  (int  newer  ) { newer_   = newer  ; filtered_ = true; }
  void setOlder  (int  older  ) { older_   = older  ; filtered_ = true; }
  void setLarger (long larger ) { larger_  = larger ; filtered_ = true; }
  void setSmaller(long smaller) { smaller_ = smaller; filtered_ = true; }

  void setExec(const std::string &exec) { exec_ = exec; filtered_ = true; }

  bool checkFile(Cls *cls, ClsFile *file) const;

  const Prefixes &prefixes() const { return prefixes_; }

 private:
  bool checkIncludeFile(Cls *cls, ClsFile *file) const;
  bool checkExcludeFile(Cls *cls, ClsFile *file) const;

 private:
  using PatternList = std::vector<CGlob *>;

  bool filtered_ { false };

  uint        includeFlags_ { 0 };
  PatternList includeFileTypes_;
  uint        excludeFlags_ { 0 };
  PatternList excludeFileTypes_;
  Prefixes    prefixes_;
  PatternList match_patterns_;
  PatternList nomatch_patterns_;
  PatternList link_match_patterns_;
  PatternList link_nomatch_patterns_;

  bool show_zero_     { true };
  bool show_non_zero_ { true };

  bool only_user_ { false };

  int  newer_   { -1 };
  int  older_   { -1 };
  long larger_  { -1 };
  long smaller_ { -1 };

  std::string exec_;
};

#endif
