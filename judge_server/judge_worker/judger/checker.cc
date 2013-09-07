#include "checker.h"
#include <sys/mman.h>
#include <sys/stat.h>

#include "memmap.hpp"

namespace oj {


ReturnCode Checker::run() {
  if (kError == checkOutLength()) {
    return kError;
  }

  auto userout_map = MemoryMap(userout_len_, userout_fd_);

  if (NULL == (userout_ = userout_map.map())) {
    log_ << "[error] MemoryMap fail: " << utils::strErr() << log_.endl();
    return kError;
  }
  auto rightout_map = MemoryMap(rightout_len_, rightout_fd_);

  if (NULL == (rightout_ = rightout_map.map())) {
    log_ << "[error] MemoryMap fail: " << utils::strErr() << log_.endl();
    return kError;
  }

  if (!isEqualLength()) {
    if (isPE()) {
      result_ = kPE;
      return kError;
    }
    result_ = kWA;
    return kError;
  }

  if (isEqualStr(userout_, rightout_)) {
    result_ = kAC;
    return kOk;
  }

  if (isPE() == true) {
    result_ = kPE;
  } else {
    result_ = kWA;
  }
  return kError;
}

// void Checker::MemoryMap() {
//   if ((userout_ = (char*) mmap(NULL,
//                               userout_len_,
//                               PROT_READ | PROT_WRITE,
//                               MAP_PRIVATE,
//                               userout_fd_,
//                               0)) == MAP_FAILED) {
//     printError("mmap userout faild");
//   }

//   if ((rightout_ = (char*) mmap(NULL,
//                                 rightout_len_,
//                                 PROT_READ | PROT_WRITE,
//                                 MAP_PRIVATE,
//                                 rightout_fd_,
//                                 0)) == MAP_FAILED) {
//     printError("mmap rightout faild");
//   }
// }

// void Checker::UnMemoryMap() {
//   munmap(userout_, userout_len_);
//   munmap(rightout_, rightout_len_);
// }

ReturnCode Checker::checkOutLength() {
  struct stat user_buf;
  if (fstat(userout_fd_, &user_buf) == -1) {
    log_ << "[error] get userout stat fail: " << utils::strErr() << log_.endl();
    return kError;
  }

  if (user_buf.st_size > kMaxLength) {
    result_ = kOLE;
    isLengthMax_ = true;
    return kError;
  } else {
    userout_len_ = user_buf.st_size;
  }
  struct stat right_buf;
  if (fstat(rightout_fd_, &right_buf) == -1) {
    log_ << "[error] get rigthtout stat fail: " << utils::strErr() << log_.endl();
    return kError;
  }
  rightout_len_ = right_buf.st_size;

  // printf("rightout_len: %d\t userout_len: %d\n",
  //        rightout_len_,
  //        userout_len_);
  // if (userout_len_ != rightout_len_) {
  //   result_ = kWA;
  //   return kError;
  // }
  return kOk;
}

bool Checker::isPE() {
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
