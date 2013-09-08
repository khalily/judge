#include "judge_python.h"
#include <stdio.h>

using namespace std;

namespace oj
{


void JudgePython::storeSourceFile(const std::string &source_code,
                               const char *work_dir_path) {

  source_file_path_ = (boost::format("%s/main.py") % work_dir_path).str();

  execute_file_path_ = (boost::format("%s/main.py") % work_dir_path).str();

  compile_error_file_ = (boost::format("%s/error_info") % work_dir_path).str();

  writeToFile(source_code);
}

bool JudgePython::getCmdArgs(CmdArgs& cmd_args) {
  cmd_args.push_back("python");
  cmd_args.push_back(execute_file_path_.c_str());
  cmd_args.push_back(NULL);
  return true;
}

bool JudgePython::getCallLimit(CallLimit& call_limit) {
  call_limit = call_limit_list[kTypePython];
  return true;
}

}
