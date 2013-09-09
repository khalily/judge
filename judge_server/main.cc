#include "judgeserver.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <string>
#include <iostream>
#include <thread>
#include <mutex>

#include <zmq.h>

#include "judge_worker/judgeworker.h"
#include "configure.hpp"

using namespace std;

std::mutex mutex;

struct WorkerConf
{
  void* context;
  string sock_back_addr;
  string program_dir;
  string tmp_dir;
  string log_path;
  int num;
};

struct ServerConf
{
  void* context;
  string sock_front_addr;
  string sock_back_addr;
  string log_path;
  string judge_server_prefix;
};

void startWorker(const WorkerConf& worker_conf) {
  oj::JudgeWorker judgeWorker(worker_conf.sock_back_addr,
                              worker_conf.context);
  judgeWorker.setWorkerNum(worker_conf.num);
  judgeWorker.setTmpDir(worker_conf.tmp_dir);
  judgeWorker.setProgramDir(worker_conf.program_dir);
  judgeWorker.setLogPath(worker_conf.log_path);

  judgeWorker.run();
}

void startServer(const ServerConf& server_conf) {
  auto judgeServer =
       oj::JudgeServer(server_conf.sock_front_addr,
                       server_conf.sock_back_addr,
                       server_conf.context);

  judgeServer.setLogPath(server_conf.log_path);
  judgeServer.run();
}

void closeStdin() {
  int fd;
  if (-1 != (fd = open("/dev/null", O_RDONLY))) {

    close(STDIN_FILENO);

    dup2(fd, STDIN_FILENO);

    close(fd);
  }
}

void closeStdout() {
  int fd;
  if (-1 != (fd = open("/dev/null", O_WRONLY))) {

    close(STDOUT_FILENO);

    dup2(fd, STDOUT_FILENO);

    close(fd);
  }
}

void daemon() {
  int pid = fork();
  if (pid < 0) {
    cout << "create daemon fail: " << utils::strErr() << endl;
    exit(errno);
  }

  if (pid) {
    cout<<"child pid: "<<pid<<endl;
    exit(0);
  }

  closeStdin();                           // move stdin to /dev/null
  closeStdout();


  setsid();
  return;
}

int main(int argc, char const *argv[])
{

  if (argc == 1) {
    cout << "usage: judgeServer configure_filepath" << endl;
    cout << "example:\n" << "./judgeServer ../judge.conf" << endl;
    return -1;
  }
  oj::Configure conf;
  if (!conf.readConfigureFrom(argv[1])) {
    cout << "open file: " << argv[1] << " fail" << endl;
    return -1;
  }

  void* context;
  if (NULL == (context = zmq_init(conf.iothreads))) {
    cout << "init context fail" << utils::strErr() << endl;
    return -1;
  }

  if (conf.daemon)
    daemon();

  ServerConf server_conf {
    context,
    conf.sock_front_addr,
    conf.sock_back_addr,
    conf.log_path,
    conf.judge_server_prefix,
  };


  WorkerConf worker_conf {
    context,
    conf.sock_back_addr,
    conf.program_dir,
    conf.tmp_dir,
    conf.log_path,
    0,
  };

  vector<thread> threads;
  threads.push_back(thread(startServer, ref(server_conf)));

  sleep(2);
  for (int num = 0; num != conf.worker_nums; ++num) {
    worker_conf.num = num;
    threads.push_back(thread(startWorker, ref(worker_conf)));
  }

  for (auto& thread: threads) {
    thread.join();
  }

  return 0;
}
