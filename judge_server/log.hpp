#ifndef _UTILS_LOG_HPP_
#define _UTILS_LOG_HPP_

#include <fstream>
#include <functional>
#include <string.h>
#include <mutex>

extern std::mutex mutex;
namespace utils
{

static const char * strErr() {
  return strerror(errno);
}

class Log
{
 public:
  typedef std::function<const char* ()> strFunc;
 public:
  Log() :
      auto_close_(true),
      isPrefix_(false),
      isTimePrefix_(true),
      prefix_(NULL),
      out_(NULL),
      isLock_(false) { }

  ~Log() {
    if (out_ && auto_close_) {
      delete out_;
    }
  }
  bool setLog(std::ostream& out, bool auto_close = false) {

    out_ = &out;
    if (out_ == NULL)
      return false;
    auto_close_ = auto_close;
    return true;
  }
  bool setLog(const char* filepath) {

    out_ = new std::fstream(filepath, std::ios_base::in  |
                                      std::ios_base::out |
                                      std::ios_base::app);
    if (out_ == NULL)
      return false;

    return true;
  }

  // void setMutex(oj::MutexLock *mutex) {
  // void setMutex(std::mutex mutex) {
  //   mutex_ = mutex;
  // }

  void setPrefix(const char* prefix) {

    if (prefix_) delete prefix_;
    prefix_ = strdup(prefix);
    isPrefix_ = true;
  }

  static const char* timeprefix() {
    static char buf[32];
    time_t t = time(NULL);
    struct tm *tm_ = localtime(&t);
    sprintf(buf, "[%d-%02d-%02d %02d:%02d:%02d] ",
      tm_->tm_year+1900, tm_->tm_mon+1, tm_->tm_mday,
      tm_->tm_hour, tm_->tm_min, tm_->tm_sec);
    return buf;
  }

  bool isLock() {
    return isLock_;
  }

  void lockofLog() {
    if (!isLock()) {
      mutex.lock();
      isLock_ = true;
    }
  }

  void unLockofLog() {
    if (isLock()) {
      isLock_ = false;
      mutex.unlock();
    }
  }

  strFunc endl() {
    if (prefix_)
      isPrefix_ = true;
    isTimePrefix_ = true;
    return &Log::endFunc;
  }

  static const char* endFunc() {
  #if (defined (__WINDOWS__))
    return "\r\n";
  #else
    return "\n";
  #endif
  }

  friend Log& operator << (Log& log, strFunc func) {
    if (log.out_) {
      (*log.out_) << func();
      (*log.out_).flush();
    }

    log.unLockofLog();
    return log;
  }

  template<typename T>
  friend Log& operator << (Log& log, T data) {
    log.lockofLog();
    if (log.out_) {
      if (log.isTimePrefix_) {
        (*log.out_) << timeprefix();
        log.isTimePrefix_ = false;
      }
      if (log.isPrefix_) {
        (*log.out_) << "[" << log.prefix_ << "] ";
        log.isPrefix_ = false;
      }
      (*log.out_)<< data;
    }
    return log;
  }

 private:
  bool auto_close_;
  bool isPrefix_;
  bool isTimePrefix_;
  char * prefix_;
  std::ostream *out_;
  // oj::MutexLock *mutex_;
  // std::mutex mutex_;
  bool isLock_;
};

}

#endif
