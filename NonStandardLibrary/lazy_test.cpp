#include "lazy.hpp"
#include <cassert>

using namespace nstd;

void basic_type()
{
  Lazy<int> v;
  *v = 10;

  assert(*v == 10);
}

int main(int, char**)
{
  basic_type();
}