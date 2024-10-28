#ifndef ClsFile_H
#define ClsFile_H

#include <string>
#include <sys/types.h>

class Cls;
class CTime;

class ClsFile {
 public:
  ClsFile(Cls *ls, bool useLink, const std::string &name, const std::string &lname="");

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

  const std::string &getFullLinkName();

 private:
  bool init();
  bool getFStat();
  bool getLStat();

 private:
  Cls*         ls_          { nullptr };
  bool         initialized_ { false };
  bool         useLink_     { false };
  bool         linkRead_    { false };
  std::string  name_;
  std::string  lname_;
  std::string  fullLinkName_;
  struct stat *stat_        { nullptr };
  struct stat *fstat_       { nullptr };
  struct stat *lstat_       { nullptr };
  CTime       *ctime_       { nullptr };
  CTime       *mtime_       { nullptr };
  CTime       *atime_       { nullptr };
  bool         output_      { true };
};

#endif
