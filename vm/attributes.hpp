#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

#include "oop.hpp"
#include "string_map.hpp"
#include "bindings.hpp"

namespace r5 {
  class GCImpl;

  class Attributes {
    Bindings attributes_;

    friend class GCImpl;
  public:
    Attributes(State& S)
      : attributes_(S)
    {}

    OOP attribute(String* name, bool* found=0);
    void set_attribute(State& S, String* name, OOP val);
    OOP keys(State& S);
    OOP values(State& S);
  };
}

#endif
