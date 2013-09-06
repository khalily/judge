#ifndef _OJ_CHECKER_H_
#define _OJ_CHECKER_H_

#include "../protocol.h"

namespace oj {

class Checker {
 public:
  Checker(int user_output_fileno, int right_output_fileno) :
       userout_(NULL),
       rightout_(NULL),
       result_(kUKE),
       userout_len_(-1),
       rightout_len_(-1),
       userout_fd_(user_output_fileno),
       rightout_fd_(right_output_fileno) { }

  ReturnCode Run();
  JudgeResult get_result() const {
    return result_;
  }

  ~Checker() { }

 private:
  void MemoryMap();
  void UnMemoryMap();
  ReturnCode CheckOutLength();
  bool EqualStr(const char *s, const char *s2) {
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
  bool IsPE();


  char *userout_;
  char *rightout_;
  JudgeResult result_;
  int32_t userout_len_;
  int32_t rightout_len_;
  int userout_fd_;
  int rightout_fd_;
};

}
#endif
