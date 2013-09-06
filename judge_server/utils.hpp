#ifndef _UTILS_UTILS_HPP_
#define _UTILS_UTILS_HPP_

#include <stdint.h>
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace utils
{


class File {
 public:
  static File CreateFile(const char* filepath,
                         int oflag,
                         mode_t mode) {
    return File(filepath, oflag, mode);
  }
  static File OpenFile(const char* filepath,
                       int oflag) {
    return File(filepath, oflag);
  }
  int fileno() const {
    return fd_;
  }
  ~File() {
    close(fd_);
  }

 private:
  File(const char* filepath, int oflag) {
    if ((fd_ = open(filepath, oflag)) < 0) {
      // printError("open file error");
    }
  }
  File(const char* filepath, int oflag, mode_t mode) {
    if ((fd_ = open(filepath, O_CREAT | oflag, mode)) < 0) {
      // printError("create file error");
    }
  }

  int fd_;
};
}


#endif
