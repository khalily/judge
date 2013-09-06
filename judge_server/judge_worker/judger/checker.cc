#include "checker.h"
#include <sys/mman.h>
#include <sys/stat.h>

namespace oj {


ReturnCode Checker::Run() {
  if (kError == CheckOutLength()) {
    return kError;
  }
  MemoryMap();

  if (EqualStr(userout_, rightout_)) {
    UnMemoryMap();
    result_ = kAC;
    return kOk;
  }

  if (IsPE() == true) {
    result_ = kPE;
  } else {
    result_ = kWA;
  }
  UnMemoryMap();
  return kError;
}

void Checker::MemoryMap() {
  if ((userout_ = (char*) mmap(NULL,
                              userout_len_,
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE,
                              userout_fd_,
                              0)) == MAP_FAILED) {
    printError("mmap userout faild");
  }

  if ((rightout_ = (char*) mmap(NULL,
                                rightout_len_,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE,
                                rightout_fd_,
                                0)) == MAP_FAILED) {
    printError("mmap rightout faild");
  }
}

void Checker::UnMemoryMap() {
  munmap(userout_, userout_len_);
  munmap(rightout_, rightout_len_);
}

ReturnCode Checker::CheckOutLength() {
  struct stat user_buf;
  if (fstat(userout_fd_, &user_buf) == -1) {
    printError("get userout stat error");
  }

  if (user_buf.st_size > kMaxLength) {
    result_ = kOLE;
    return kError;
  } else {
    userout_len_ = user_buf.st_size;
  }
  struct stat right_buf;
  if (fstat(rightout_fd_, &right_buf) == -1) {
    printError("get rightout stat error");
  }
  rightout_len_ = right_buf.st_size;

  // printf("rightout_len: %d\t userout_len: %d\n",
  //        rightout_len_,
  //        userout_len_);
  if (userout_len_ != rightout_len_) {
    result_ = kWA;
    return kError;
  }
  return kOk;
}

bool Checker::IsPE() {
  const char *cuser = userout_, *cright = rightout_;
  const char *end_user = userout_ + userout_len_;
  const char *end_right = rightout_ + rightout_len_;
  while ((cuser < end_user) && (cright < end_right)) {
      while ((cuser < end_user)
              && (*cuser == ' ' || *cuser == '\n' || *cuser == '\r'
                      || *cuser == '\t'))
          cuser++;
      while ((cright < end_right)
              && (*cright == ' ' || *cright == '\n' || *cright == '\r'
                      || *cright == '\t'))
          cright++;
      if (cuser == end_user || cright == end_right)
          break;
      if (*cuser != *cright)
          break;
      cuser++;
      cright++;
  }
  while ((cuser < end_user)
          && (*cuser == ' ' || *cuser == '\n' || *cuser == '\r'
                  || *cuser == '\t'))
      cuser++;
  while ((cright < end_right)
          && (*cright == ' ' || *cright == '\n' || *cright == '\r'
                  || *cright == '\t'))
      cright++;
  if (cuser == end_user && cright == end_right) {
    return true;
  }
  return false;
}

}
