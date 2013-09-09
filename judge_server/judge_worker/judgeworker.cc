#include "judgeworker.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream>             // test
#include <fstream>
#include <mutex>
#include <vector>

#include <boost/format.hpp>

#include <zmq.h>
#include <jsoncpp/json.h>

#include "../zmqmsg/zmqmsg.h"
#include "protocol.h"

#include "judger/judger.h"
#include "judger/judgeoftype.hpp"

using namespace std;

extern std::mutex mutex;

namespace oj {


void printRunconfigure(const RunConfigure& run_configure) {
  // cout << "work_dir_path: \t\t"  << run_configure.work_dir_path   << endl
  cout << "memory_limit: \t\t"   << run_configure.memory_limit    << endl
       << "time_limit: \t\t"     << run_configure.time_limit      << endl
       << "code_type: \t\t"      << run_configure.code_type       << endl
       << "run_id: \t\t"         << run_configure.run_id          << endl;
}

void printResults(const Results& results) {
  cout << "judge_result: \t\t"  << results.judge_result   << endl
       << "memory_used: \t\t"   << results.memory_used    << endl
       << "time_used: \t\t"     << results.time_used      << endl
       << "signum_result: \t\t" << results.signum_result  << endl
       << "run_id: \t\t"        << results.run_id         << endl;
}

JudgeWorker::JudgeWorker(const std::string& sock_back_addr,
                         void* context) :
    sock_back_addr_(sock_back_addr),
    context_(context),
    sock_worker_(NULL),
    worker_number_(0) {
      temp_dir_ = "/tmp/oj";
      program_dir_ = "/oj/program";
}

void JudgeWorker::destroy() {
  if (context_) {
    if (sock_worker_)
      zmq_close(sock_worker_);
    zmq_term(context_);
  }
}

void JudgeWorker::run() {
  if (!init()) {
    log_ << "[error] init fail" << log_.endl();
    log_ << "[exit] JudgeWorker exit ..." << log_.endl();
    return;
  }

  log_ << "start judge worker " << worker_number_ << " ... " << log_.endl();
  for (; ;) {
    zmqmsg::ZmqMsg rmsg;
    if (!rmsg.recvMsg(sock_worker_)) {
      log_ << "[error] recv from client fail: " << zmqmsg::strErr() << log_.endl();
    }

    Results results;
    if (!processTask(rmsg, results)) {
      log_ << "[error] processTask fail: " << utils::strErr() << log_.endl();
    }

    log_ << "processTask finish: " << log_.endl();

    // printResults(results);

    zmqmsg::ZmqMsg smsg;
    wrapData(results, smsg);
    if (!smsg.sendMsg(sock_worker_)) {
      log_ << "[error] send to client fail: " << zmqmsg::strErr() << log_.endl();
    }

  }
}

bool JudgeWorker::processTask(const zmqmsg::ZmqMsg& msg,
                              Results& results) {
  RunConfigure run_configure;
  parseData(msg, run_configure);

  string work_dir_path;
  if (!buildEnv(run_configure.run_id, work_dir_path)) {
    log_ << "[error] buildEnv fail" << log_.endl();
    return false;
  }
  log_ << "buildEnv " << work_dir_path << " sucessful" << log_.endl();

  ExecuteCondtion execute_condtion {
    run_configure.time_limit,
    run_configure.memory_limit,
    run_configure.runer,
    run_configure.trace,
    0                          // memory_add
  };

  JudgeofType judgeoftype;
  Judger* judger;
  if (NULL == (judger = judgeoftype.constructJudger(
                                    run_configure.code_type,
                                    log_,
                                    execute_condtion))) {
    log_ << "constructJudger fail" << log_.endl();
    return false;
  }
  judger->storeSourceFile(run_configure.source_code,
                          work_dir_path.c_str());

  log_ << "storeSourceFile sucessful" << log_.endl();
  if (!judger->compile()) {
    results = judger->getResults();
    cleanEnv(work_dir_path.c_str());
    results.run_id = run_configure.run_id;
    log_ << "compile error" << log_.endl();
    return true;
  }

  log_ << "start execute ..." << log_.endl();
  IOFileno ioFileno;
  for (int num = 0; num != run_configure.run_times; ++num) {
    getFileno(ioFileno,
              run_configure.program_id,
              run_configure.run_id,
              num);
    if (!judger->execute(ioFileno)) {
      // log_ << "error start running ... " << log_.endl();

      break;
    }
    log_ << "execute one times ..." << log_.endl();

    file_manager_.closeAll();
  }
  results = judger->getResults();
  results.run_id = run_configure.run_id;
  // printResults(results);
  cleanEnv(work_dir_path.c_str());
  log_ << "cleanEnv " << work_dir_path << " sucessful" << log_.endl();
  return true;
}


bool JudgeWorker::init() {
  vector<char> prefix(20);
  sprintf(&prefix[0], "JudgeWorker %d", worker_number_);
  log_.setPrefix(&prefix[0]);

  if (!log_.setLog(log_path_.c_str())) {
    log_ << "[error] setLog fail: " << utils::strErr() << log_.endl();
    return false;
  }

  if (NULL == (sock_worker_ = zmq_socket(context_, ZMQ_REP))) {
    log_ << "[error] create worker socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }
  if (0 != zmq_connect(sock_worker_, sock_back_addr_.c_str())) {
    log_ << "[error] connect back socket fail: " << zmqmsg::strErr() << log_.endl();
    return false;
  }
  // sock_server_.connect(server_addr_.c_str());
  // sock_server_.setsockopt(ZMQ_LINGER, &value, sizeof(value));

  // sock_storage_.connect(storage_addr_.c_str());
  // sock_storage_.setsockopt(ZMQ_LINGER, &value, sizeof(value));
  return true;
}

bool JudgeWorker::buildEnv(uint32_t run_id,
                           string& work_dir_path) {
  if (mkdir(temp_dir_.c_str(), 0777) == -1) {
    if (errno != EEXIST) {
      log_ << "[error] mkdir " << temp_dir_ << " fail: "<< utils::strErr() << log_.endl();
      return false;
    }
  }


  // work_dir_path

  work_dir_path = (boost::format("%s/%d") %
                   temp_dir_.c_str() %
                   run_id).str();
  while (mkdir(work_dir_path.c_str(), 0777) == -1) {
    if (errno != EEXIST) {
      log_ << "[error] mkdir work_dir_path " << work_dir_path << " fail: " << utils::strErr() << log_.endl();
      return false;
    }
    removeDir(work_dir_path.c_str());
  }

  return true;
}

void JudgeWorker::getFileno(IOFileno& ioFileno,
                            uint32_t program_id,
                            uint32_t run_id,
                            uint32_t numofExecute) {
  string file_path = (boost::format("%s/%d/in/%d") %
                      program_dir_.c_str() %
                      program_id %
                      numofExecute).str();
  log_ << file_path << log_.endl();
  auto right_input_file = File::OpenFile(file_path.c_str(), O_RDONLY);
  ioFileno.right_input_fileno = right_input_file->fileno();

  file_path = (boost::format("%s/%d/out/%d") %
               program_dir_.c_str() %
               program_id %
               numofExecute).str();
  log_ << file_path << log_.endl();
  auto right_output_file = File::OpenFile(file_path.c_str(), O_RDONLY);
  ioFileno.right_output_fileno = right_output_file->fileno();

  file_path = (boost::format("%s/%d/%d.out") %
               temp_dir_.c_str() %
               run_id %
               numofExecute).str();
  log_ << file_path << log_.endl();
  auto user_output_file = File::CreateFile(file_path.c_str(), O_RDWR, 0666);
  ioFileno.user_output_fileno = user_output_file->fileno();

  file_manager_.push_back(right_input_file);
  file_manager_.push_back(right_output_file);
  file_manager_.push_back(user_output_file);
}

void JudgeWorker::parseData(const zmqmsg::ZmqMsg &rmsg,
                            RunConfigure& run_configure) {
  Json::Reader reader;
  Json::Value root;

  reader.parse(rmsg.data(), root);

  run_configure.run_id = root.get("run_id", 1).asUInt();
  cout << run_configure.run_id <<endl;
  run_configure.time_limit = root.get("time_limit", 0).asUInt();
  run_configure.memory_limit = root.get("memory_limit", 0).asUInt();
  run_configure.code_type = root.get("code_type", kTypeUnknown).asUInt();
  run_configure.trace = root.get("trace", true).asBool();
  run_configure.run_times = root.get("run_times", 0).asUInt();
  run_configure.program_id = root.get("program_id", 0).asUInt();
  run_configure.runer = root.get("runner", -1).asInt();
  run_configure.source_code = root.get("source_code", "").asString();
}

void JudgeWorker::wrapData(const Results &results, zmqmsg::ZmqMsg &smsg) {
  Json::FastWriter fast_writer;
  Json::Value root;
  root["time_used"] = results.time_used;
  root["memory_used"] = results.memory_used;
  root["judge_result"] = judgeStringResult[results.judge_result];
  root["signum_result"] = results.signum_result;
  root["run_id"] = results.run_id;
  root["compile_error"] = results.compile_error;
  string data = fast_writer.write(root);

  smsg.rebuild(data);
}

void JudgeWorker::removeDir(const char *dir_path) {
  vector<char> subname(100);
  struct dirent *pdirent = NULL;

  DIR *pdir = opendir(dir_path);
  if (pdir) {
    // printf("why?\n");
    while ((pdirent = readdir(pdir)) != NULL) {
      if (pdirent->d_type == 8) {       // 8 dir
        sprintf(&subname[0], "%s/%s", dir_path, pdirent->d_name);
        remove(&subname[0]);
      }

      if (pdirent->d_type == 4 &&       // 4 file
        strcmp(pdirent->d_name, ".") != 0 &&
        strcmp(pdirent->d_name, "..") != 0) {
        sprintf(&subname[0], "%s/%s", dir_path, pdirent->d_name);
        remove(&subname[0]);
      }
    }
    closedir(pdir);
    remove(dir_path);
  }
  else {
    printError("remove dir error");
  }
}

}
