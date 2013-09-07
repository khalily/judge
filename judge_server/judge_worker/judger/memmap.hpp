#ifndef _OJ_MEMORY_MAP_HPP
#define _OJ_MEMORY_MAP_HPP


#include <sys/mman.h>
#include <cstdint>

namespace oj
{

class MemoryMap
{
public:
  MemoryMap(int32_t len, int32_t fd) :
      len_(len),
      fd_(fd),
      isMap(false) {
  }
  ~MemoryMap() {
    if (isMap)
      munmap(start_, len_);
  }
  const char* map() {
    if (isMap)
      return start_;
    if ((start_ = (char*) mmap(NULL,
                               len_,
                               PROT_READ | PROT_WRITE,
                               MAP_PRIVATE,
                               fd_,
                               0)) == MAP_FAILED) {
      return NULL;
    }
    isMap = true;
    return start_;
  }

private:
  char *start_;
  int32_t len_;
  int32_t fd_;
  bool isMap;
};

}
#endif
