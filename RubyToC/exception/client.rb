#!/usr/bin/env ruby
require 'ffi'

module ExceptionLib
	extend FFI::Library

	ffi_lib "./exception.so"

	attach_function 'WrappedTrigger', [:pointer], :void
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

class LibraryException < Exception
  attr_accessor :what, :type

  def initialize(error_result)
    @what = error_result[:what]
    @type = LibraryException.clean_cpp_type(error_result[:type])
  end

  def self.clean_cpp_type(typeid)
    `c++filt -t #{typeid}`
  end

  def self.make_name_const(typeid)
    full_name = clean_cpp_type typeid
    full_name.delete!("\n")
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

module WrappedLib
  def self.Trigger
    error_result = FFI::MemoryPointer.new(:pointer)
    ExceptionLib.WrappedTrigger(error_result)

    if error_result.get_pointer(0)
      error = ErrorResult.new(error_result.get_pointer(0))
      raise LibraryException.create_native(error)
    end
  end
end


begin
  WrappedLib.Trigger
rescue LibraryException => e
  puts e.type
  puts e.what
end

WrappedLib.Trigger
