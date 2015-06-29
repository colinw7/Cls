#ifndef ClsFile_H
#define ClsFile_H

#include <string>
#include <sys/types.h>

class CTime;

class ClsFile {
 public:
  ClsFile(bool use_link, const std::string &name, const std::string &lname = "") :
   initialized_(false), use_link_(use_link), name_(name), lname_(lname),
   stat_(NULL), f_stat_(NULL), l_stat_(NULL), output_(true) {
  }

  const std::string &getName() const { return name_; }
  void setName(const std::string &name) { name_ = name; }

  const std::string &getLinkName() const { return lname_; }
  void setLinkName(const std::string &lname) { lname_ = lname; }

  bool exists();
  bool isDir();
  bool isFIFO();
  bool isChar();
  bool isBlock();
  bool isRegular();
  bool isLink();
  bool isSocket();

  bool isUserExecutable();

  bool isElf();

  size_t getSize();

  uint getUid();
  uint getGid();

  int getUPerm();
  int getGPerm();
  int getOPerm();

  CTime *getCTime();
  CTime *getMTime();
  CTime *getATime();

  ulong getINode();
  uint  getNLink();
  uint  getDev();
  uint  getRDev();
  ulong getNumBlocks();

  void setIsOutput(bool flag) { output_ = flag; }
  bool getIsOutput() const { return output_; }

  bool hasLinkStat();

 private:
  bool init();
  bool get_fstat();
  bool get_lstat();

 private:
  bool         initialized_;
  bool         use_link_;
  std::string  name_;
  std::string  lname_;
  struct stat *stat_;
  struct stat *f_stat_;
  struct stat *l_stat_;
  CTime       *ctime_;
  CTime       *mtime_;
  CTime       *atime_;
  bool         output_;
};

#endif
