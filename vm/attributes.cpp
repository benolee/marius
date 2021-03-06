#include "attributes.hpp"

namespace r5 {
  OOP Attributes::attribute(String* name, bool* found) {
    option<OOP> v = attributes_.find(name);
    if(v.set_p()) {
      if(found) *found = true;
      return *v;
    }

    if(found) *found = false;
    return OOP::nil();
  }

  void Attributes::set_attribute(State& S, String* name, OOP val) {
    attributes_.set(S, name, val);
  }

  OOP Attributes::keys(State& S) {
    return attributes_.keys(S);
  }

  OOP Attributes::values(State& S) {
    return attributes_.values(S);
  }
}
