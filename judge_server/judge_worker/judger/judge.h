#ifndef _OJ_JUDGE_H
#define _OJ_JUDGE_H

#include "stdint.h"
#include <string>

// #include "preprocess.h"
#include "../protocol.h"

namespace oj {


class Judge {
 public:


 public:
  // Judge() { }

  // Judge(const char* work_dir_path, uint32_t time_limit,
  //       uint32_t memory_limit, uint16_t code_type) :
  //     work_dir_path_(work_dir_path),
  //     time_limit_(time_limit),
  //     memory_limit_(memory_limit),
  //     code_type_(code_type) { }

  // Judge(const RunConfigure& run_configure) :
  //     work_dir_path_(run_configure.work_dir_path),
  //     time_limit_(run_configure.time_limit),
  //     memory_limit_(run_configure.memory_limit),
  //     code_type_(run_configure.code_type),
  //     trace_(run_configure.trace),
  //     num_(run_configure.num) { }
  // Judge(const RunConfigure& run_configure,
  //       const RunPath& run_path,
  //       const std::vector<const char*>& cmd_args) :
  //     run_configure_(run_configure),
  //     run_path_(run_path),
  //     cmd_args_(cmd_args) { }
  Judge(const ExecuteCondtion& execute_condtion,
        const IOFileno& ioFileno,
        const CmdArgs& cmd_args,
        const CallLimit& call_limit) :
      execute_condtion_(execute_condtion),
      ioFileno_(ioFileno),
      cmd_args_(cmd_args),
      call_limit_(call_limit) { }


  ~Judge() { }

  // void set_time_limit(uint32_t time_limit) {
  //   time_limit_ = time_limit;
  // }

  // void set_memory_limit(uint32_t memory_limit) {
  //   memory_limit_ = memory_limit;
  // }

  // void set_code_type(uint16_t code_type) {
  //   code_type_ = code_type;
  // }

  ReturnCode Run();
  Results get_results() const {
    return results_;
  }

 private:
  void SetResourceLimit();
  void SetIOEnv();
  void setRunner() {
    if (execute_condtion_.runer == 0) {
      printError("can't root to run");
    } else if (execute_condtion_.runer != -1) {
      setuid(execute_condtion_.runer);
    } else {
      // printf("warning: missing runner\n");
    }
  }
  void WaitExit(pid_t pid);
  ReturnCode TraceLoop(pid_t pid);
  // std::string work_dir_path_;
  // uint32_t time_limit_;
  // uint32_t memory_limit_;

  const ExecuteCondtion& execute_condtion_;
  const IOFileno& ioFileno_;
  // uint32_t code_type_;
  // bool trace_;
  // uint32_t num_;
  // const RunConfigure& run_configure_;
  // const RunPath& run_path_;
  const CmdArgs& cmd_args_;
  const CallLimit& call_limit_;

  Results results_;
};

}
#endif
