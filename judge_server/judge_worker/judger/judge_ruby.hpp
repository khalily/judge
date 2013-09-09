#ifndef _JUDGE_RUBY_HPP_
#define _JUDGE_RUBY_HPP_

#include "judger.h"
namespace oj
{

class JudgeRuby: public Judger
{
 public:
  JudgeRuby(utils::Log& log, ExecuteCondtion& execute_condtion) :
      Judger(log, execute_condtion) { }
  ~JudgeRuby() { }

  virtual void storeSourceFile(const std::string& source_code,
                       // uint32_t code_type,
                       const char* work_dir_path);

  virtual bool execute(const IOFileno& ioFileno);

  virtual bool getCmdArgs(CmdArgs& cmd_args);
  virtual bool getCallLimit(CallLimit& call_limit);
};

void JudgeRuby::storeSourceFile(const std::string &source_code,
                               const char *work_dir_path) {

  source_file_path_ = (boost::format("%s/main.rb") % work_dir_path).str();

  execute_file_path_ = (boost::format("%s/main.rb") % work_dir_path).str();

  compile_error_file_ = (boost::format("%s/error_info") % work_dir_path).str();

  writeToFile(source_code);
}

bool JudgeRuby::execute(const IOFileno& ioFileno) {
  execute_condtion_.memory_add = 1024 * 2;
  return Judger::execute(ioFileno);
}

bool JudgeRuby::getCmdArgs(CmdArgs& cmd_args) {
  cmd_args.push_back("ruby");
  cmd_args.push_back(execute_file_path_.c_str());
  cmd_args.push_back(NULL);
  return true;
}

bool JudgeRuby::getCallLimit(CallLimit& call_limit) {
  call_limit = call_limit_list[kTypeRuby];
  return true;
}

}
#endif
