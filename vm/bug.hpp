#ifndef BUG_HPP
#define BUG_HPP

namespace r5 {
  void abort();
  void bug(const char* message);
  void bug(const char* message, const char* arg);
  void warn(const char* message);

  static inline void check(bool expr, const char* msg = "Check failed") {
    if(expr) return;
    bug(msg);
  }
}

#endif
