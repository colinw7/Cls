#include <ClsFile.h>
#include <Cls.h>

#include <CTime.h>
#include <COSFile.h>
#include <CFile.h>
#include <CFileUtil.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <sys/param.h>

ClsFile::
ClsFile(Cls *ls, bool useLink, const std::string &name, const std::string &lname) :
 ls_(ls), useLink_(useLink), name_(name), lname_(lname)
{
}

bool
ClsFile::
exists()
{
  if (! init())
    return false;

  return true;
}

bool
ClsFile::
isDir()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_dir(stat_))
    return false;

  return true;
}

bool
ClsFile::
isFIFO()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_fifo(stat_))
    return false;

  return true;
}

bool
ClsFile::
isChar()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_char(stat_))
    return false;

  return true;
}

bool
ClsFile::
isBlock()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_block(stat_))
    return false;

  return true;
}

bool
ClsFile::
isRegular()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_reg(stat_))
    return false;

  return true;
}

bool
ClsFile::
isLink()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_link(stat_))
    return false;

  return true;
}

bool
ClsFile::
isSocket()
{
  if (! init())
    return false;

  if (! COSFile::stat_is_socket(stat_))
    return false;

  return true;
}

bool
ClsFile::
isUserExecutable()
{
  if (! init())
    return false;

  return (stat_->st_mode & S_IXUSR);
}

bool
ClsFile::
isElf()
{
  if (! init())
    return false;

  CFile file(name_);

  return (! isLink() && CFileUtil::isELF(&file) != CFILE_TYPE_NONE);
}

size_t
ClsFile::
getSize()
{
  if (! init())
    return size_t(-1);

  return size_t(stat_->st_size);
}

uint
ClsFile::
getUid()
{
  if (! init())
    return 0;

  return stat_->st_uid;
}

uint
ClsFile::
getGid()
{
  if (! init())
    return 0;

  return stat_->st_gid;
}

int
ClsFile::
getUPerm()
{
  if (! init())
    return 0;

  return ((stat_->st_mode & S_IRWXU) >> 6);
}

int
ClsFile::
getGPerm()
{
  if (! init())
    return 0;

  return ((stat_->st_mode & S_IRWXG) >> 3);
}

int
ClsFile::
getOPerm()
{
  if (! init())
    return 0;

  return ((stat_->st_mode & S_IRWXO) >> 0);
}

CTime *
ClsFile::
getCTime()
{
  if (! init())
    return nullptr;

  return ctime_;
}

CTime *
ClsFile::
getMTime()
{
  if (! init())
    return nullptr;

  return mtime_;
}

CTime *
ClsFile::
getATime()
{
  if (! init())
    return nullptr;

  return atime_;
}

ulong
ClsFile::
getINode()
{
  if (! init())
    return 0;

  return stat_->st_ino;
}

uint
ClsFile::
getNLink()
{
  if (! init())
    return 0;

  return uint(stat_->st_nlink);
}

uint
ClsFile::
getDev()
{
  if (! init())
    return 0;

  return uint(stat_->st_dev);
}

uint
ClsFile::
getRDev()
{
  if (! init())
    return 0;

  return uint(stat_->st_rdev);
}

ulong
ClsFile::
getNumBlocks()
{
  if (! init())
    return 0;

  return ulong(stat_->st_blocks);
}

bool
ClsFile::
hasLinkStat()
{
  if (! init())
    return false;

  return (lstat_ && fstat_);
}

bool
ClsFile::
init()
{
  if (initialized_) {
    if (! fstat_ && ! lstat_)
      return false;

    return true;
  }

  initialized_ = true;

  getFStat();
  getLStat();

  if (! fstat_ && ! lstat_)
    return false;

  if      (! fstat_) {
    stat_ = lstat_;
  }
  else if (! lstat_) {
    stat_ = fstat_;
  }
  else {
    if (useLink_)
      stat_ = fstat_;
    else
      stat_ = lstat_;
  }

  ctime_ = new CTime(stat_->st_ctime);
  mtime_ = new CTime(stat_->st_mtime);
  atime_ = new CTime(stat_->st_atime);

  return true;
}

bool
ClsFile::
getFStat()
{
  if (fstat_)
    return true;

  fstat_ = new struct stat;

  int error = ::stat(name_.c_str(), fstat_);

  if (error != 0) {
    delete fstat_;

    fstat_ = nullptr;

    return false;
  }

  return true;
}

bool
ClsFile::
getLStat()
{
  if (lstat_)
    return true;

  lstat_ = new struct stat;

  int error = ::lstat(name_.c_str(), lstat_);

  if (error != 0) {
    if (! ls_->isSilent())
      std::cerr << name_ << ": " << strerror(errno) << "\n";

    delete lstat_;

    lstat_ = nullptr;

    return false;
  }

  return true;
}

const std::string &
ClsFile::
getFullLinkName()
{
  if (! linkRead_) {
    static char link_name[MAXPATHLEN + 1];

    auto len = readlink(getName().c_str(), link_name, MAXPATHLEN);

    if (len == -1)
      len = 0;

    link_name[len] = '\0';

    fullLinkName_ = link_name;

    linkRead_ = true;
  }

  return fullLinkName_;
}
