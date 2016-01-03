def assert_solutions(base, solution, *input)
	base_sln = base.call(*input)
	attempted_sln = solution.call(*input)
	if base_sln != attempted_sln
		puts "#{input}: Error: solution fails. Expected: #{base_sln}, Actual: #{attempted_sln}"
	else
		puts "#{input}: Passes"
	end
end
