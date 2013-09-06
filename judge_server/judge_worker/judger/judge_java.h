#ifndef _OJ_JUDGE_JAVA_H_
#define _OJ_JUDGE_JAVA_H_

#include "judger.h"

namespace oj
{

class JudgeJava: public Judger
{
 public:
  JudgeJava(utils::Log& log, uint32_t run_id) :
      Judger(log, run_id) { }
  ~JudgeJava() { }

 /* data */
};
}
#endif
