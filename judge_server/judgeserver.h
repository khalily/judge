#ifndef _OJ_JUDGESERVER_H
#define _OJ_JUDGESERVER_H

// #include <zmq.h>
#include <string>
#include <vector>
#include "thread.hpp"
#include "log.hpp"

namespace oj
{

class JudgeWorker;
class JudgeServer
{
 public:
  JudgeServer(const std::string& sock_front_addr,
              const std::string& sock_back_addr,
              int worker_num = 1);
  ~JudgeServer();
  void run();

 private:
  bool init();
  void destroy();
  void startWorker();
  const std::string sock_front_addr_;
  const std::string sock_back_addr_;
  int worker_num_;
  void* context_;
  void* sock_front_;
  void* sock_back_;
  utils::Log log_;

  ThreadPool threadPool_;
  std::vector<JudgeWorker*> workers_;
};
}
#endif
