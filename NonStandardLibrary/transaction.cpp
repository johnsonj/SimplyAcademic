#include "transaction.hpp"

namespace nstd
{

Transaction::Transaction() noexcept
{
}

Transaction::~Transaction()
{
  // TODO: Should this assert?
  if(!completed)
    rollback();
}

void Transaction::complete() noexcept
{
  completed = true;
}

void Transaction::rollback()
{
  if(completed)
  {
    // TODO: throw would be better here
    assert(false);
    return;
  }

  while(!past->empty())
  {
    past->top()->rollback();
    past->pop();
  }

  completed = true;
}

void Transaction::push(HistoryInstance&& history)
{
  past->push(history);
}

}