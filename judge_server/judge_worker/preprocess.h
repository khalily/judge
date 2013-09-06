#ifndef _OJ_PREPROCESS_H_
#define _OJ_PREPROCESS_H_

#include "stdint.h"
#include <string>
#include <vector>
#include "protocol.h"

namespace oj {


class PreProcess {
 public:
  PreProcess(uint32_t code_type, const RunPath& run_path) :
      code_type_(code_type),
      run_path_(run_path) {
        vec_cmd_.reserve(10);
      }

  bool Run(std::vector<const char*>& cmd_args);

 private:
  bool PP_TypeC(std::vector<const char*>& cmd_args);

  bool PP_TypeCpp(std::vector<const char*>& cmd_args);

  bool PP_TypePython(std::vector<const char*>& cmd_args);

  bool PP_TypeJava(std::vector<const char*>& cmd_args);

  // uint8_t code_type_;
  // const std::string& work_dir_path_;
  // const std::string& temp_dir_path_;
  uint32_t code_type_;
  const RunPath& run_path_;
  std::vector<char> vec_cmd_;
};

}
#endif
