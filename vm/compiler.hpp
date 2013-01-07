#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <stdio.h>

namespace marius {
  class Code;

  class Compiler {
    bool debug_;
    Code* code_;

  public:
    Compiler(bool debug=false)
      : debug_(debug)
      , code_(0)
    {}

    Code* code() {
      return code_;
    }

    bool compile(FILE* f);
  };
}

#endif