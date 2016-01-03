#include<vector>
#include<map>
#include<functional>
#include<iostream>
#include<string>


/********************************************************************
 * Simple TestSuite. Built to aid in those one-off scratch programs. 
 * The ones you'd rather have code verify instead of your tired eyes.
 *
 * Usage:
 *	
 *		Given a function:
 *			bool is_two(int value) {
 *				return value == 2;
 *			}
 *	
 *		TestSuite<bool, int> runner;	// Create a new test suite. TestSuite<output_type, input_types...>
 *		runner.AddTestCase(true, 2);	// Add test case(s). AddTestCase(output_result, inputs...)
 *		runner.AddTestCase(false, 1);	
 *		runner.AddSolution("Valid Solution", is_two); // Add solutions
 *		runner.Execute();				// Run and done. Output in STDOUT
*********************************************************************/

template <class T>
std::ostream& operator<<(std::ostream&os, const std::vector<T>& v) {
	os << "{";
	std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, ", "));
	os << "}";
	return os;
}

template<typename TestResultType, typename ...TestInputTypes>
class TestSuite {
	using SolutionType = typename std::function < TestResultType(TestInputTypes...) >;
public:
	TestSuite() { }
	TestSuite(std::string _test_batch_name) : test_batch_name(_test_batch_name) { }
	void AddSolution(std::string name, SolutionType handle) {
		solutions.emplace(name, handle);
	}
	void AddTestCase(TestResultType result, TestInputTypes... inputs) {
		// We need to bind the variable input params to a function placeholder that we can later call with the solutions
		tests.emplace(result, std::bind(&TestSuite::wrap_solution, this, std::placeholders::_1, inputs...));
	}
	void Execute() {
		std::cout << "= Start batch";
		if (test_batch_name.length())
			std::cout << ": " << test_batch_name;

		std::cout << "\n";

		for (auto sln = solutions.begin(); sln != solutions.end(); sln++) {
			std::cout << " - Solution: " << sln->first << "\n";
			for (auto test = tests.begin(); test != tests.end(); test++) {
				assert(test->second(sln->second), test->first);
			}
			std::cout << "\n - end\n";
		}
		std::cout << "= end \n\n";
	}

private:
	bool assert(TestResultType actual, TestResultType expected) {
		if (actual != expected) {
			std::cout << "\nFAILURE | Actual: " << (actual) << " | Expected: " << expected << "\n";
			return false;
		}
		std::cout << ".";
		return true;
	}
	TestResultType wrap_solution(SolutionType sln, TestInputTypes...inputs) {
		return std::bind(sln, inputs...)();
	}
	std::map<std::string, SolutionType> solutions;
	std::map<TestResultType, std::function<TestResultType(SolutionType)>> tests;
	std::string test_batch_name;
};