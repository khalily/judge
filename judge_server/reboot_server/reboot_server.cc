#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "../zmqmsg/zmqmsg.h"
#include "../utils.hpp"
#include "../log.hpp"
#include "../judge_worker/protocol.h"

#include "jsoncpp/json.h"

using namespace std;


namespace oj {

class RebootServer
{
 public:
  RebootServer(const char* sock_addr, const char* sock_server_addr) :
      sock_addr_(sock_addr),
      sock_server_addr_(sock_server_addr),
      context_(NULL),
      sock_(NULL),
      sock_server_(NULL) { }

  ~RebootServer() {
    if (context_) {
      if (sock_)
        zmq_close(sock_);
      if (sock_server_)
        zmq_close(sock_server_);
      zmq_term(context_);
    }
  }

  void run();


 private:
  bool init();
  void wrapData(const RunConfigure &run_configure, zmqmsg::ZmqMsg &smsg);
  void parseData(Results &results, const zmqmsg::ZmqMsg &rmsg);
  void checkServer();
  void readFromFile(vector<char> &data, const char *filepath);
  void execCmd(const char* cmd);
  const std::string sock_addr_;
  const std::string sock_server_addr_;
  void* context_;
  void* sock_;
  void* sock_server_;

};

void RebootServer::execCmd(const char* cmd) {
  system(cmd);
}

void RebootServer::readFromFile(vector<char> &data, const char *filepath) {
  ifstream input(filepath);
  input.seekg(0, ios::end);
  int length = input.tellg();
  data.resize(length);
  input.seekg(0, ios::beg);
  input.read(&data[0], length);
}

bool RebootServer::init() {
  if (NULL == (context_ = zmq_init(1))) {
    cout << "init context fail" << utils::strErr() << endl;
    return false;
  }

  if (NULL == (sock_ = zmq_socket(context_, ZMQ_REP))) {
    cout << "create socket error" << utils::strErr() << endl;
    return false;
  }

  if (NULL == (sock_server_ = zmq_socket(context_, ZMQ_REQ))) {
    cout << "create socket error" << utils::strErr() << endl;
    return false;
  }

  return true;
}

void RebootServer::run() {
  if (!init())
    return;

  zmq_pollitem_t items[] = {
    {sock_, 0, ZMQ_POLLIN, 0}
  };

  const int64_t kTimeOut = 3600000000;   // 1 * 3600 * 1000 * 1000 ms    <==>   1 hour

  while (true) {
    int re = zmq_poll(items, 1, kTimeOut);

    assert(re >= 0);

    if (re > 0) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmqmsg::ZmqMsg msg;
        if (!msg.recvMsg(sock_)) {
          cout << "recvMsg error" << utils::strErr() << endl;
        }

        execCmd("/acm/reboot-server.sh");

        if (!msg.sendMsg(sock_)) {
          cout << "sendMsg error" << utils::strErr() << endl;
        }
    } else {
      checkServer();
    }

    }

  }
}

void RebootServer::wrapData(const RunConfigure &run_configure, zmqmsg::ZmqMsg &smsg) {
  Json::FastWriter fast_writer;
  Json::Value root;
  root["time_limit"] = run_configure.time_limit;
  root["memory_limit"] = run_configure.memory_limit;
  root["run_times"] = run_configure.run_times;
  root["code_type"] = run_configure.code_type;
  root["run_id"] = run_configure.run_id;
  root["source_code"] = run_configure.source_code;
  root["program_id"] = run_configure.program_id;
  root["trace"] = run_configure.trace;
  string data = fast_writer.write(root);

  smsg.rebuild(data);
}

void RebootServer::parseData(Results &results, const zmqmsg::ZmqMsg &rmsg) {
  Json::Reader reader;
  Json::Value root;

  reader.parse(rmsg.data(), root);

  results.compile_error = root.get("compile_error", "").asString();
  results.judge_result = JudgeResult(root.get("judge_result", kUnknown).asInt());
  // results.memory_used
  // results.run_id
  // results.time_used
  // results.signum_result
}

void RebootServer::checkServer() {
  RunConfigure run_configure;
  vector<char> source_code;
  readFromFile(source_code, "./a+b.c");
  run_configure.source_code.assign(source_code.begin(), source_code.end());
  run_configure.time_limit = 1000;
  run_configure.memory_limit = 40000;
  run_configure.run_times = 3;
  run_configure.program_id = 1;
  // run_configure.runer = 1000;
  run_configure.code_type = 0; // gcc
  run_configure.trace = true;
  run_configure.run_id = 0;

  zmqmsg::ZmqMsg smsg;
  wrapData(run_configure, smsg);

  if (!smsg.sendMsg(sock_server_)) {
    cout << "sendMsg error" << utils::strErr() << endl;
  }

  zmqmsg::ZmqMsg rmsg;
  if (!rmsg.recvMsg(sock_server_)) {
    cout << "recvMsg error" << utils::strErr() << endl;
  }

  Results results;
  parseData(results, rmsg);

  if (results.judge_result != kAC) {
    execCmd("/acm/reboot-server.sh");
  }
}

}

const char* sock_addr = "tcp://0.0.0.0:9292";
const char* sock_server_addr = "tcp://0.0.0.0:7878";

int main(int argc, char const *argv[])
{
  oj::RebootServer rebootServer(sock_addr, sock_server_addr);
  rebootServer.run();
  return 0;
}

