require 'ffi'

module ResourceLib
	extend FFI::Library
	ffi_lib "./resource.so"

	attach_function 'CreateIntDb', [], :pointer
	attach_function 'CreateInstance', [], :pointer
	attach_function 'DestroyIntDb', [:pointer], :bool
	attach_function 'AppendValue', [:pointer, :int], :int
	attach_function 'GetValue', [:pointer, :int], :int
end


class DatabaseInstance
	def initialize(_handle)
		@handle = _handle
	end

	def <<(v)
		ResourceLib.AppendValue(@handle, v)
	end

	def [](pos)
		ResourceLib.GetValue(@handle, pos)
	end
end

def using_db
	handle = ResourceLib.CreateIntDb

	begin
		yield DatabaseInstance.new(handle)
	rescue Exception => e
	end

	ResourceLib.DestroyIntDb(handle)
end

handle = ResourceLib.CreateIntDb

using_db do |db|
	0.upto(5) do |i|
		db << i * 2
	end

	0.upto(5) do |i|
		puts db[i]
	end
end


class ManagedDB
	class Instance < FFI::ManagedStruct
		layout :instance, :pointer
		def self.release(ptr)
			ResourceLib.DestroyIntDb(self[:instance])
		end
	end

	def initialize
		@instance = Instance.new(ResourceLib.CreateInstance)
	end

	def <<(v)
		ResourceLib.AppendValue(@instance[:instance], v)
	end

	def [](pos)
		ResourceLib.GetValue(@instance[:instance], pos)
	end
end


db = ManagedDB.new
db << 50
puts db[0]