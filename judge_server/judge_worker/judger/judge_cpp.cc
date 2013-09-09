#include "judge_cpp.h"

#include <stdio.h>

using namespace std;

namespace oj
{


void JudgeCpp::storeSourceFile(const std::string &source_code,
                               const char *work_dir_path) {

  source_file_path_ = (boost::format("%s/main.c") % work_dir_path).str();

  execute_file_path_ = (boost::format("%s/main") % work_dir_path).str();

  compile_error_file_ = (boost::format("%s/error_info") % work_dir_path).str();
  writeToFile(source_code);
}

bool JudgeCpp::compile() {

  string cmds = (boost::format("g++ %s -o %s 2> %s")
                 % source_file_path_
                 % execute_file_path_
                 % compile_error_file_).str();
  if (system(cmds.c_str()) == -1) {
    log_ << "[error] compile fail: " << utils::strErr() << log_.endl();
    results_.judge_result = kSE;
    return false;
  }

  if (hasData(compile_error_file_.c_str())) {
    vector<char> data(50);
    readFromFile(compile_error_file_.c_str(), data);
    results_.compile_error.assign(&data[0], data.size());
    results_.judge_result = kCE;
    return false;
  }
  return true;
}

bool JudgeCpp::getCallLimit(CallLimit& call_limit) {
  call_limit = call_limit_list[kTypeCpp];
  return true;
}

}
