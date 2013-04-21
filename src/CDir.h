#ifndef CDIR_H
#define CDIR_H

#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>

namespace CDir {
  std::vector<int> dirStack_;
  std::string      errorMsg_;

  std::string getCurrent() {
    char dirname1[MAXPATHLEN];

    getcwd(dirname1, MAXPATHLEN);

    return dirname1;
  }

  bool changeDir(const std::string &dirname) {
    int error = chdir(dirname.c_str());

    if (error != 0) {
      errorMsg_ = "failed to change to directory '" + dirname + "'";
      return false;
    }

    return true;
  }

  bool enter(const std::string &dirname) {
    int dir_fd = open(".", O_RDONLY);

    if (dir_fd < 0) {
      errorMsg_ = "Failed to open current dir";
      return false;
    }

    if (! changeDir(dirname))
      return false;

    dirStack_.push_back(dir_fd);

    return true;
  }

  bool leave() {
    if (dirStack_.empty()) {
      errorMsg_ = "enter/leave mismatch";

      return false;
    }

    int dir_fd = dirStack_[dirStack_.size() - 1];

    dirStack_.pop_back();

    if (fchdir(dir_fd) < 0) {
      errorMsg_ = "failed to change to original dir";

      close(dir_fd);

      return false;
    }

    close(dir_fd);

    return true;
  }

  std::string getErrorMsg() { return errorMsg_; }
};

#endif
