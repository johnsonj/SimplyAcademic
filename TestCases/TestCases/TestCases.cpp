// TestCases.cpp : Sample usage of the TestSuite library.

#include "stdafx.h"
#include "TestSuite.h"
#include<functional>


bool valid(bool value) {
	return value;
}

bool invalid(bool value) {
	return !value;
}

int multi_input_valid(int a, int b) {
	return a + b;
}

int multi_input_invalid(int a, int b) {
	return a + b + 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestSuite<bool, bool> runner("Simple Input Runner");

	runner.AddSolution("Valid Solution", valid);
	runner.AddSolution("Invalid Solution", invalid);

	runner.AddTestCase(true, true);
	runner.AddTestCase(false, false);

	runner.Execute();

	TestSuite<int, int, int> multi_input_runner("Multi-Input Runner");

	multi_input_runner.AddSolution("Valid Solution", multi_input_valid);
	multi_input_runner.AddSolution("Invalid Solution", multi_input_invalid);

	multi_input_runner.AddTestCase(4, 2, 2);
	multi_input_runner.AddTestCase(8, 4, 4);
	multi_input_runner.AddTestCase(3, 2, 1);

	multi_input_runner.Execute();

	return 0;
}

