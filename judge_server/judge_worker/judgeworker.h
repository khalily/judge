#ifndef _OJ_JUDEGWORKER_H_
#define _OJ_JUDEGWORKER_H_

#include <string>
#include "protocol.h"
#include "../zmqmsg/zmqmsg.h"
#include "../log.hpp"

namespace oj
{

class JudgeWorker
{
 public:
  JudgeWorker(const std::string& sock_back_addr,
              void* context);
  ~JudgeWorker() { }
  void setTmpDir(const std::string& tmp_dir) {
    temp_dir_ = tmp_dir;
  }
  void setProgramDir(const std::string& program_dir) {
    program_dir_ = program_dir;
  }
  void run();

  void setWorkerNum(int num) {
    worker_number_ = num;
  }

  void setLogPath(const std::string& log_path) {
    log_path_ = log_path;
  }

 private:
  bool init();
  bool buildEnv(uint32_t run_id,
                std::string& work_dir_path);       //build environment

  void cleanEnv(const char *work_dir_path) {
    removeDir(work_dir_path);
  }

  void removeDir(const char *dir_path);

  void getFileno(IOFileno& ioFileno,
                 uint32_t program_id,
                 uint32_t run_id,
                 uint32_t numofExecute);

  void preProcess(const RunPath& run_path,
                uint32_t code_type,
                std::vector<const char*> &cmd_args);

  bool processTask(const zmqmsg::ZmqMsg& msg,
                   Results& results);

  bool sendError(const zmqmsg::ZmqMsg& msg);
  bool sendResults(const zmqmsg::ZmqMsg& msg);

  ReturnCode judgeOnce(const RunConfigure &run_configure,
                       const std::vector<const char*> &cmd_args,
                       RunPath &run_path,
                       Results &results,
                       // FileList &user_output_list,
                       // FileList &right_output_list,
                       // FileList &input_list,
                       uint32_t times);
  // void InitCallLimit();
  // void ReadFromFile(std::vector<char> &data);
  void writeToFile(const std::string &source_code,
                   const char *source_code_path);

  void storeSourceCode(const std::string& source_code,
                       const char *work_dir_path,
                       uint32_t code_type);

  void parseData(const zmqmsg::ZmqMsg &rmsg,
                 RunConfigure &run_configure);

  void wrapData(const Results &results,
                zmqmsg::ZmqMsg &smsg);

  void destroy();



  std::string sock_back_addr_;
  void* context_;
  void* sock_worker_;
  utils::Log log_;

  std::string temp_dir_;
  std::string program_dir_;
  std::string log_path_;

  int worker_number_;

  FileManger file_manager_;
};
}
#endif
