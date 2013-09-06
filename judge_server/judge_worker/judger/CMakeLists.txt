cmake_minimum_required(VERSION 2.8)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/judge_worker)
include_directories(${PROJECT_SOURCE_DIR}/judge_worker/judger)
set(SRC_LIST judge.cc judge_c.cc judge_cpp.cc judge_python.cc
             judger.cc judge_java.cc checker.cc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_library(judger ${SRC_LIST})
target_link_libraries(judger)
