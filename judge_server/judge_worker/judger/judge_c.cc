#include "judge_c.h"

#include <stdio.h>

using namespace std;

namespace oj
{


void JudgeC::storeSourceFile(const std::string &source_code,
                             const char *work_dir_path) {
  sprintf(&source_file_path_[0], "%s/main.c", work_dir_path);

  sprintf(&execute_file_path_[0], "%s/main", work_dir_path);

  sprintf(&compile_error_file_[0], "%s/error_info", work_dir_path);

  writeToFile(source_code);
}

bool JudgeC::compile() {
  vector<char> cmds(500);
  sprintf(&cmds[0], "gcc %s -o %s 2> %s",
          &source_file_path_[0],
          &execute_file_path_[0],
          &compile_error_file_[0]
          );
  if (system(&cmds[0]) == -1) {
    // printf("system error\n");
    log_ << "[error] compile fail: " << utils::strErr() << log_.endl();
    return false;
  }
  // log_ << "compile finish " << log_.endl();
  if (hasData(&compile_error_file_[0])) {

    // log_ << "hasData ..." << log_.endl();
    vector<char> data(50);
    readFromFile(&compile_error_file_[0], data);
    results_.compile_error.assign(&data[0], data.size());
    return false;
  }
  // log_ << "no data ..." << log_.endl();
  return true;
}

// bool JudgeC::getCmdArgs(CmdArgs& cmd_args) {
//   cmd_args.push_back(&execute_file_path_[0]);
//   cmd_args.push_back(NULL);
//   return true;
// }

bool JudgeC::getCallLimit(CallLimit& call_limit) {
  call_limit = call_limit_list[kTypeC];
  return true;
}

}
