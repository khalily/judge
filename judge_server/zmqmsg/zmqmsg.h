#ifndef _OJ_ZMQMSG_H_
#define _OJ_ZMQMSG_H_
#include <zmq.h>
#include <string>
#include <vector>
// #include <cstdint>
#include <stdint.h>
#include <iostream>

namespace zmqmsg
{

inline const char* strErr() {
    return zmq_strerror(zmq_errno());
}

class ZmqMsg
{
 public:
  ZmqMsg();
  ZmqMsg(const char* strData);
  ZmqMsg(const std::string& strData);
  ZmqMsg(ZmqMsg& msg);
  ~ZmqMsg();
  void rebuild(const std::string& strData);
  bool sendMsg(void* sock, std::string strData, int flag = 0);
  bool sendMsg(void* sock, int flag = 0);
  bool recvMsg(void* sock, int flag = 0);
  size_t size() const {
    return data().size();
  }

  std::string data() const {
    return strData_;
  }

 private:
  bool isfirst_;
  int64_t first_more_;
  zmq_msg_t msg_;
  std::string strData_;
};

class ZmqMutiMsg
{
 public:
  ZmqMutiMsg();
  ~ZmqMutiMsg();
  bool sendMutiMsg(void* sock);
  bool recvMutiMsg(void* sock);
  void pushMsg(ZmqMsg* msg);
  ZmqMsg* popMsg();
  std::string data() const {
    return (*msgs_.rbegin())->data();
  }
  int size() const {
    return msgs_.size();
  }

 private:
  std::vector<ZmqMsg*> msgs_;
};

}
#endif
