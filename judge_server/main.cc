#include "judgeserver.h"

#include <string>

#include "judge_worker/judgeworker.h"
#include "thread.hpp"

using namespace std;

const string sock_front_addr = "tcp://0.0.0.0:7878";

// const string sock_back_addr = "inproc://dealer";
const string sock_back_addr = "ipc://dealer.ipc";
const int worker_num = 1;

oj::MutexLock mutex;

void* startWorker(void* args) {
  int num = *((int*)args);
  oj::JudgeWorker judgeWorker(sock_back_addr);
  judgeWorker.setWorkerNum(num);
  judgeWorker.run();
  return (void*)0;
}

int main(int argc, char const *argv[])
{
  auto judgeServer =
       oj::JudgeServer(sock_front_addr,
                       sock_back_addr,
                       worker_num);
  int i;
  for (i = 0; i != worker_num; ++i) {
    pthread_t tid;
    pthread_create(&tid, NULL, startWorker, &i);
  }
  judgeServer.run();

  return 0;
}
