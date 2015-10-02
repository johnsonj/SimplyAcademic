require 'ffi'

module HelloLib
	extend FFI::Library
	ffi_lib "./hello.so"

	attach_function 'hello', [], :void
end

HelloLib.hello