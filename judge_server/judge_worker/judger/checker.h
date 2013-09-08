#ifndef _OJ_CHECKER_H_
#define _OJ_CHECKER_H_

#include "../../log.hpp"
#include "../protocol.h"

namespace oj {

class Checker {
 public:
  Checker(int user_output_fileno,
          int right_output_fileno,
          utils::Log& log) :
       userout_(NULL),
       rightout_(NULL),
       result_(kUKE),
       userout_len_(-1),
       rightout_len_(-1),
       userout_fd_(user_output_fileno),
       rightout_fd_(right_output_fileno),
       isLengthMax_(false),
       log_(log) { }

  ReturnCode run();
  JudgeResult get_result() const {
    return result_;
  }

  ~Checker() { }

 private:
  // void MemoryMap();
  // void UnMemoryMap();
  ReturnCode checkOutLength();
  bool isEqualStr(const char *s, const char *s2) {
    const char *c1 = s, *c2 = s2;
    while (c1 && c2 && (*c1 == *c2)) {
      if (*c1 == '\0' || *c2 == '\0')
        break;
      c1++;
      c2++;
    }
    if ((*c1 | *c2) == 0)
        return true;
    return false;
  }
  bool isPE();

  bool isEqualLength() {
    return userout_len_ == rightout_len_;
  }


  const char *userout_;
  const char *rightout_;
  JudgeResult result_;
  int32_t userout_len_;
  int32_t rightout_len_;
  int userout_fd_;
  int rightout_fd_;
  bool isLengthMax_;
  utils::Log &log_;
};

}
#endif
