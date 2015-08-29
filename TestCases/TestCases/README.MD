# TestSuite #
A lightweight, header based C++ library for executing mutliple test cases across multiple solutions.

## Usage ##
Create a new instance of the test runner: `TestSuite<return_value, input_types,...> runner("Test Name")`. The first template type is the return type of the solutions. The remaining are the input types in order. This can be 1 or more inputs.

Add test cases to the runner: `runner.AddTestCase(expected_return_value, solution_inputs, ...)

Add solutions to the runner: `runner.AddSolution("Name of Solution", reference_to_method)`

Run it: `runner.Execute()`

## Example ##
Given a function:

```C++
bool is_two(int value)
{
  return value == 2;
}
bool is_invalid(int)
{
  return false;
}
```

The following will execute two tests against two solution

```C++
#include<TestSuite.h>
TestSuite<bool, int> runner;	// Create a new test suite. TestSuite<output_type, input_types...>
runner.AddTestCase(true, 2);	// Add test case(s). AddTestCase(output_result, inputs...)
runner.AddTestCase(false, 1);	
runner.AddSolution("Valid Solution", is_two); // Add solutions
runner.AddSolution("Invalid Solution", is_invalid); // Add solutions
runner.Execute();				// Run and done. Output in STDOUT
```