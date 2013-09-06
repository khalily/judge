#ifndef _OJ_THREAD_HPP_
#define _OJ_THREAD_HPP_

#include "stdio.h"
#include "pthread.h"
#include <set>
// #include <boost/function.hpp>
#include <functional>

namespace oj {

class MutexLock {
public:
  MutexLock() :
      isLock_(false) {
    pthread_mutex_init(&mutex_, NULL);
  }
  ~MutexLock() {
    pthread_mutex_destroy(&mutex_);
  }

  pthread_mutex_t* getMutex() {
    return &mutex_;
  }
  bool isLock() {
    return isLock_;
  }
  void lock() {
    pthread_mutex_lock(&mutex_);
    isLock_ = true;
  }
  void unlock() {
    isLock_ = false;
    pthread_mutex_unlock(&mutex_);
  }

private:
  pthread_mutex_t mutex_;
  bool isLock_;
};

class MutexLockGuard {
public:
  explicit MutexLockGuard(MutexLock &mutex) :
      mutex_(mutex) {
    mutex_.lock();
  }
  ~MutexLockGuard() {
    mutex_.unlock();
  }

private:
  MutexLock &mutex_;
};

class Condition
{
public:
  explicit Condition(MutexLock &mutex) :
      mutex_(mutex) {
    pthread_cond_init(&cond_, NULL);
  }
  ~Condition() {
    pthread_cond_destroy(&cond_);
  }
  void notify() {
    pthread_cond_signal(&cond_);
  }
  void notifyall() {
    pthread_cond_broadcast(&cond_);
  }
  void wait() {
    pthread_cond_wait(&cond_, mutex_.getMutex());
  }

private:
  MutexLock &mutex_;
  pthread_cond_t cond_;
};

class ThreadPool {
 public:
  typedef std::function<void ()> ThreadFunc;

 public:
  bool Spwan(const ThreadFunc &tf) {
    func_ = tf;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, &StartThread, this)) {
      printf("create thread error\n");
      return false;
    }
    threads_.insert(thread_id);
    return true;
  }

  void JoinAll() {
    std::set<pthread_t>::iterator iter = threads_.begin();
    for (; iter != threads_.end(); ++iter) {
      pthread_join(*iter, NULL);
    }
  }

 private:
  static void *StartThread(void *object) {
    ThreadPool *thread_one = static_cast<ThreadPool *>(object);
    thread_one->func_();
    return NULL;
  }

  ThreadFunc func_;
  std::set<pthread_t> threads_;
};
}
#endif
