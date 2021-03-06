#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace r5 {
  struct Buffer {
    uint8_t* buf;
    size_t   len;

    Buffer(uint8_t* b, size_t s)
      : buf(b)
      , len(s)
    {}

    Buffer(const char* c, size_t s)
      : buf((uint8_t*)c)
      , len(s)
    {}

    char* c_buf() {
      return (char*)buf;
    }

    uint8_t take1() {
      uint8_t c = *buf;
      buf++;
      len--;
      return c;
    }

    uint8_t peek1() {
      return *buf;
    }

    bool empty_p() {
      return len == 0;
    }

    int trim(size_t count) {
      if(count > len) count = len;

      buf += count;
      len -= count;

      return count;
    }
  };

  struct ScratchBuffer {
    uint8_t* buf;
    size_t   len;
    size_t   capa;

    static const int cDefaultBufferSize = 32;

    ScratchBuffer()
      : buf(new uint8_t[cDefaultBufferSize])
      , len(0)
      , capa(cDefaultBufferSize)
    {}

    ~ScratchBuffer() {
      delete buf;
    }

    bool empty_p() {
      return len == 0;
    }

    void append(uint8_t c) {
      if(len == capa) {
        capa += cDefaultBufferSize;
        uint8_t* n = new uint8_t[capa];
        memcpy(n, buf, len-1);
        delete buf;
        buf = n;
      }

      buf[len] = c;
      len++;
    }

    uint8_t last() {
      return buf[len-1];
    }

    void remove_last() {
      len--;
    }

    char* copy_out() {
      buf[len] = 0;
      return strdup((const char*)buf);
    }
  };
}

#endif
