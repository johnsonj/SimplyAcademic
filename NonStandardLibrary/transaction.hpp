#pragma once
#include <memory>
#include <stack>
#include <cassert>
#include "lazy.hpp"

namespace nstd
{

class ITransaction;
class IHistory;
class IRollbackVisitor;

using HistoryInstance = unique_ptr<IHistory>;

unique_ptr<ITransaction> StartTransaction();
unique_ptr<ITransaction> StartTransaction(unique_ptr<IRollbackVisitor>&& rollbackVisitor);

class ITransaction
{
public:
  ITransaction() noexcept {};
  virtual ~ITransaction() {}
  virtual void complete() noexcept = 0;
  virtual void push(HistoryInstance&& history) = 0;
  virtual void rollback() = 0;
  virtual void accept(IRollbackVisitor& visitor) = 0;
};

class IHistory
{
public:
  virtual ~IHistory() noexcept {}
  virtual void rollback() = 0;
  virtual void accept(IRollbackVisitor& visitor) = 0;
};

class IRollbackVisitor
{
public:
  virtual ~IRollbackVisitor() noexcept {}
  virtual void visit(std::stack<HistoryInstance>&) = 0;
  virtual void visit(IHistory&) = 0;
};

}