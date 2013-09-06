#include "preprocess.h"
#include "stdio.h"
#include "stdlib.h"
#include <string>

using namespace std;

namespace oj {

bool PreProcess::Run(vector<const char*>& cmd_args) {
  switch (code_type_) {
    case kTypeC : return PP_TypeC(cmd_args);
    case kTypeCpp : return PP_TypeCpp(cmd_args);
    case kTypePython : return PP_TypePython(cmd_args);
    case kTypeJava : return PP_TypeJava(cmd_args);
    default : {
      printf("Unknown Type: %d\n", code_type_);
      break;
    }
  }
  return false;
}

bool PreProcess::PP_TypeC(vector<const char*>& cmd_args) {
  vector<char> cmds(50);
  sprintf(&cmds[0], "gcc %s/main.c -o %s/main",
          &run_path_.work_dir_path[0],
          &run_path_.work_dir_path[0]
          );
  if (system(&cmds[0]) == -1) {
    printf("system error\n");
    return false;
  }
  sprintf(&vec_cmd_[0], "%s/main",
        &run_path_.work_dir_path[0]);
  // printf("cmds: %s\n", &vec_cmd[0]);
  cmd_args.push_back(&vec_cmd_[0]);
  printf("cmd_args: %s\n", cmd_args[0]);
  return true;
}

bool PreProcess::PP_TypeCpp(vector<const char*>& cmd_args) {
  vector<char> cmds(50);
  sprintf(&cmds[0], "g++ %s/main.cpp -o %s/main",
          &run_path_.work_dir_path[0],
          &run_path_.work_dir_path[0]
          );
  if (system(&cmds[0]) == -1) {
    printf("system error\n");
    return false;
  }
  sprintf(&vec_cmd_[0], "%s/main",
        &run_path_.work_dir_path[0]);
  cmd_args.push_back(&vec_cmd_[0]);
  return true;
}

bool PreProcess::PP_TypePython(vector<const char*>& cmd_args) {
  cmd_args.push_back("python");
  sprintf(&vec_cmd_[0], "%s/main.py",
        &run_path_.work_dir_path[0]);
  cmd_args.push_back(&vec_cmd_[0]);
  return true;
}

bool PreProcess::PP_TypeJava(vector<const char*>& cmd_args) {
  vector<char> cmds(50);
  sprintf(&cmds[0], "javac %s/main.java",
          &run_path_.work_dir_path[0]
          );
  if (system(&cmds[0]) == -1) {
    printf("system error\n");
    return false;
  }
  sprintf(&vec_cmd_[0], "ls -alF %s",
        &run_path_.work_dir_path[0]);
  system(&vec_cmd_[0]);
  cmd_args.push_back("java");
  cmd_args.push_back("-cp");
  cmd_args.push_back(&run_path_.work_dir_path[0]);
  cmd_args.push_back("Sum");
  // sprintf(&vec_cmd_[0], "-cp %s",
  //       &run_path_.work_dir_path[0]);
  // cmd_args.push_back(&vec_cmd_[0]);
  return true;
}

}
