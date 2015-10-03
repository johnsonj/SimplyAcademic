#!/usr/bin/env ruby
require 'ffi'

module ExceptionLib
	extend FFI::Library

	ffi_lib "./exception.so"

	attach_function 'WrappedTrigger', [:pointer], :void
  attach_function 'WrappedSuccess', [:pointer], :bool
end

def dump_ptr_ptr(ptr)
  puts ptr
  puts "* #{ptr.get_pointer(0)}"
end

class ErrorResult < FFI::ManagedStruct
  layout :type,:string,
         :what, :string
  def self.release(ptr)
  end
end

#
# Base exception object for exceptions thrown by the library.
#
# Creates a new exception class for each unique exception thrown
# by the library. Currently done at time of throw but could be moved
# to load so clients can catch specific exceptions.
#
class LibraryException < StandardError
  attr_accessor :what, :type

  def initialize(error_result)
    @what = error_result[:what]
    @type = LibraryException.clean_cpp_type(error_result[:type])
  end

  def self.clean_cpp_type(typeid)
    `c++filt -t #{typeid}`.delete!("\n")
  end

  def self.make_name_const(typeid)
    full_name = clean_cpp_type typeid
    full_name.split(/[::,_]/).each { |token| token.capitalize! }.join("")
  end

  def self.create_native(error_result)
    exception_name = make_name_const(error_result[:type])
    unless LibraryException.const_defined? exception_name
      exception_class = Class.new(LibraryException)
      LibraryException.const_set(exception_name, exception_class)
    end

    LibraryException.const_get(exception_name).new(error_result)
  end
end

#
# Wrapper for the exception calling convention
# 
# Use this class to wrap a FFI module. It will check for
# the presence of exceptions in library calls and throw them.
#
class WrappedLib
  def initialize(library)
    @library = library
  end

  def method_missing(method, *args, &block)
    error_result = FFI::MemoryPointer.new(:pointer)

    res = @library.send("Wrapped#{method}", error_result, *args)

    if error_result.get_pointer(0) != nil
      error = ErrorResult.new(error_result.get_pointer(0))
      raise LibraryException.create_native(error)
    end

    res
  end
end

lib = WrappedLib.new(ExceptionLib)

puts "Return from library: #{lib.Success}"

begin
  lib.Trigger
rescue LibraryException => e
  puts "Caught exception: #{e.type}, reason: #{e.what}"
end

begin
  lib.Trigger
rescue LibraryException => e
  puts "Caught again"
end