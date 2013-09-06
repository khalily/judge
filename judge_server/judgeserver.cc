#include "judgeserver.h"

#include <functional>
#include <zmq.h>

#include "zmqmsg/zmqmsg.h"
#include "judge_worker/judgeworker.h"

using namespace std;

extern oj::MutexLock mutex;

namespace oj {

JudgeServer::JudgeServer(const string& sock_front_addr,
                         const string& sock_back_addr,
                         int worker_num) :
    sock_front_addr_(sock_front_addr),
    sock_back_addr_(sock_back_addr),
    worker_num_(worker_num),
    context_(NULL),
    sock_front_(NULL),
    sock_back_(NULL) {

    }

JudgeServer::~JudgeServer() {
  for (auto worker: workers_) {
    delete worker;
  }
  destroy();
}

bool JudgeServer::init() {
  log_.setPrefix("JudgeServer");
  log_.setMutex(&mutex);

  if (!log_.setLog(std::cout)) {
    log_ << "[error] setLog fail: " << utils::strErr() << log_.endl();
    return false;
  }

  if (NULL == (context_ = zmq_init(1))) {
    log_ << "[error] create context fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }
  if (NULL == (sock_front_ = zmq_socket(context_, ZMQ_ROUTER))) {
    log_ << "[error] create front socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }

  if (0 != zmq_bind(sock_front_, sock_front_addr_.c_str())) {
    log_ << "[error] bind front socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }

  if (NULL == (sock_back_ = zmq_socket(context_, ZMQ_DEALER))) {
    log_ << "[error] create back socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }
  if (0 != zmq_bind(sock_back_, sock_back_addr_.c_str())) {
    log_ << "[error] bind back socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }
  return true;
}

void JudgeServer::destroy() {
  if (context_) {
    if (sock_front_)
      zmq_close(sock_front_);
    if (sock_back_)
      zmq_close(sock_back_);
    zmq_term(context_);
  }
}

void JudgeServer::run() {
  if (!init()) {
    log_ << "[error] init fail: " <<log_.endl();
    return;
  }
  // startWorker();

  zmq_pollitem_t items[] = {
    {sock_front_, 0, ZMQ_POLLIN, 0},
    {sock_back_, 0, ZMQ_POLLIN, 0}
  };

  log_ << "JudgeServer running ..." << log_.endl();
  while (true) {
    zmq_poll(items, 2, -1);

    if (items[0].revents & ZMQ_POLLIN) {
      zmqmsg::ZmqMutiMsg msgs;
      if (!msgs.recvMutiMsg(sock_front_)) {
        log_ << "[error] recv from front fail: " << zmqmsg::strErr() << log_.endl();
      }
      // cout << "front recv: " << msgs.size() << " " << msgs.data() <<endl;
      if (!msgs.sendMutiMsg(sock_back_)) {
        log_ << "[error] send to back fail: " << zmqmsg::strErr() << log_.endl();
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      zmqmsg::ZmqMutiMsg msgs;
      if (!msgs.recvMutiMsg(sock_back_)) {
        log_ << "[error] recv from back fail: " << zmqmsg::strErr() << log_.endl();
      }
      // cout << "back recv: " << msgs.data() <<endl;
      if (!msgs.sendMutiMsg(sock_front_)) {
        log_ << "[error] send to front fail: " << zmqmsg::strErr() << log_.endl();
      }
    }
  }
}

}
