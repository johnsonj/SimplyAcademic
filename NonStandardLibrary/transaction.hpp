#pragma once
#include <memory>
#include <stack>
#include <cassert>
#include "lazy.hpp"

namespace nstd
{

class ITransaction;
class IHistory;

using std::unique_ptr;
using std::make_unique;
using std::stack;

using HistoryInstance = IHistory*;

class ITransaction
{
public:
  ITransaction() noexcept {};
  virtual ~ITransaction() {}
  virtual void complete() noexcept = 0;
  virtual void rollback() = 0;
  virtual void push(HistoryInstance&& history) = 0;
};

class IHistory
{
public:
  virtual ~IHistory() noexcept {}
  virtual void rollback() = 0;
};

class Transaction : public ITransaction
{
public:
  Transaction() noexcept;
  ~Transaction();
  void complete() noexcept override;
  void rollback() override;
  void push(HistoryInstance&& history) override;

private:
  bool completed { false };
  Lazy<stack<HistoryInstance>> past;
};

template<typename ValType>
class History : public IHistory
{
public:
  History(ValType& _var, ValType& _new_val) : old_var(_var), var(_var)
  {
    var = _new_val;
  }
  void rollback() override
  {
    var = old_var;
  }
private:
  ValType& var;
  ValType old_var;
};

template<typename ValType>
void Set(ITransaction& trans, ValType& var, ValType new_value)
{
  trans.push(new History<ValType>(var, new_value));
}

}