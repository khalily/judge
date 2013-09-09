#ifndef _OJ_PROTOCOL_H_
#define _OJ_PROTOCOL_H_

#include <string.h>
#include <stdint.h>
#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace oj {


static const uint32_t kMaxLength = 1000;

// typedef std::set<int> CallLimit;

typedef std::vector<const char*> CmdArgs;

// static CallLimit c_call_limit{
//   8,SYS_time,SYS_read, SYS_uname, SYS_write, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_munmap, SYS_mprotect, SYS_mmap, SYS_fstat, SYS_set_thread_area, 252,SYS_arch_prctl,231,0
// };

// static CallLimit cpp_call_limit{
//   8,SYS_time,SYS_read, SYS_uname, SYS_write, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_munmap, SYS_mprotect, SYS_mmap, SYS_fstat, SYS_set_thread_area, 252,SYS_arch_prctl,231,0
// };

// static CallLimit python_call_limit{
//   146,SYS_mremap,158,117,60,39,102,191, 257, SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_futex,SYS_getcwd,SYS_getdents,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_lstat,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_readlink,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_tid_address,SYS_stat,SYS_write,0
// };

// static CallLimit java_call_limit{
//   61,22,6,33,8,13,16,111,110,39,79,SYS_fcntl,SYS_getdents64 , SYS_getrlimit, SYS_rt_sigprocmask, SYS_futex, SYS_read, SYS_mmap, SYS_stat, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_readlink, SYS_munmap, SYS_close, SYS_uname, SYS_clone, SYS_uname, SYS_mprotect, SYS_rt_sigaction, SYS_getrlimit, SYS_fstat, SYS_getuid, SYS_getgid, SYS_geteuid, SYS_getegid, SYS_set_thread_area, SYS_set_tid_address, SYS_set_robust_list, SYS_exit_group,158, 0
// };

// static CallLimit pascal_call_limit{
//   SYS_open, SYS_set_thread_area, SYS_brk, SYS_read, SYS_uname, SYS_write, SYS_execve, SYS_ioctl, SYS_readlink, SYS_mmap, SYS_rt_sigaction, SYS_getrlimit, 252,191,158,231,SYS_close,SYS_exit_group,SYS_munmap,SYS_time,4,0
// };

// static CallLimit bash_call_limit{
//   22,61,56,42,41,79,158,117,60,39,102,191,183,SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_dup2,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getpgrp,SYS_getpid,SYS_getppid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_stat,SYS_uname,SYS_write,14,0
// };

// static CallLimit perl_call_limit{
//   78,158,117,60,39,102,191,SYS_access,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_futex,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_readlink,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_thread_area,SYS_set_tid_address,SYS_stat,SYS_time,SYS_uname,SYS_write,0
// };

// static CallLimit ruby_call_limit{
//   16,22,72,98,131,340,4,126,SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fstat,SYS_futex,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getuid,SYS_getrlimit,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_tid_address,SYS_write,0
// };


enum CodeType {
  kTypeC = 0,
  kTypeCpp,
  kTypePython,
  kTypeJava,
  kTypePascal,
  kTypeBash,
  kTypePerl,
  kTypeRuby,
  kTypeUnknown,
};

// static std::map<uint32_t, CallLimit> call_limit_list{
//   {kTypeC, c_call_limit},
//   {kTypeCpp, cpp_call_limit},
//   {kTypePython, python_call_limit},
//   {kTypeJava, java_call_limit},
//   {kTypePascal, pascal_call_limit},
//   {kTypeBash, bash_call_limit},
//   {kTypePerl, perl_call_limit},
//   {kTypeRuby, ruby_call_limit}
// };

enum JudgeResult {
  kAC = 0,   //0 Accepted
  kPE,       //1 Presentation Error
  kTLE,      //2 Time Limit Exceeded
  kMLE,      //3 Memory Limit Exceeded
  kWA,       //4 Wrong Answer
  kRE,       //5 Runtime Error
  kOLE,      //6 Output Limit Exceeded
  kCE,       //7 Compile Error
  kSE,       //8 System Error
  kUKE,       //9 Unknown Errir
};

static std::map<JudgeResult, const char *> judgeStringResult{
  {kAC, "Accepted"},
  {kPE, "Presentation Error"},
  {kTLE, "Time Limit Exceeded"},
  {kMLE, "Memory Limit Exceeded"},
  {kWA, "Wrong Answer"},
  {kRE, "Runtime Error"},
  {kOLE, "Output Limit Exceeded"},
  {kCE, "Compile Error"},
  {kSE, "System Error"},
  {kUKE, "Unknown Error"}
};

enum ReturnCode {
  kOk,            // continue
  kError,         // over
  kUnknown,
};

struct Results {
  Results() :
      judge_result(kUKE),
      time_used(0),
      memory_used(0),
      signum_result(-1),
      run_id(0) { }

  JudgeResult judge_result;
  uint32_t time_used;
  uint32_t memory_used;
  int signum_result;
  uint32_t run_id;
  std::string compile_error;
};

struct RunConfigure {
  uint32_t run_id;
  uint32_t program_id;
  uint32_t time_limit;
  uint32_t memory_limit;
  uint32_t code_type;
  uint32_t run_times;
  int32_t runer;
  bool trace;
  std::string source_code;
};

struct IOFileno
{
  int right_input_fileno;
  int right_output_fileno;
  int user_output_fileno;
};

struct ExecuteCondtion {
  uint32_t time_limit;
  uint32_t memory_limit;
  int32_t runer;
  bool trace;
  uint32_t memory_add;
};

static void printError(const char* error) {
  perror(error);
  exit(errno);
}

class File {
 public:
  static File* CreateFile(const char* filepath,
                         int oflag,
                         mode_t mode) {
    return new File(filepath, oflag, mode);
  }
  static File* OpenFile(const char* filepath,
                       int oflag) {
    return new File(filepath, oflag);
  }
  int fileno() const {
    return fd_;
  }
  ~File() {
    close(fd_);
    // printf("close fd: %s %s\n", filepath_.c_str(), strerror(errno));
  }

 private:
  File(const char* filepath, int oflag) {
    if ((fd_ = open(filepath, oflag)) < 0) {
      printError("open file error");
    }
    filepath_ = filepath;
  }
  File(const char* filepath, int oflag, mode_t mode) {
    if ((fd_ = open(filepath, O_CREAT | oflag, mode)) < 0) {
      printError("create file error");
    }
    filepath_ = filepath;
  }
  std::string filepath_;
  int fd_;
};

class FileManger : public std::vector<File*> {
 public:
  FileManger() :
      isClosed(true)
       { }
  virtual ~FileManger() {
    closeAll();
  }
  virtual void push_back (File*& val) {
    std::vector<File*>::push_back(val);
    // ((std::vector<File*>*)(this))->push_back(val);
    isClosed = false;
  }

  void closeAll() {
    if (!isClosed) {
      // printf("closeAll\n");
      for (auto iter = begin(); iter != end(); ++iter) {
        delete *iter;
      }
      clear();
      isClosed = true;
    }
  }
 private:
  bool isClosed;
};

}
#endif
