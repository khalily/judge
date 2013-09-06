#include "judge_python.h"
#include <stdio.h>

using namespace std;

namespace oj
{


void JudgePython::storeSourceFile(const std::string &source_code,
                               const char *work_dir_path) {
  sprintf(&source_file_path_[0], "%s/main.py", work_dir_path);

  sprintf(&execute_file_path_[0], "%s/main.py", work_dir_path);

  sprintf(&compile_error_file_[0], "%s/error_info", work_dir_path);

  writeToFile(source_code);
}

bool JudgePython::getCmdArgs(CmdArgs& cmd_args) {
  cmd_args.push_back("python");
  cmd_args.push_back(&execute_file_path_[0]);
  cmd_args.push_back(NULL);
  return true;
}

}
