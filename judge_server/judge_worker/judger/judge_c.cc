#include "judge_c.h"

#include <stdio.h>

using namespace std;

namespace oj
{


void JudgeC::storeSourceFile(const std::string &source_code,
                             const char *work_dir_path) {
  // sprintf(&source_file_path_[0], "%s/main.c", work_dir_path);
  source_file_path_ = (boost::format("%s/main.c") % work_dir_path).str();

  // sprintf(&execute_file_path_[0], "%s/main", work_dir_path);
  execute_file_path_ = (boost::format("%s/main") % work_dir_path).str();

  // sprintf(&compile_error_file_[0], "%s/error_info", work_dir_path);
  compile_error_file_ = (boost::format("%s/error_info") % work_dir_path).str();

  writeToFile(source_code);
}

bool JudgeC::compile() {
  // vector<char> cmds(500);
  // sprintf(&cmds[0], "gcc %s -o %s 2> %s",
  //         &source_file_path_[0],
  //         &execute_file_path_[0],
  //         &compile_error_file_[0]
  //         );
  string cmds = (boost::format("gcc %s -o %s 2> %s")
                 % source_file_path_
                 % execute_file_path_
                 % compile_error_file_).str();
  if (system(cmds.c_str()) == -1) {
    // printf("system error\n");
    log_ << "[error] compile fail: " << utils::strErr() << log_.endl();
    return false;
  }
  // log_ << "compile finish " << log_.endl();
  if (hasData(compile_error_file_.c_str())) {

    // log_ << "hasData ..." << log_.endl();
    vector<char> data(50);
    readFromFile(compile_error_file_.c_str(), data);
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
