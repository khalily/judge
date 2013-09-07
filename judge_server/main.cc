#include "judgeserver.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <string>
#include <iostream>

#include <zmq.h>

#include "judge_worker/judgeworker.h"
#include "thread.hpp"
#include "configure.hpp"

using namespace std;

oj::MutexLock mutex;


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

void* startWorker(void* args) {
  auto worker_conf = *((WorkerConf*)args);
  oj::JudgeWorker judgeWorker(worker_conf.sock_back_addr,
                              worker_conf.context);
  judgeWorker.setWorkerNum(worker_conf.num);
  judgeWorker.setTmpDir(worker_conf.tmp_dir);
  judgeWorker.setProgramDir(worker_conf.program_dir);
  judgeWorker.setLogPath(worker_conf.log_path);

  judgeWorker.run();
  return (void*)0;
}

void* startServer(void* args) {
  auto server_conf = *((ServerConf*)args);
  auto judgeServer =
       oj::JudgeServer(server_conf.sock_front_addr,
                       server_conf.sock_back_addr,
                       server_conf.context);

  judgeServer.setLogPath(server_conf.log_path);
  judgeServer.run();

  return (void*)0;
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

  pthread_t tid;
  pthread_create(&tid, NULL, startServer, &server_conf);

  sleep(2);
  for (int num = 0; num != conf.worker_nums; ++num) {
    worker_conf.num = num;
    pthread_t tid;
    pthread_create(&tid, NULL, startWorker, &worker_conf);
  }
  // judgeServer.run();
  pthread_join(tid, NULL);

  return 0;
}
