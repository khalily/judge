#ifndef _OJ_JUDGEOFTYPE_HPP_
#define _OJ_JUDGEOFTYPE_HPP_

#include "judger.h"
#include "judge_c.h"
#include "judge_cpp.h"
#include "judge_python.h"
#include "judge_java.h"
#include "../protocol.h"

namespace oj
{

class JudgeofType
{
public:
  JudgeofType() :
    judger_(NULL),
    isConstruct_(false) {

  }
  ~JudgeofType() {
    if (isConstruct_) {
      delete judger_;
    }
  }
  Judger* constructJudger(uint32_t code_type,
                          utils::Log& log,
                          uint32_t run_id) {
    switch (code_type) {
      case kTypeC:
        judger_ = new JudgeC(log, run_id);
        break;
      case kTypeCpp:
        judger_ = new JudgeCpp(log, run_id);
        break;
      case kTypePython:
        judger_ = new JudgePython(log, run_id);
        break;
      case kTypeJava:
        judger_ = new JudgeJava(log, run_id);
        break;
      default:
        break;
    }
    isConstruct_ = true;
    return judger_;
  }

private:
  Judger *judger_;
  bool isConstruct_;
};
}
#endif
