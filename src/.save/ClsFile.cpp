#include <ClsFile.h>

#include <CTime.h>
#include <COSFile.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#if 0
#include <CFileUtil.h>
#endif

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

  if (isLink())
    return false;

#if 0
  FILE *file = fopen(name_.c_str(), "r");

  bool rc = isELF(file);

  fclose(fp);

  return rc;
#endif
  return false;
}

size_t
ClsFile::
getSize()
{
  if (! init())
    return false;

  return stat_->st_size;
}

uint
ClsFile::
getUid()
{
  if (! init())
    return false;

  return stat_->st_uid;
}

uint
ClsFile::
getGid()
{
  if (! init())
    return false;

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
    return 0;

  return ctime_;
}

CTime *
ClsFile::
getMTime()
{
  if (! init())
    return 0;

  return mtime_;
}

CTime *
ClsFile::
getATime()
{
  if (! init())
    return 0;

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

  return stat_->st_nlink;
}

uint
ClsFile::
getDev()
{
  if (! init())
    return 0;

  return stat_->st_dev;
}

uint
ClsFile::
getRDev()
{
  if (! init())
    return 0;

  return stat_->st_rdev;
}

ulong
ClsFile::
getNumBlocks()
{
  if (! init())
    return 0;

  return stat_->st_blocks;
}

bool
ClsFile::
hasLinkStat()
{
  if (! init())
    return false;

  return (l_stat_ != 0 && f_stat_ != 0);
}

bool
ClsFile::
init()
{
  if (initialized_) {
    if (f_stat_ == 0 && l_stat_ == 0)
      return false;

    return true;
  }

  initialized_ = true;

  get_fstat();
  get_lstat();

  if (f_stat_ == 0 && l_stat_ == 0)
    return false;

  if      (f_stat_ == 0) {
    stat_ = l_stat_;
  }
  else if (l_stat_ == 0) {
    stat_ = f_stat_;
  }
  else {
    if (use_link_)
      stat_ = f_stat_;
    else
      stat_ = l_stat_;
  }

  ctime_ = new CTime(stat_->st_ctime);
  mtime_ = new CTime(stat_->st_mtime);
  atime_ = new CTime(stat_->st_atime);

  return true;
}

bool
ClsFile::
get_fstat()
{
  if (f_stat_ != 0)
    return true;

  f_stat_ = new struct stat;

  int error = ::stat(name_.c_str(), f_stat_);

  if (error != 0) {
    delete f_stat_;

    f_stat_ = 0;

    return false;
  }

  return true;
}

bool
ClsFile::
get_lstat()
{
  if (l_stat_ != 0)
    return true;

  l_stat_ = new struct stat;

  int error = ::lstat(name_.c_str(), l_stat_);

  if (error != 0) {
    std::cerr << name_ << ": " << strerror(errno) << std::endl;

    delete l_stat_;

    l_stat_ = 0;

    return false;
  }

  return true;
}
