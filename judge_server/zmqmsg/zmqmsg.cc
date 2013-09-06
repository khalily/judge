#include "zmqmsg.h"
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

namespace zmqmsg
{

ZmqMsg::ZmqMsg() {
  first_more_ = 0;
  isfirst_ = true;
  zmq_msg_init(&msg_);
}

ZmqMsg::ZmqMsg(const char* strData) {
  ZmqMsg();
  zmq_msg_init_size(&msg_, strlen(strData));
  memcpy(zmq_msg_data(&msg_), strData, strlen(strData));
  strData_.assign(strData, strlen(strData));
}

ZmqMsg::ZmqMsg(const string& strData) {
  ZmqMsg();
  zmq_msg_init_size(&msg_, strData.size());
  memcpy(zmq_msg_data(&msg_), strData.c_str(), strData.size());
  strData_.assign(strData);
}

ZmqMsg::ZmqMsg(ZmqMsg& msg) {
  ZmqMsg();
  zmq_msg_init_size(&msg_, msg.size());
  memcpy(zmq_msg_data(&msg_), msg.data().c_str(), msg.size());
  strData_.assign(msg.strData_);
}

ZmqMsg::~ZmqMsg() {
  zmq_msg_close(&msg_);
}


void ZmqMsg::rebuild(const std::string& strData) {
  zmq_msg_close(&msg_);
  zmq_msg_init(&msg_);
  zmq_msg_init_size(&msg_, strData.size());
  memcpy(zmq_msg_data(&msg_), strData.c_str(), strData.size());
  strData_.assign(strData);
}

bool ZmqMsg::sendMsg(void* sock, string strData, int flag) {
  zmq_msg_init_size(&msg_, strData.size());
  memcpy(zmq_msg_data(&msg_), strData.c_str(), strData.size());
  int size = zmq_send(sock, &msg_, strData.size(),flag);
  if (size == -1)
    return false;
  return true;
}

bool ZmqMsg::sendMsg(void* sock, int flag) {
  int size = zmq_sendmsg(sock, &msg_, flag);
  if (size == -1)
    return false;
  return true;
}

bool ZmqMsg::recvMsg(void* sock, int flag) {
  int size = zmq_recvmsg(sock, &msg_, flag);
  if (size == -1)
    return false;

  strData_.assign((char*)zmq_msg_data(&msg_), zmq_msg_size(&msg_));
  return true;
}


ZmqMutiMsg::ZmqMutiMsg() {

}

ZmqMutiMsg::~ZmqMutiMsg() {
  for (auto msg: msgs_) {
    delete msg;
  }
  msgs_.clear();
}

bool ZmqMutiMsg::sendMutiMsg(void* sock) {
  auto iter = msgs_.begin();
  for (; iter != msgs_.end() - 1; ++iter) {
    if (!(*iter)->sendMsg(sock, ZMQ_SNDMORE))
      return false;
  }
  if (!(*iter)->sendMsg(sock))
    return false;
  return true;
}

bool ZmqMutiMsg::recvMutiMsg(void *sock) {
  int64_t more = 1;
  size_t more_size = sizeof(more);
  while (more) {
    ZmqMsg* msg = new ZmqMsg();
    if (!msg->recvMsg(sock, ZMQ_RCVMORE)) {
        return false;
    }
    msgs_.push_back(msg);
    zmq_getsockopt(sock, ZMQ_RCVMORE, &more, &more_size);
  }
  return true;
}

void ZmqMutiMsg::pushMsg(ZmqMsg *msg) {
  msgs_.push_back(msg);
}

ZmqMsg* ZmqMutiMsg::popMsg() {
  ZmqMsg* msg = msgs_.back();
  msgs_.pop_back();
  return msg;
}

}
