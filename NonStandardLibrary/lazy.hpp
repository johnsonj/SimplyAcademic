#pragma once
#include <memory>

namespace nstd
{

using std::unique_ptr;
using std::make_unique;

template<typename Type>
class Lazy
{
public:
  Type* operator->()
  {
    return &ensure();
  }

  Type& operator=(Type& v)
  {
    ensure();
    *obj = v;

    return *this;
  }

  Type& operator*()
  {
    return ensure();
  }

private:
  Type& ensure()
  {
    if(!obj)
      obj = make_unique<Type>();

    return *obj;
  }
  unique_ptr<Type> obj;
};

}