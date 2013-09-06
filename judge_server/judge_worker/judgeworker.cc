#include "judgeworker.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream>             // test
#include <fstream>
#include <vector>
#include <zmq.h>
#include <jsoncpp/json.h>

#include "../zmqmsg/zmqmsg.h"
#include "protocol.h"
// #include "judge.h"
// #include "preprocess.h"
// #include "checker.h"
#include "judger/judger.h"
#include "judger/judgeoftype.hpp"

using namespace std;

extern oj::MutexLock mutex;

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

JudgeWorker::JudgeWorker(const std::string& sock_back_addr) :
    sock_back_addr_(sock_back_addr),
    context_(NULL),
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
  // InitCallLimit();
  // SyncToServer();

  for (; ;) {
    zmqmsg::ZmqMsg rmsg;
    if (!rmsg.recvMsg(sock_worker_)) {
      log_ << "[error] recv from client fail: " << zmqmsg::strErr() << log_.endl();
    }
    // cout << "worker recv:" << rmsg.data() <<endl;
    Results results;
    if (!processTask(rmsg, results)) {
      log_ << "[error] processTask fail: " << utils::strErr() << log_.endl();
    }

    log_ << "processTask finish: " << log_.endl();

    // RunConfigure run_configure;
    // parseData(rmsg, run_configure);
    // PrintRunconfigure(run_configure);

    // RunPath run_path;
    // // FileList user_output_list;
    // // FileList right_output_list;
    // // FileList input_list;
    // // buildEnv(run_configure, run_path);

    // vector<const char*> cmd_args;
    // // PProcess(run_path,
    // //          run_configure.code_type,
    // //          cmd_args);
    // PreProcess pre_process(run_configure.code_type, run_path);
    // if (!pre_process.Run(cmd_args)) {
    //   printError("preprocess error");
    // }
    // cmd_args.push_back(NULL);

    // // run judge
    // Results results;
    // for (uint32_t times = 0;
    //      times != run_configure.run_times;
    //      ++times) {
    //   // if (judgeOnce(run_configure,
    //   //               cmd_args,
    //   //               run_path,
    //   //               results,
    //   //               // user_output_list,
    //   //               right_output_list,
    //   //               input_list,
    //   //               times) == kError) {
    //   //   break;
    //   // }
    // }
    // results.run_id = run_configure.run_id;
    printResults(results);

    zmqmsg::ZmqMsg smsg;
    wrapData(results, smsg);
    if (!smsg.sendMsg(sock_worker_)) {
      log_ << "[error] send to client fail: " << zmqmsg::strErr() << log_.endl();
    }
    // zmq::message_t send_msg;
    // WrapData(results, send_msg);
    // if (!sock_storage_.send(send_msg)) {
    //   printf("send error\n");
    //   return false;
    // }
  }
}

bool JudgeWorker::processTask(const zmqmsg::ZmqMsg& msg,
                              Results& results) {
  RunConfigure run_configure;
  parseData(msg, run_configure);

  vector<char> work_dir_path(15);
  if (!buildEnv(run_configure.run_id, work_dir_path)) {
    log_ << "[error] buildEnv fail" << log_.endl();
    return false;
  }
  log_ << "buildEnv " << &work_dir_path[0] << " sucessful" << log_.endl();

  JudgeofType judgeoftype;
  Judger *judger =
        judgeoftype.constructJudger(run_configure.code_type,
                                    log_,
                                    run_configure.run_id);
  judger->storeSourceFile(run_configure.source_code,
                          &work_dir_path[0]);

  log_ << "storeSourceFile sucessful" << log_.endl();
  if (!judger->compile()) {
    results = judger->getResults();
    log_ << "compile error" << log_.endl();
    return true;
  }
  // log_ << "why?" << log_.endl();
  ExecuteCondtion execute_condtion {
    run_configure.time_limit,
    run_configure.memory_limit,
    run_configure.runer,
    run_configure.trace
  };
  log_ << "start execute ..." << log_.endl();
  IOFileno ioFileno;
  for (int num = 0; num != run_configure.run_times; ++num) {
    getFileno(ioFileno,
              run_configure.program_id,
              run_configure.run_id,
              num);
    if (!judger->execute(execute_condtion,
                         ioFileno)) {
      log_ << "error start running ... " << log_.endl();
      results = judger->getResults();
      printResults(results);
      results.run_id = run_configure.run_id;
      log_ << "run_id" << results.run_id << log_.endl();
      file_manager_.closeAll();
      return true;
    }
    log_ << "execute one times ..." << log_.endl();

    // log_ << "file_manager_: " << file_manager_.size() << log_.endl();
    file_manager_.closeAll();
  }
  results = judger->getResults();
  results.run_id = run_configure.run_id;
  printResults(results);
  cleanEnv(&work_dir_path[0]);
  log_ << "cleanEnv " << &work_dir_path[0] << " sucessful" << log_.endl();
  return true;
}


