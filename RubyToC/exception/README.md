# RubyToC: Exception #

This example allows us to throw C++ exceptions into Ruby. This is achieved by catching all of the exceptions at the API surface and marshaling the information into a `WrappedException` object which is passes as an out param by the API. 

The Ruby caller uses a wrapper around the FFI library that checks for the presence of the of the error. If it exists, it creates a new class that matches the name of the C++ exception and throws it.

The goal of this is to provide error handling across the two languages little impact on the calling code. The client's integration work looks like this:

```C++

// Given a library method:
bool doWork();

EXPORT_WRAPPED(bool, doWork);
```

```ruby
module Lib
    extend FFI::Library

    attach_function 'WrappedDoWork', [:pointer], :bool
end

lib = WrappedLib.new(Lib)

begin
    if lib.doWork

    end
rescue LibraryException => e
    puts "Exception raise: #{e.type}: #{e.what}"
end
```

## Future Work ##
 1. Remove the need to specify 'Wrapped[..]' when attaching functions to the library
 2. Allow clients to catch specific exceptions (eg: StdLogicError). This can't happen yet because the classes are created when the exception is thrown.
 3. Allow parameters in the EXPORT_WRAPPED macro