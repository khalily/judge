#include "judger.h"
#include "judge.h"

#include "checker.h"

using namespace std;

namespace oj
{

bool Judger::execute(const ExecuteCondtion &execute_condtion,
                     const IOFileno& ioFileno) {

//   auto input_file = utils::File::OpenFile(input_file_path.c_str(), O_RDONLY);
  CmdArgs cmd_args;
  if (!getCmdArgs(cmd_args)) {
    log_ << "[error] getCmdArgs fail: " << utils::strErr() << log_.endl();
  }

  CallLimit call_limit;
  if (!getCallLimit(call_limit)) {
    log_ << "[error] getCallLimit fail: " << utils::strErr() << log_.endl();
  }

  // log_ << "user_output_fileno: " << ioFileno.user_output_fileno << log_.endl();
  Judge judge(execute_condtion,
              ioFileno,
              cmd_args,
              call_limit);

  switch(judge.Run()) {

    case kOk: {
      results_ = judge.get_results();

      // log_ << "[judge process] judge result: " << results_.judge_result << log_.endl();
      log_ << "[judge process] execute finish, start check ... " << log_.endl();
      Checker checker(ioFileno.user_output_fileno,
                      ioFileno.right_output_fileno);
      if (checker.Run() == kError) {
        results_.judge_result = checker.get_result();
        // log_ << "[judge process] checker result: " << results_.judge_result << log_.endl();
        return false;
      }
      // log_ << "[judge process] checker result: " << results_.judge_result << log_.endl();
      return true;
    }
    case kError: {
      Checker checker(ioFileno.user_output_fileno,
                      ioFileno.right_output_fileno);
      if (checker.Run() == kError) {
        results_.judge_result = checker.get_result();
        // log_ << "[judge process] checker result: " << results_.judge_result << log_.endl();
        return false;
      }
      results_ = judge.get_results();
      // log_ << "[judge process] judge result: " << results_.judge_result << log_.endl();
      log_ << "[judge process] execute interrupt ... " << log_.endl();
      return false;
    }
    case kUnknown: {
      break;
    }

  }
  return false;
}

bool Judger::getCmdArgs(CmdArgs& cmd_args) {
  cmd_args.push_back(&execute_file_path_[0]);
  cmd_args.push_back(NULL);
  return true;
}



// }

}