bool JudgeWorker::init() {
  vector<char> prefix(20);
  sprintf(&prefix[0], "JudgeWorker %d", worker_number_);
  log_.setPrefix(&prefix[0]);
  log_.setMutex(&mutex);

  if (!log_.setLog(std::cout)) {
    log_ << "[error] setLog fail: " << utils::strErr() << log_.endl();
    return false;
  }

  if (NULL == (context_ = zmq_init(1))) {
    log_ << "[error] create context fail: " << zmqmsg::strErr() << log_.endl();
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

// void JudgeWorker::InitCallLimit() {
//   // vector<char> data;
//   // ReadFromFile(data);
//   c_call_limit_.swap(c_call_limit);

//   cpp_call_limit_.swap(cpp_call_limit);

//   py_call_limit_.swap(py_call_limit);

//   java_call_limit_.swap(java_call_limit);

// }

// void JudgeWorker::ReadFromFile(vector<char> &data) {
  // ifstream input(call_limit_file_.c_str());
  // input.seekg(0, ios::end);
  // int length = input.tellg();
  // data.resize(length);
  // input.seekg(0, ios::beg);
  // input.read(&data[0], length);
// }

// ReturnCode JudgeWorker::judgeOnce(const RunConfigure &run_configure,
//                                   const vector<const char*> &cmd_args,
//                                   RunPath &run_path,
//                                   Results &results,
//                                   FileList &user_output_list,
//                                   FileList &right_output_list,
//                                   FileList &input_list,
//                                   uint32_t times) {
//   getFileno(run_configure,
//             run_path,
//             user_output_list,
//             right_output_list,
//             input_list,
//             times);

//   Judge judge(run_configure, run_path, cmd_args);
//   switch(judge.Run()) {
//     case kOk: {
//       results = judge.get_results();
//       Checker checker(run_path.user_output_fileno,
//                       run_path.right_output_fileno);
//       if (checker.Run() == kError) {
//         results.judge_result = checker.get_result();
//         return kError;
//       }
//       return kOk;
//     }
//     case kError: {
//       results = judge.get_results();
//       return kError;
//     }
//     case kUnknown: {
//       printError("judge return kUnknown");
//     }
//   }
//   return kUnknown;
// }

// void JudgeWorker::preProcess(const RunPath& run_path,
//                            uint32_t code_type,
//                            vector<const char*> &cmd_args) {

// }

// vector<const char*> cmd_args;

// void JudgeWorker::writeToFile(const string &source_code,
//                               const char *source_code_path) {
//   ofstream output(source_code_path);
//   output.write(source_code.c_str(), source_code.size());
// }

// void JudgeWorker::storeSourceCode(const std::string& source_code,
//                                   const char *work_dir_path,
//                                   uint32_t code_type) {
//   vector<char> filepath(100);
//   switch(code_type) {
//     case kTypeC: {
//       sprintf(&filepath[0], "%s/main.c", work_dir_path);
//       break;
//     }
//     case kTypeCpp: {
//       sprintf(&filepath[0], "%s/main.cpp", work_dir_path);
//       break;
//     }
//     case kTypePython: {
//       sprintf(&filepath[0], "%s/main.py", work_dir_path);
//       break;
//     }
//     case kTypeJava: {
//       sprintf(&filepath[0], "%s/main.java", work_dir_path);
//       break;
//     }
//     default: {
//       printError("error code_type");
//       break;
//     }
//   }
//   writeToFile(source_code, &filepath[0]);
// }

bool JudgeWorker::buildEnv(uint32_t run_id,
                           vector<char>& work_dir_path) {
  if (mkdir(temp_dir_.c_str(), 0777) == -1) {
    if (errno != EEXIST) {
      log_ << "[error] mkdir " << temp_dir_ << " fail: "<< utils::strErr() << log_.endl();
      return false;
    }
  }


  // work_dir_path
  sprintf(&work_dir_path[0],
          "%s/%d",
          temp_dir_.c_str(),
          run_id);
  while (mkdir(&work_dir_path[0], 0777) == -1) {
    if (errno != EEXIST) {
      log_ << "[error] mkdir work_dir_path " << &work_dir_path[0] << " fail: " << utils::strErr() << log_.endl();
      return false;
    }
    removeDir(&work_dir_path[0]);
  }

  return true;
  // storeSourceCode(run_configure.source_code,
  //                 &run_path.work_dir_path[0],
  //                 run_configure.code_type);

  // // input_list
  // run_path.input_path = new vector<char>[run_configure.run_times];
  // run_path.input_fileno = new int[run_configure.run_times];
  // for (uint32_t times = 0; times != run_configure.run_times; ++times) {
  //   run_path.input_path[times].reserve(100);
  //   sprintf(&run_path.input_path[times][0],
  //           "/oj/program/%d/%d.in",
  //           run_configure.program_id,
  //           times);
  //   // printf("input_path[%d]: %s\n", times, &run_path.input_path[times][0]);
  //   input_list.push_back(File::OpenFile(&run_path.input_path[times][0],
  //                                       O_RDONLY));
  //   run_path.input_fileno[times] = input_list[times]->fileno();
  // }

  // // user_output_list
  // run_path.user_output_path = new vector<char>[run_configure.run_times];
  // run_path.user_output_fileno = new int[run_configure.run_times];
  // for (uint32_t times = 0; times != run_configure.run_times; ++times) {
  //   run_path.user_output_path[times].reserve(100);
  //   sprintf(&run_path.user_output_path[times][0],
  //           "/tmp/oj/%d/%d.out",
  //           run_configure.run_id,
  //           times);
  //   user_output_list.push_back(File::CreateFile(&run_path.user_output_path[times][0],
  //                                          O_RDWR, 0666));
  //   run_path.user_output_fileno[times] = user_output_list[times]->fileno();
  // }

  // // right_output_list
  // run_path.right_output_path = new vector<char>[run_configure.run_times];
  // run_path.right_output_fileno = new int[run_configure.run_times];
  // for (uint32_t times = 0; times != run_configure.run_times; ++times) {
  //   run_path.right_output_path[times].reserve(100);
  //   sprintf(&run_path.right_output_path[times][0],
  //           "/oj/program/%d/%d.out",
  //           run_configure.program_id,
  //           times);
  //   right_output_list.push_back(File::OpenFile(&run_path.right_output_path[times][0],
  //                                              O_RDONLY));
  //   run_path.right_output_fileno[times] = right_output_list[times]->fileno();
  // }
}

void JudgeWorker::getFileno(IOFileno& ioFileno,
                            uint32_t program_id,
                            uint32_t run_id,
                            uint32_t numofExecute) {
  vector<char> file_path(50);
  sprintf(&file_path[0], "%s/%d/%d.in",
          program_dir_.c_str(),
          program_id,
          numofExecute);
  log_ << &file_path[0] << log_.endl();
  auto right_input_file = File::OpenFile(&file_path[0], O_RDONLY);
  ioFileno.right_input_fileno = right_input_file->fileno();


  sprintf(&file_path[0], "%s/%d/%d.out",
          program_dir_.c_str(),
          program_id,
          numofExecute);
  log_ << &file_path[0] << log_.endl();
  auto right_output_file = File::OpenFile(&file_path[0], O_RDONLY);
  ioFileno.right_output_fileno = right_output_file->fileno();

  sprintf(&file_path[0], "%s/%d/%d.out",
          temp_dir_.c_str(),
          run_id,
          numofExecute);
  log_ << &file_path[0] << log_.endl();
  auto user_output_file = File::CreateFile(&file_path[0], O_RDWR, 0666);
  ioFileno.user_output_fileno = user_output_file->fileno();

  file_manager_.push_back(right_input_file);
  file_manager_.push_back(right_output_file);
  file_manager_.push_back(user_output_file);
}
// void JudgeWorker::getFileno(
//                             uint32_t times) {
//   //input_list
//   sprintf(&run_path.input_path[0],
//           "/oj/program/%d/%d.in",
//           run_configure.program_id,
//           times);
//   printf("input_path[%d]: %s\n", times, &run_path.input_path[0]);
//   input_list.push_back(File::OpenFile(&run_path.input_path[0],
//                                       O_RDONLY));
//   run_path.input_fileno = input_list[times]->fileno();


//   //right_output_list
//   sprintf(&run_path.right_output_path[0],
//           "/oj/program/%d/%d.out",
//           run_configure.program_id,
//           times);

//   printf("rigtht_output_path[%d]: %s\n", times, &run_path.right_output_path[0]);
//   right_output_list.push_back(File::OpenFile(&run_path.right_output_path[0],
//                                              O_RDONLY));
//   run_path.right_output_fileno = right_output_list[times]->fileno();

//   //user_output_list
//   sprintf(&run_path.user_output_path[0],
//           "/tmp/oj/%d/%d.out",
//           run_configure.run_id,
//           times);
//   printf("user_output_path[%d]: %s\n", times, &run_path.user_output_path[0]);
//   user_output_list.push_back(File::CreateFile(&run_path.user_output_path[0],
//                                               O_RDWR, 0666));
//   run_path.user_output_fileno = user_output_list[times]->fileno();
// }

void JudgeWorker::parseData(const zmqmsg::ZmqMsg &rmsg,
                            RunConfigure& run_configure) {
  Json::Reader reader;
  Json::Value root;
  // string data(static_cast<const char*>(rmsg.data()), recv_msg.size());
  // cout << "data: " << data
  //      << "size: " << data.size() << endl;

  // cout << "rmsg.data: " << rmsg.data() <<endl;
  reader.parse(rmsg.data(), root);
  // cout << "parse ok" <<endl;
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
  string data = fast_writer.write(root);
  // send_msg.rebuild((void*)(data.c_str()), data.size() + 1, NULL);
  smsg.rebuild(data);
}

void JudgeWorker::removeDir(const char *dir_path) {
  vector<char> subname(100);
  struct dirent *pdirent = NULL;

  DIR *pdir = opendir(dir_path);
  if (pdir) {
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
