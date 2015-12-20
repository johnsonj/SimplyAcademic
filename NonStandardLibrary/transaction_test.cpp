#include "transaction.hpp"
#include<cassert>

using namespace nstd;

namespace client_code
{

class MergableHistory : public nstd::IHistory
{
public:
  virtual bool can_merge(MergableHistory& history) = 0;
};

template<typename ValType>
class HistoryNode : public MergableHistory
{
public:
  HistoryNode(ValType& _var, const ValType& _new_val) : old_var(_var), var(_var)
  {
    var = _new_val;
  }
  
  void rollback() override
  {
    var = old_var;
  }
  
  void accept(IRollbackVisitor& visitor) override
  {
    visitor.visit(*this);
  }

  bool can_merge(MergableHistory& mergable_history) override
  {
    auto& history = static_cast<HistoryNode<ValType>&>(mergable_history);
    // Merge history for the same memory location
    return &history.var == &var;
  }

private:
  ValType& var;
  ValType old_var;
};

class MergableRollbackVisitor : public nstd::IRollbackVisitor
{
  void visit(stack<HistoryInstance>& past) override
  {
    // Only our own history nodes should exist
    // Could this cast be avoided by visiting the node? 
    auto previous = static_cast<MergableHistory*>(past.top());
    while(!past.empty())
    {
      auto current = static_cast<MergableHistory*>(past.top());

      // if it can't merge it then run it now
      if(!current->can_merge(*previous))
        current->accept(*this);

      previous = std::move(current);
      past.pop();
    }

    previous->accept(*this);
  }

  void visit(IHistory& history) override
  {
    history.rollback();
  }
};

template<typename ValType>
void Set(unique_ptr<ITransaction>& trans, ValType& var, ValType new_value)
{
  trans->push(new HistoryNode<ValType>(var, new_value));
}

}

using namespace client_code;

void set_int()
{
  int foo = 0;
  {
    auto trans = StartTransaction();
    Set<int>(trans, foo, 5);
    assert(foo == 5);
    trans->complete();
    assert(foo == 5);
  }
}

void rollback_int()
{
  int foo = 0;
  {
    auto trans = StartTransaction();
    Set<int>(trans, foo, 2);
    assert(foo == 2);
    trans->rollback();
    assert(foo == 0);
  }
}

void nested()
{
  int foo = 0;
  {
    auto trans = StartTransaction();
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

void mergable()
{
  int foo = 0;
  {
    auto trans = StartTransaction(make_unique<MergableRollbackVisitor>());
    Set<int>(trans, foo, 1);
    assert(foo == 1);
  }
  assert(foo == 0);

  {
    auto trans = StartTransaction(make_unique<MergableRollbackVisitor>());
    Set<int>(trans, foo, 1);
    {
      Set<int>(trans, foo, 5);
    }
  }
  assert(foo == 0);
}

int main(int, char**)
{
  set_int();

  rollback_int();

  nested();

  mergable();

  return 0;
}