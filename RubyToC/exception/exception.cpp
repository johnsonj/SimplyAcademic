#include<stdexcept>
#include<string.h>
#include<iostream>
#include<typeinfo>

struct WrappedException
{
  char* type = nullptr;
  char* what = nullptr;
};

char* copyalloc(const char* str)
{
  char* copy = new char[strlen(str) + 1];
  strcpy(copy, str);
  return copy;
}

void handle_exception(const std::exception& e, void** ppWrappedException)
{
  if (ppWrappedException == nullptr)
  {
    std::cout << "Exception space is nullptr" << std::endl;
    return;
  }
  WrappedException* pWrappedException = new WrappedException;
  pWrappedException->what = copyalloc(e.what());
  pWrappedException->type = copyalloc(typeid(e).name());

  *ppWrappedException = pWrappedException;
}

#define EXPORT_WRAPPED_OPENED(type, fn, wrapped_name, failure_return) \
  extern "C"                                                \
  {                                                         \
  type wrapped_name(void** ppWrappedException) throw()      \
  {                                                         \
    try                                                     \
    {                                                       \
      return fn();                                          \
    }                                                       \
    catch(const std::exception& e)                          \
    {                                                       \
      handle_exception(e, ppWrappedException);              \
    }                                                       \
    catch(...)                                              \
    {                                                       \
      std::cout << "Unknown exception thrown" << std::endl; \
    }                                                       \
    failure_return;                                         \
  }                                                         \
  }                                                         \

#define EXPORT_WRAPPED_void(fn, wrapped_name) EXPORT_WRAPPED_OPENED(void, fn, wrapped_name, return)
#define EXPORT_WRAPPED_bool(fn, wrapped_name) EXPORT_WRAPPED_OPENED(bool, fn, wrapped_name, bool error_val = {}; return error_val;)

#define EXPORT_WRAPPED(type, fn)                                                            \
    EXPORT_WRAPPED_##type(fn, Wrapped ## fn)                                                \

// Client examples
void Trigger()
{
  throw std::logic_error("Yoo rolf");
}
bool Success()
{
  return true;
}

EXPORT_WRAPPED(void, Trigger);
EXPORT_WRAPPED(bool, Success);