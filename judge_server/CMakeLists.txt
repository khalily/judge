cmake_minimum_required(VERSION 2.8)
project(Judge)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${CMAKE_SOURCE_DIR})

message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/judge_worker)
add_subdirectory(${PROJECT_SOURCE_DIR}/zmqmsg)
#add_subdirectory(${PROJECT_SOURCE_DIR}/test)
add_subdirectory(${PROJECT_SOURCE_DIR}/judge_worker/judger)
add_executable(judgeServer main.cc judgeserver.cc)
target_link_libraries(judgeServer pthread zmqmsg judge_worker)
