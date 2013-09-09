#ifndef _OJ_JUDGE_H
#define _OJ_JUDGE_H

#include "stdint.h"
#include <string>

#include "../protocol.h"

namespace oj {


class Judge {

 public:

  // Judge() {}

  Judge(const ExecuteCondtion& execute_condtion,
        const IOFileno& ioFileno,
        const CmdArgs& cmd_args,
        const CallLimit& call_limit) :
      execute_condtion_(execute_condtion),
      ioFileno_(ioFileno),
      cmd_args_(cmd_args),
      call_limit_(call_limit) { }


  ~Judge() { }


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

  const ExecuteCondtion& execute_condtion_;
  const IOFileno& ioFileno_;

  const CmdArgs& cmd_args_;
  const CallLimit& call_limit_;

  Results results_;
};

}
#endif
