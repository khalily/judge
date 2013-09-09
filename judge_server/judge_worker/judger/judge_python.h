#ifndef _OJ_JUDGE_PYTHON_H_
#define _OJ_JUDGE_PYTHON_H_

#include "judger.h"

namespace oj
{

class JudgePython: public Judger
{
 public:
  JudgePython(utils::Log& log, ExecuteCondtion& execute_condtion) :
      Judger(log, execute_condtion) { }
  ~JudgePython() { }

  virtual void storeSourceFile(const std::string& source_code,
                       // uint32_t code_type,
                       const char* work_dir_path);

  virtual bool execute(const IOFileno& ioFileno);
  virtual bool compile();
  virtual bool getCmdArgs(CmdArgs& cmd_args);
  virtual bool getCallLimit(CallLimit& call_limit);
};
}
#endif
