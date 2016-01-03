#pragma once

namespace nstd { namespace test {

/*
test::Runner test(format_strategy::print());
test.is_true(true, L"The truth");
*/


template<typename Matchers>
class IRunner : public Matchers
{
public:
  virtual ~IRunner() {}
};

class BasicMatchers
{
public:
  void is_true(expected, const wchar_t* wzMessage)
  {
    if(expected)
      return;

    raise new FailedExpectation(wzMessage);
  }
};



} }