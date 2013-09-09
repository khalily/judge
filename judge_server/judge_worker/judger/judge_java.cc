#include "judge_java.h"

#include <string>

using namespace std;

namespace oj
{

void JudgeJava::storeSourceFile(const std::string &source_code,
                           const char *work_dir_path) {
  source_file_path_ = (boost::format("%s/Main.java") % work_dir_path).str();

  execute_file_path_ = "Main";

  compile_error_file_ = (boost::format("%s/error_info") % work_dir_path).str();

  writeToFile(source_code);
  classpath_ = work_dir_path;
}

bool JudgeJava::execute(const IOFileno& ioFileno) {
  execute_condtion_.memory_add = 1024 * 14;
  return Judger::execute(ioFileno);
}

bool JudgeJava::compile() {
  string cmds = (boost::format("javac -J-Xms32m -J-Xmx256m %s 2> %s")
                 % source_file_path_
                 % compile_error_file_).str();
  if (system(cmds.c_str()) == -1) {
    log_ << "[error] compile fail: " << utils::strErr() << log_.endl();
    results_.judge_result = kSE;
    return false;
  }
  // log_ << "compile finish " << log_.endl();
  if (hasData(compile_error_file_.c_str())) {

    vector<char> data(50);
    readFromFile(compile_error_file_.c_str(), data);
    results_.compile_error.assign(&data[0], data.size());
    results_.judge_result = kCE;
    return false;
  }
  return true;
}

bool JudgeJava::getCallLimit(CallLimit& call_limit) {
  call_limit = call_limit_list[kTypeJava];
  return true;
}

bool JudgeJava::getCmdArgs(CmdArgs& cmd_args) {
  cmd_args.push_back("java");
  cmd_args.push_back("-cp");
  cmd_args.push_back(classpath_.c_str());
  cmd_args.push_back("-Xms32m");
  cmd_args.push_back("-Xmx256m");
  cmd_args.push_back(execute_file_path_.c_str());
  cmd_args.push_back(NULL);
  return true;
}

}
