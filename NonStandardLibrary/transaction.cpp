#include "transaction.hpp"

namespace nstd
{

class BasicRollbackVisitor : public IRollbackVisitor
{
  void visit(stack<HistoryInstance>& past) override
  {
    while(!past.empty())
    {
      past.top()->accept(*this);
      past.pop();
    }
  }

  void visit(IHistory& history) override
  {
    history.rollback();
  }
};

class Transaction : public ITransaction
{
public:
  Transaction(unique_ptr<IRollbackVisitor>&& _rollback) noexcept : rollback_visitor(std::move(_rollback)) {};
  ~Transaction()
  {
    if(!completed)
      rollback();
  }
  void complete() noexcept override
  {
    completed = true;
  }
  void rollback() override
  {
    accept(*rollback_visitor);
  }
  void push(HistoryInstance&& history) override
  {
      past->push(history);
  }
  void accept(IRollbackVisitor& visitor) override
  {
      visitor.visit(*past);
      complete();
  }
private:
  bool completed { false };
  Lazy<stack<HistoryInstance>> past;
  unique_ptr<IRollbackVisitor> rollback_visitor;
};

unique_ptr<ITransaction> StartTransaction()
{
  return make_unique<Transaction>(make_unique<BasicRollbackVisitor>());
}

unique_ptr<ITransaction> StartTransaction(unique_ptr<IRollbackVisitor>&& rollback_visitor)
{
  return make_unique<Transaction>(std::move(rollback_visitor));
}

}