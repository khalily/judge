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

typedef std::set<int> CallLimit;

typedef std::vector<const char*> CmdArgs;

static CallLimit c_call_limit{
  SYS_open,
  SYS_write,
  SYS_read,
  SYS_close,
  SYS_time,
  SYS_access,
  SYS_times,
  SYS_brk,
  SYS_munmap,
  SYS_uname,
  SYS_mprotect,
  // SYS__llseek,
  SYS_lseek,
  SYS_writev,
  SYS_rt_sigprocmask,
  // SYS_mmap2,
  SYS_mmap,
  // SYS_fstat64,
  SYS_fstat,
  SYS_getuid,
  // SYS_getuid32,
  SYS_gettid,
  SYS_futex,
  SYS_set_thread_area,
  SYS_exit_group,
  SYS_tgkill,
  SYS_arch_prctl
};

static CallLimit cpp_call_limit{
  SYS_open,
  SYS_write
};

static CallLimit py_call_limit{
  SYS_open,
  SYS_write
};

static CallLimit java_call_limit{
  SYS_open,
  SYS_write
};

enum CodeType {
  kTypeC = 0,
  kTypeCpp,
  kTypePython,
  kTypeJava,
  kTypeUnknown,
};

static std::map<uint32_t, CallLimit> call_limit_list{
  {kTypeC, c_call_limit},
  {kTypeCpp, cpp_call_limit},
  {kTypePython, py_call_limit},
  {kTypeJava, java_call_limit}
};

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
};

struct RunPath {
  RunPath() :
      work_dir_path(255),
      input_path(255),
      right_output_path(255),
      user_output_path(255),
      input_fileno(-1),
      right_output_fileno(-1),
      user_output_fileno(-1) { }

  std::vector<char> work_dir_path;
  std::vector<char> input_path;
  std::vector<char> right_output_path;
  std::vector<char> user_output_path;
  int input_fileno;
  int right_output_fileno;
  int user_output_fileno;
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
    // printf("close fd\n");
    close(fd_);
    printf("close fd: %s %s\n", filepath_.c_str(), strerror(errno));
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
  // const char * filepath_;
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
      printf("closeAll\n");
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
