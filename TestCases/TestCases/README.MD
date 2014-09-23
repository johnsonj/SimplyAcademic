 # Simple TestSuite
 Built to aid in those one-off scratch programs. 
 The ones you'd rather have code verify instead of your tired eyes.

## Usage:
Given a function:

```C++
bool is_two(int value) {
	return value == 2;
}
```

The following will execute two tests against the single solution

```C++
TestSuite<bool, int> runner;	// Create a new test suite. TestSuite<output_type, input_types...>
runner.AddTestCase(true, 2);	// Add test case(s). AddTestCase(output_result, inputs...)
runner.AddTestCase(false, 1);	
runner.AddSolution("Valid Solution", is_two); // Add solutions
runner.Execute();				// Run and done. Output in STDOUT
```
