#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include "bindings.hpp"
#include "util/option.hpp"
#include "handle.hpp"

namespace r5 {
  class Dictionary {
    Bindings table_;

    friend class GCImpl;
  public:
    Dictionary(State& S)
      : table_(S)
    {}

    static void init(State& S, Class* dict);
    option<OOP> get(String* name);
    void set(State& S, String* name, OOP val);
    OOP keys(State& S);
  };

  typedef TypedHandle<Dictionary, OOP::eDictionary> HDictionary;
}

#endif
