#ifndef COS_H
#define COS_H

#include <pwd.h>
#include <grp.h>

namespace COS {
  std::string getUserName(uint uid) {
    struct passwd *pw = getpwuid((uid_t) uid);

    std::string name;

    if (pw != NULL)
      name = pw->pw_name;
    else {
      char *name1 = getlogin();

      if (name1 != NULL)
        name = name1;
      else {
        char buffer[32];

        sprintf(buffer, "%d", uid);

        name = buffer;
      }
    }

    return name;
  }

  std::string getGroupName(uint gid) {
    struct group *gr = getgrgid((gid_t) gid);

    std::string group_name;

    if (gr != NULL)
      group_name = gr->gr_name;
    else {
      char buffer[32];

      sprintf(buffer, "%d", gid);

      group_name = buffer;
    }

    return group_name;
  }
}

#endif
