#include <ClsFilterData.h>
#include <Cls.h>
#include <CFileUtil.h>
#include <CGlob.h>
#include <CTime.h>

#define SECONDS_PER_DAY (24*60*60)

ClsFilterData::
ClsFilterData()
{
}

void
ClsFilterData::
addIncludeFileType(const std::string &fileType)
{
  includeFileTypes_.push_back(new CGlob(fileType));

  filtered_ = true;
}

void
ClsFilterData::
addExcludeFileType(const std::string &fileType)
{
  excludeFileTypes_.push_back(new CGlob(fileType));

  filtered_ = true;
}

void
ClsFilterData::
addPrefix(const std::string &prefix)
{
  prefixes_.push_back(prefix);

  filtered_ = true;
}

void
ClsFilterData::
addMatch(const std::string &pattern)
{
  match_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

void
ClsFilterData::
addNoMatch(const std::string &pattern)
{
  nomatch_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

void
ClsFilterData::
addLinkMatch(const std::string &pattern)
{
  link_match_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

void
ClsFilterData::
addNoLinkMatch(const std::string &pattern)
{
  link_nomatch_patterns_.push_back(new CGlob(pattern));

  filtered_ = true;
}

bool
ClsFilterData::
checkFile(Cls *cls, ClsFile *file) const
{
  if (! filtered_)
    return true;

  //-----

  // some tests only apply to regular files
  bool isDir = file->isDir();

  //-----

  if (! isDir && (! show_zero_ || ! show_non_zero_)) {
    size_t size = file->getSize();

    if ((size == 0 && ! show_zero_) || (size != 0 && ! show_non_zero_))
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
  auto num_match_patterns = match_patterns_.size();

  if (num_match_patterns > 0) {
    bool rc = false;

    const auto &name = file->getName();

    for (size_t i = 0; i < num_match_patterns; ++i) {
      if (match_patterns_[i]->compare(name)) {
        rc = true;
        break;
      }
    }

    if (! rc)
      return false;
  }

  //-----

  // if any no-match patterns then fail if matches any
  auto num_nomatch_patterns = nomatch_patterns_.size();

  if (num_nomatch_patterns > 0) {
    const auto &name = file->getName();

    for (size_t i = 0; i < num_nomatch_patterns; ++i) {
      if (nomatch_patterns_[i]->compare(name))
        return false;
    }
  }

  //-----

  if (file->isLink()) {
    // if any link match patterns it must match one
    auto num_link_match_patterns = link_match_patterns_.size();

    if (num_link_match_patterns > 0) {
      bool rc = false;

      const auto &linkName = file->getFullLinkName();

      for (size_t i = 0; i < num_link_match_patterns; ++i) {
        if (link_match_patterns_[i]->compare(linkName)) {
          rc = true;
          break;
        }
      }

      if (! rc)
        return false;
    }

    //-----

    // if any no-match link patterns then fail if matches any
    auto num_link_nomatch_patterns = link_nomatch_patterns_.size();

    if (num_link_nomatch_patterns > 0) {
      const auto &linkName = file->getFullLinkName();

      for (size_t i = 0; i < num_link_nomatch_patterns; ++i) {
        if (link_nomatch_patterns_[i]->compare(linkName))
          return false;
      }
    }
  }

  //-----

  // if newer specified then fail if older
  if (newer_ >= 0) {
    long diff = static_cast<long>(cls->getCurrentTime()->diff(*file->getMTime()));

    long days = diff/SECONDS_PER_DAY;

    if (days > newer_)
      return false;
  }

  //-----

  // if older specified then fail if newer
  if (older_ >= 0) {
    long diff = static_cast<long>(cls->getCurrentTime()->diff(*file->getMTime()));

    long days = diff/SECONDS_PER_DAY;

    if (days <= older_)
      return false;
  }

  //-----

  // if larger specified then fail if smaller or equal
  if (! isDir && larger_ >= 0) {
    if (file->getSize() <= static_cast<uint>(larger_))
      return false;
  }

  //-----

  // if smaller specified then fail if larger or equal
  if (! isDir && smaller_ >= 0) {
    if (file->getSize() >= static_cast<uint>(smaller_))
      return false;
  }

  //-----

  // run filter script/command
  if (exec_ != "") {
    int rc = cls->execFile(file, exec_);

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

  if ((file->isFIFO   () && (includeFlags_ & int(ClsFileType::FIFO))) ||
      (file->isChar   () && (includeFlags_ & int(ClsFileType::CHR ))) ||
      (file->isDir    () && (includeFlags_ & int(ClsFileType::DIR ))) ||
      (file->isBlock  () && (includeFlags_ & int(ClsFileType::BLK ))) ||
      (file->isRegular() && (includeFlags_ & int(ClsFileType::REG ))) ||
      (file->isLink   () && (includeFlags_ & int(ClsFileType::LNK ))) ||
      (file->isSocket () && (includeFlags_ & int(ClsFileType::SOCK))))
    return true;

  if ((includeFlags_ & int(ClsFileType::EXEC)) && file->isUserExecutable())
    return true;

  if ((includeFlags_ & int(ClsFileType::ELF)) && file->isElf())
    return true;

  if ((includeFlags_ & int(ClsFileType::BAD)) &&
      (cls->isBadFile(file) || ! file->hasLinkStat()))
    return true;

  ClsColorType color;

  if ((includeFlags_ & int(ClsFileType::SPECIAL)) &&
      cls->specialGlobMatch(file->getName(), &color))
    return true;

  CFileType type = cls->getDataType(file);

  std::string typeStr = CFileUtil::getTypeStr(type);

  for (const auto &includeFileType : includeFileTypes_)
    if (includeFileType->compare(typeStr))
      return true;

  return false;
}

bool
ClsFilterData::
checkExcludeFile(Cls *cls, ClsFile *file) const
{
  if (excludeFlags_ == 0 && excludeFileTypes_.empty())
    return false;

  if ((file->isFIFO   () && (excludeFlags_ & int(ClsFileType::FIFO))) ||
      (file->isChar   () && (excludeFlags_ & int(ClsFileType::CHR ))) ||
      (file->isDir    () && (excludeFlags_ & int(ClsFileType::DIR ))) ||
      (file->isBlock  () && (excludeFlags_ & int(ClsFileType::BLK ))) ||
      (file->isRegular() && (excludeFlags_ & int(ClsFileType::REG ))) ||
      (file->isLink   () && (excludeFlags_ & int(ClsFileType::LNK ))) ||
      (file->isSocket () && (excludeFlags_ & int(ClsFileType::SOCK))))
    return true;

  if ((excludeFlags_ & int(ClsFileType::EXEC)) && file->isUserExecutable())
    return true;

  if ((excludeFlags_ & int(ClsFileType::ELF)) && file->isElf())
    return true;

  if ((excludeFlags_ & int(ClsFileType::BAD)) &&
      (cls->isBadFile(file) || ! file->hasLinkStat()))
    return true;

  ClsColorType color;

  if ((excludeFlags_ & int(ClsFileType::SPECIAL)) &&
      cls->specialGlobMatch(file->getName(), &color))
    return true;

  CFileType type = cls->getDataType(file);

  std::string typeStr = CFileUtil::getTypeStr(type);

  for (const auto &excludeFileType : excludeFileTypes_)
    if (excludeFileType->compare(typeStr))
      return true;

  return false;
}
