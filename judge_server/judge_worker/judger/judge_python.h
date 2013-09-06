#ifndef _OJ_JUDGE_PYTHON_H_
#define _OJ_JUDGE_PYTHON_H_

#include "judger.h"

namespace oj
{

class JudgePython: public Judger
{
 public:
  JudgePython(utils::Log& log, uint32_t run_id) :
    Judger(log, run_id) { }
  ~JudgePython() { }

  virtual void storeSourceFile(const std::string& source_code,
                       // uint32_t code_type,
                       const char* work_dir_path);

  virtual bool getCmdArgs(CmdArgs& cmd_args);
  // virtual bool getCallLimit(CallLimit& call_limit);
 /* data */
};
}
#endif
