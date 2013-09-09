#include "judge.h"
#include "stdio.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <errno.h>
#include <string>
#include <vector>

using namespace std;

namespace oj {

ReturnCode Judge::Run() {
  pid_t pid;

  if ((pid = vfork()) < 0) {

    printError("vfork error");
  }
  if (pid == 0) {               // child process

    // SetResourceLimit();
    printf("%ld\n", cmd_args_.size());
    for (int i = 0; i != cmd_args_.size(); ++i) {
      printf("cmd_args[%d] : %s\n", i, cmd_args_[i]);
    }
    printf("trace: %d\n", execute_condtion_.trace);
    SetIOEnv();
    setRunner();

    if (execute_condtion_.trace) {
      ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    }

    execvp(cmd_args_[0], (char* const *)&cmd_args_[0]);

    printError("execvp error");
  } else {                      // parent process

    if (execute_condtion_.trace) {
      return TraceLoop(pid);
    }
    else
      WaitExit(pid);
  }
  return kOk;
}

void Judge::SetResourceLimit() {
  struct rlimit rl;
  rl.rlim_cur = execute_condtion_.time_limit / 1000 + 2;
  rl.rlim_max = rl.rlim_cur + 1;
  if (setrlimit(RLIMIT_CPU, &rl)) {
    printError("rlimit cpu error");
  }

  rl.rlim_cur = 256 * 1024 * 1024;
  rl.rlim_max = rl.rlim_cur + 1024;
  if (setrlimit(RLIMIT_AS, &rl)) {
    printError("rlimit stack error");
  }
}

void Judge::SetIOEnv() {

  if (dup2(ioFileno_.right_input_fileno, 0) == -1) {
    printError("dup2 infile error");
  }

  if (dup2(ioFileno_.user_output_fileno, 1) == -1) {
    printError("dup2 outfile error");
  }
}

void Judge::WaitExit(pid_t pid) {
  int status;
  struct rusage resource;
  if (wait4(pid, &status, 0, &resource) == -1) {
    printf("wait4 error\n");
  }

  results_.time_used = resource.ru_utime.tv_sec * 1000 + \
                       resource.ru_utime.tv_usec / 1000;
  printf("_SC_PAGESIZE: %ld\n", sysconf(_SC_PAGESIZE));
  printf("ru_minflt: %ld\n", resource.ru_minflt);
  results_.memory_used = resource.ru_minflt * \
                         (sysconf(_SC_PAGESIZE) / 1024) -
                         execute_condtion_.memory_add;

  if (WIFSIGNALED(status)) {
    switch (WTERMSIG(status)) {
      case SIGSEGV:
        if (results_.memory_used > execute_condtion_.memory_limit)
            results_.judge_result = kMLE;
        else
            results_.judge_result = kRE;
        break;
      case SIGALRM:
      case SIGXCPU:
        results_.judge_result = kTLE;
        break;
      default:
        results_.judge_result = kRE;
        break;
    }
    results_.signum_result = WTERMSIG(status);
  } else {
    if (results_.time_used > execute_condtion_.time_limit)
      results_.judge_result = kTLE;
    else if (results_.memory_used > execute_condtion_.memory_limit)
      results_.judge_result = kMLE;
    else
      results_.judge_result = kAC;
  }

}

ReturnCode Judge::TraceLoop(pid_t pid) {
  int status;
  struct rusage resource;
  struct user_regs_struct regs;
  bool in_syscall = false;

  while (true) {
    if (wait4(pid,
             &status,
             WSTOPPED,
             &resource) == -1) {
      printError("wait4 WSTOPPED error");
    }
    if (WIFEXITED(status)) {
      break;
    } else if (WSTOPSIG(status) != SIGTRAP) {
      ptrace(PTRACE_KILL, pid, NULL, NULL);
      waitpid(pid, NULL, 0);

      results_.time_used = resource.ru_utime.tv_sec * 1000
                + resource.ru_utime.tv_usec / 1000;
      results_.memory_used = resource.ru_minflt *
                  (sysconf(_SC_PAGESIZE) / 1024) -
                  execute_condtion_.memory_add;

      switch (WSTOPSIG(status)) {
          case SIGSEGV:
              if (results_.memory_used > execute_condtion_.memory_limit)
                  results_.judge_result = kMLE;
              else
                  results_.judge_result = kRE;
              break;
          case SIGALRM:
          case SIGXCPU:
              results_.judge_result = kTLE;
              break;
          default:
              results_.judge_result = kRE;
              break;
      }
      results_.signum_result = WSTOPSIG(status);
      return kError;
    } else {
      if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        printError("ptrace getregs error");
      }
      if (in_syscall) {
        auto re = call_limit_.end();
        if (call_limit_.find(regs.orig_rax) == re) {
          printf("syscall number: %lld\n", regs.orig_rax);
          ptrace(PTRACE_KILL, pid, NULL, NULL);
          waitpid(pid, NULL, 0);
          results_.time_used = resource.ru_utime.tv_sec * 1000
                    + resource.ru_utime.tv_usec / 1000;
          results_.memory_used = resource.ru_minflt *
                     (sysconf(_SC_PAGESIZE) / 1024);
          results_.judge_result = kRE;
          return kError;
        }
      } else {
        in_syscall = true;
      }
      ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }
  }
  results_.time_used = resource.ru_utime.tv_sec * 1000
                + resource.ru_utime.tv_usec / 1000;
  results_.memory_used = resource.ru_minflt *
              (sysconf(_SC_PAGESIZE) / 1024) -
              execute_condtion_.memory_add;
  printf("memory_add: %d\n", execute_condtion_.memory_add);
  if (results_.time_used > execute_condtion_.time_limit)
    results_.judge_result = kTLE;
  else if (results_.memory_used > execute_condtion_.memory_limit)
    results_.judge_result = kMLE;
  else
    results_.judge_result = kAC;
  return kOk;
}

}
