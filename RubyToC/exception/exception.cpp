#include<stdexcept>
#include<string.h>
#include<iostream>
#include<typeinfo>

// Used to marshal the exception information to the caller.
// Caller is responsible for freeing the structure and char*
struct WrappedException
{
  char* type = nullptr;
  char* what = nullptr;
};

// Helper to allocate and copy a string.
// There's probably a std library method I'm missing here.
char* strcopyalloc(const char* str)
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
  pWrappedException->what = strcopyalloc(e.what());
  pWrappedException->type = strcopyalloc(typeid(e).name());

  *ppWrappedException = pWrappedException;
}

#define EXPORT_WRAPPED_OPENED(type, fn, wrapped_name, failure_return) \
  extern "C"                                                \
  {                                                         \
  type wrapped_name(void** ppWrappedException) throw()      \
  {                                                         \
    try                                                     \
      return fn();                                          \
    catch(const std::exception& e)                          \
      handle_exception(e, ppWrappedException);              \
    catch(...)                                              \
      std::cout << "Unknown exception thrown" << std::endl; \
    failure_return;                                         \
  }                                                         \
  }                                                         \

// Need to define a specialization for each type because they may have unique 'failure_return' segments
// Mostly needed to support void methods as they don't have to default returns.
#define EXPORT_WRAPPED_void(fn, wrapped_name) EXPORT_WRAPPED_OPENED(void, fn, wrapped_name, return)
#define EXPORT_WRAPPED_bool(fn, wrapped_name) EXPORT_WRAPPED_OPENED(bool, fn, wrapped_name, bool error_val = {}; return error_val;)

#define EXPORT_WRAPPED(type, fn) EXPORT_WRAPPED_##type(fn, Wrapped ## fn)

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