#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <zmq.hpp>
#include <jsoncpp/json.h>

#include "../judge_worker/protocol.h"

using namespace std;

const char addr_server[] = "tcp://0.0.0.0:7878";
// const char addr_storage[] = "tcp://127.0.0.1:2002";
// const char addr_sync[] = "tcp://127.0.0.1:2003";

void WrapData(const oj::RunConfigure &run_configure, zmq::message_t &send_msg) {
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
  cout << "data : " << data
       << "size : " << data.size() <<endl;

  send_msg.rebuild(data.size());
  memcpy(send_msg.data(), data.c_str(), data.size());
}

void ReadFromFile(vector<char> &data, const char *filepath) {
  ifstream input(filepath);
  input.seekg(0, ios::end);
  int length = input.tellg();
  data.resize(length);
  input.seekg(0, ios::beg);
  input.read(&data[0], length);
}

void oneRequest() {

}

int main(int argc, char *argv[])
{
  if (argc != 4) {
    cout << "usage: ./test3 CodeType filepath test_num" << endl;
    cout << "enum CodeType {\n\
                kTypeC = 0,\n\
                kTypeCpp,\n\
                kTypePython,\n\
                kTypeJava,\n\
                kTypeUnknown,\n\
              }; "<< endl;
    return -1;
  }
  oj::RunConfigure run_configure;
  vector<char> source_code;
  ReadFromFile(source_code, argv[2]);
  run_configure.source_code.assign(source_code.begin(), source_code.end());
  run_configure.time_limit = 1000;
  run_configure.memory_limit = 40000;
  run_configure.run_times = 3;
  run_configure.program_id = 1;
  // run_configure.runer = 1000;
  run_configure.code_type = atoi(argv[1]);
  run_configure.trace = true;

  zmq::context_t context(2);
  zmq::socket_t sock(context, ZMQ_REQ);
  int opv = 0;
  sock.setsockopt(ZMQ_LINGER, &opv, sizeof(int));
  sock.connect(addr_server);


  for (int i = 0; i < atoi(argv[3]); ++i) {
    zmq::message_t send_msg;
    run_configure.run_id = i;
    WrapData(run_configure, send_msg);
    if (!sock.send(send_msg)) {
      cout << "send error" <<endl;
      return -1;
    }
    zmq::message_t recv_msg;
    if (!sock.recv(&recv_msg)) {
      cout << "send error" <<endl;
      return -1;
    }
    string re;
    // re.assign((char*)recv_msg.data(), recv_msg.size());
    cout << "recv_msg: " << string((char*)recv_msg.data(), recv_msg.size()) <<endl;
  }
  // sleep(10);

  return 0;
}
