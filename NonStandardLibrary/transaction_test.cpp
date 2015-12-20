#include "transaction.hpp"
#include<cassert>

using namespace nstd;


void set_int()
{
  int foo = 0;
  {
    Transaction trans;
    Set<int>(trans, foo, 5);
    assert(foo == 5);
    trans.complete();
    assert(foo == 5);
  }
}

void rollback_int()
{
  int foo = 0;
  {
    Transaction trans;
    Set<int>(trans, foo, 2);
    assert(foo == 2);
    trans.rollback();
    assert(foo == 0);
  }
}

void nested()
{
  int foo = 0;
  {
    Transaction trans;
    Set<int>(trans, foo, 1);
    {
      assert(foo == 1);
      Set<int>(trans, foo, 5);
      assert(foo == 5);
    }
    assert(foo == 5);
  }
  assert(foo == 0);
}

int main(int, char**)
{
  set_int();

  rollback_int();

  nested();

  return 0;
}