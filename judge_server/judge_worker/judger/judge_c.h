#ifndef _OJ_JUDGE_C_H_
#define _OJ_JUDGE_C_H_

#include "judger.h"

namespace oj
{

class JudgeC: public Judger
{
 public:
  JudgeC(utils::Log& log, ExecuteCondtion& execute_condtion) :
      Judger(log, execute_condtion) { }
  ~JudgeC() { }
  virtual void storeSourceFile(const std::string& source_code,
                       // uint32_t code_type,
                       const char* work_dir_path);
  virtual bool compile();
 protected:
  // virtual bool getCmdArgs(CmdArgs& cmd_args);
  virtual bool getCallLimit(CallLimit& call_limit);

 /* data */
};
}
#endif
