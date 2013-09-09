#ifndef _OJ_JUDGE_JAVA_H_
#define _OJ_JUDGE_JAVA_H_

#include "judger.h"
#include <string>

namespace oj
{

class JudgeJava: public Judger
{
 public:
  JudgeJava(utils::Log& log, ExecuteCondtion& execute_condtion) :
      Judger(log, execute_condtion) { }
  ~JudgeJava() { }
  virtual bool execute(const IOFileno& ioFileno);
  virtual void storeSourceFile(const std::string& source_code,
                       // uint32_t code_type,
                       const char* work_dir_path);
  virtual bool compile();

 private:
  virtual bool getCallLimit(CallLimit& call_limit);
  virtual bool getCmdArgs(CmdArgs& cmd_args);
  std::string classpath_;
};
}
#endif
