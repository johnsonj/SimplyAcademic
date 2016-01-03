# Ruby implementation if karatsuba Multiplication
require './asserts.rb'

def digits(x)
	x.to_s.length
end

def subint(x,i,j)
	x.to_s[i,j].to_i
end

def split(n)
	length = digits(n)
	return [subint(n, 0, (length) / 2).floor, subint(n, ((length)/2).floor, length)]
end

# Break out the top
def karatsuba_multiply(x, y)
	length = digits(x)

	return x*y if digits(x) <= 2 || digits(y) <= 2

	a, b = split(x)
	c, d = split(y)

	# 1. A*C
	ac = karatsuba_multiply(a, c)
	# 2. B*D
	bd = karatsuba_multiply(b, d)
	# 3. (AD + BC) = (a+b)(c+d) - ac - bd
	ad_plus_bc = karatsuba_multiply(a + b, c + d) - ac - bd

	degree = digits(x) % 2 == 0 ? digits(x) : digits(x) + 1

	ac * (10**(degree)) + ad_plus_bc * (10**((degree)/2)) + bd
end

def base_mul(x, y)
	x*y
end

def assert_karatsuba(x, y)
	assert_solutions(lambda {|x,y| x*y}, lambda{|x,y| karatsuba_multiply(x,y)} , x, y)
end

assert_karatsuba(567, 123)
assert_karatsuba(5678,1234)
assert_karatsuba(56782,12341)
assert_karatsuba(567822,123411)
assert_karatsuba(12341222222221,12341222222221)
assert_karatsuba(10,10)
assert_karatsuba(2,1)
assert_karatsuba(1,1)
assert_karatsuba(10,10)