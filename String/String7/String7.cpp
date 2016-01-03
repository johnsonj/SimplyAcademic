// String7.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestSuite.h"
#include<string>

//////////////////////////
// String to int
/////////////////////////

int to_i(const wchar_t*str) {
	// If null string, return 0
	if (*str == '\0')
		return 0;

	// Detect the sign
	bool is_negative = false;
	if (*str == '-') {
		is_negative = true;
		str++;
	}

	int value = 0;
	while (*str != '\0') {
		value *= 10;
		value += *str - '0';
		str++;
	}

	return is_negative ? -value : value;
}


void test_s_to_i() {
	TestSuite<int, const wchar_t*> test("String to int");


	test.AddTestCase(1, L"1");
	test.AddTestCase(-1, L"-1");
	test.AddTestCase(0, L"");
	test.AddTestCase(123, L"123");
	test.AddTestCase(-123, L"-123");

	test.AddSolution("C-Style Strings", to_i);

	test.Execute();


}


/***************************
 * Check that a sentence is a palindrome
 * Ignoring punctuation, spacing, casing
 *
 * For this, we'll just check for a-Z matching
 **************************/
bool is_valid(char c) {
	return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
}

char upcase(char c) {
	if (!is_valid(c))
		return '\0';
	return (c >= 'a') ? (c - ('a' - 'A')) : c;
}

const wchar_t* seek_valid(const wchar_t*ptr, bool forward, const wchar_t*limit) {
	while (ptr != limit && !is_valid(*ptr)) {
		if (forward)
			ptr++;
		else
			ptr--;
	}

	return ptr;
}

bool palindrome(const wchar_t*str) {
	if (str == nullptr)
		return false;

	// Establish front & back of string
	const wchar_t*front = str;
	const wchar_t*back = str;
	while (*(++back + 1) != '\0'); // Classic C style here

	// Iterate through our characters
	while (front < back) {
		front = seek_valid(front, true, back);
		back = seek_valid(back, false, front);
		if (upcase(*front) != upcase(*back))
			return false;
		front++;
		back--;
	}
	return true;
}

void test_palindrome() {
	TestSuite<bool, const wchar_t*> test("Generous Palindrome");

	test.AddTestCase(false, L"Ray a Ray");
	test.AddTestCase(true, L"A man, a plan, a canal, Panama.");
	test.AddTestCase(true, L"Able was I, ere I saw, Elba!");

	test.AddSolution("Palindrome", palindrome);

	test.Execute();
}

int _tmain(int argc, _TCHAR* argv[]){

	//test_s_to_i();
	test_palindrome();

	std::cout << "hey now";
	return 0;

}