#ifndef _OJ_JUDGE_CPP_H_
#define _OJ_JUDGE_CPP_H_

#include "judger.h"

namespace oj
{

class JudgeCpp: public Judger
{
 public:
  JudgeCpp(utils::Log& log, uint32_t run_id) :
      Judger(log, run_id) { }
  ~JudgeCpp() { }
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
