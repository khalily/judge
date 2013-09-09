#ifndef _OJ_CALL_LIMIT_HPP_
#define _OJ_CALL_LIMIT_HPP_

#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "../protocol.h"

namespace oj
{

typedef std::set<int> CallLimit;

static CallLimit c_call_limit{
  8,SYS_time,SYS_read, SYS_uname, SYS_write, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_munmap, SYS_mprotect, SYS_mmap, SYS_fstat, SYS_set_thread_area, 252,SYS_arch_prctl,231,0
};

static CallLimit cpp_call_limit{
  8,SYS_time,SYS_read, SYS_uname, SYS_write, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_munmap, SYS_mprotect, SYS_mmap, SYS_fstat, SYS_set_thread_area, 252,SYS_arch_prctl,231,0
};

static CallLimit python_call_limit{
  146,SYS_mremap,158,117,60,39,102,191, 257, SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_futex,SYS_getcwd,SYS_getdents,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_lstat,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_readlink,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_tid_address,SYS_stat,SYS_write,0
};

static CallLimit java_call_limit{
  61,22,6,33,8,13,16,111,110,39,79,SYS_fcntl,SYS_getdents64 , SYS_getrlimit, SYS_rt_sigprocmask, SYS_futex, SYS_read, SYS_mmap, SYS_stat, SYS_open, SYS_close, SYS_execve, SYS_access, SYS_brk, SYS_readlink, SYS_munmap, SYS_close, SYS_uname, SYS_clone, SYS_uname, SYS_mprotect, SYS_rt_sigaction, SYS_getrlimit, SYS_fstat, SYS_getuid, SYS_getgid, SYS_geteuid, SYS_getegid, SYS_set_thread_area, SYS_set_tid_address, SYS_set_robust_list, SYS_exit_group,158, 0
};

static CallLimit pascal_call_limit{
  SYS_open, SYS_set_thread_area, SYS_brk, SYS_read, SYS_uname, SYS_write, SYS_execve, SYS_ioctl, SYS_readlink, SYS_mmap, SYS_rt_sigaction, SYS_getrlimit, 252,191,158,231,SYS_close,SYS_exit_group,SYS_munmap,SYS_time,4,0
};

static CallLimit bash_call_limit{
  22,61,56,42,41,79,158,117,60,39,102,191,183,SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_dup2,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getpgrp,SYS_getpid,SYS_getppid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_stat,SYS_uname,SYS_write,14,0
};

static CallLimit perl_call_limit{
  78,158,117,60,39,102,191,SYS_access,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fcntl,SYS_fstat,SYS_futex,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getrlimit,SYS_getuid,SYS_ioctl,SYS_lseek,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_readlink,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_thread_area,SYS_set_tid_address,SYS_stat,SYS_time,SYS_uname,SYS_write,0
};

static CallLimit ruby_call_limit{
  6,16,22,56,72,98,131,340,4,126,SYS_access,SYS_arch_prctl,SYS_brk,SYS_close,SYS_execve,SYS_exit_group,SYS_fstat,SYS_futex,SYS_getegid,SYS_geteuid,SYS_getgid,SYS_getuid,SYS_getrlimit,SYS_mmap,SYS_mprotect,SYS_munmap,SYS_open,SYS_read,SYS_rt_sigaction,SYS_rt_sigprocmask,SYS_set_robust_list,SYS_set_tid_address,SYS_write,0
};

static std::map<uint32_t, CallLimit> call_limit_list{
  {kTypeC, c_call_limit},
  {kTypeCpp, cpp_call_limit},
  {kTypePython, python_call_limit},
  {kTypeJava, java_call_limit},
  {kTypePascal, pascal_call_limit},
  {kTypeBash, bash_call_limit},
  {kTypePerl, perl_call_limit},
  {kTypeRuby, ruby_call_limit}
};

}
#endif
