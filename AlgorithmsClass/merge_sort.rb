# Merge Sort i-rb
require './asserts.rb'

def merge(a, b)
	res = []

	(b.length + a.length).times do
		if b.empty? or (!a.empty? && a[0] <= b[0])
			res << a.shift
		else
			res << b.shift
		end
	end

	res
end

def merge_sort(a)
	return a if a.length <= 1
	left = merge_sort(a[0, (a.length / 2)])
	right = merge_sort(a[(a.length / 2), a.length])

	merge(left, right)
end

def assert_merge_sort(a)
	assert_solutions(lambda{|x| x.sort}, lambda{|x| merge_sort(x)}, a)
end


assert_merge_sort [3,2,1]
assert_merge_sort [4,3,2,1]
assert_merge_sort [5,4,3,2,1]
assert_merge_sort [5,5,4,5,3,2,1]
assert_merge_sort 1.upTo(10)