#ifndef _OJ_CONFIGURE_HPP_
#define _OJ_CONFIGURE_HPP_

#include <string>

#include <fstream>

#include <jsoncpp/json.h>

namespace oj
{

struct Configure
{
 public:
  Configure() {
    // readFromFile(filepath.c_str(), data);
    // parseData();
  }
  ~Configure() {

  }

  bool readConfigureFrom(const std::string& filepath) {
    if (!readFromFile(filepath.c_str(), data)) {
      return false;
    }
    parseData();
    return true;
  }

 private:
  void parseData() {
    Json::Reader reader;
    Json::Value root;

    reader.parse(&data[0], root);
    iothreads = root.get("iothreads", 1).asInt();
    worker_nums = root.get("worker_nums", 4).asInt();
    daemon = root.get("daemon", false).asInt();
    sock_front_addr = root.get("sock_front_addr",
                      "tcp://0.0.0.0:7878").asString();
    sock_back_addr = root.get("sock_back_addr",
                      "ipc://dealer.ipc").asString();
    tmp_dir = root.get("tmp_dir", "/tmp/oj").asString();
    program_dir = root.get("program_dir",
                  "/oj/program").asString();
    log_path = root.get("log_path",
               "/var/log/oj/judge.log").asString();
    judge_server_prefix = root.get("judge_server_prefix",
                          "JudgeServer").asString();
    judge_worker_prefix = root.get("judge_worker_prefix",
                          "JudgeWorker").asString();
  }

  bool readFromFile(const char *filepath, std::vector<char> &data) {
    std::ifstream input(filepath);

    input.seekg(0, std::ios::end);
    int length = input.tellg();
    data.resize(length);
    input.seekg(0, std::ios::beg);
    input.read(&data[0], length);
    return true;
  }

 public:

  int32_t iothreads;
  int32_t worker_nums;
  bool daemon;

  std::string sock_front_addr;
  std::string sock_back_addr;

  std::string tmp_dir;
  std::string program_dir;
  std::string log_path;

  std::string judge_server_prefix;
  std::string judge_worker_prefix;

  std::vector<char> data;
};

}
#endif
