#include<stdexcept>
#include<string.h>
#include<iostream>
#include<typeinfo>

extern "C"
{

struct WrappedException
{
  char* type;
  char* what;
};

void Trigger()
{
  throw std::logic_error("Yoo rolf");
}

char* copyalloc(const char* str)
{
  char* copy = new char[strlen(str) + 1];
  strcpy(copy, str);
  return copy;
}

void WrappedTrigger(void** ppWrappedException)
{
  try
  {
    Trigger();
  }
  catch(const std::exception& e)
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
    std::cout << "Caught:" << typeid(e).name() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception throw" << std::endl;
  }
}

}