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

using std::unique_ptr;
using std::make_unique;
using std::stack;

using HistoryInstance = IHistory*;

unique_ptr<ITransaction> StartTransaction();
unique_ptr<ITransaction> StartTransaction(unique_ptr<IRollbackVisitor>&& rollbackVisitor);

class IRollbackSite
{
public:
  virtual ~IRollbackSite() {}
  virtual void accept(IRollbackVisitor& visitor) = 0;
};

class ITransaction : public IRollbackSite
{
public:
  ITransaction() noexcept {};
  virtual ~ITransaction() {}
  virtual void complete() noexcept = 0;
  virtual void push(HistoryInstance&& history) = 0;
  virtual void rollback() = 0;
};

class IHistory : public IRollbackSite
{
public:
  virtual ~IHistory() noexcept {}
  virtual void rollback() = 0;
};

class IRollbackVisitor
{
public:
  virtual ~IRollbackVisitor() {}
  virtual void visit(stack<HistoryInstance>&) = 0;
  virtual void visit(IHistory&) = 0;
};

}