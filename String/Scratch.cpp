// String Exercises:
//	Dedup

#include "stdafx.h"
#include<string>
#include<iostream>
#include<map>
#include<functional>
#include<cmath>

using std::string;

bool assert(string actual, string expected) {
	if (actual == expected) {
		return true;
	}
	std::cout << "FAILURE | Actual: " << (actual) << "(" << std::to_string(actual.length()) << ") | Expected: " << expected << "(" << std::to_string(expected.length()) << ")\n";

	return false;
}

string dedup(string str) {
	std::map<char, bool> lookup;

	string result = "";

	for (int i = 0; i < str.length(); i++) {
		if (lookup[str[i]])
			continue;

		lookup[str[i]] = true;

		result.append(string{ str[i] });
	}

	return result;
}

string dedup_bitmask(string str) {
	const unsigned int lookup_size = 2 << (sizeof(wchar_t)*CHAR_BIT);

	bool lookup[lookup_size] = { 0 };

	string result = "";

	for (int i = 0; i < str.length(); i++) {
		if (lookup[str[i]])
			continue;

		lookup[str[i]] = true;

		result.append(string{ str[i] });
	}
	return result;
}

char* dedup_cstr(const char* str, int length) {
	if (str == nullptr || length < 0)
		return nullptr;

	const unsigned int lookup_size = 2 << (sizeof(wchar_t)*CHAR_BIT);
	bool lookup[lookup_size] = { 0 };

	char* result = (char*)malloc(sizeof(char) * (length + 1));
	char* current_result = result;
	for (int i = 0; i < length; i++) {
		if (lookup[str[i]])
			continue;

		lookup[str[i]] = true;

		*current_result = str[i];
		current_result++;
	}

	// Null terminate the string
	*current_result = '\0';

	return result;
}

string dedup_cstr_wrapper(string str) {
	char* result = dedup_cstr(str.c_str(), str.length());
	string result_string = { result };
	free(result);

	return result_string;
}

void dedup_cstr_inplace(char* str) {
	if (str == nullptr)
		return;

	const unsigned int lookup_size = 2 << (sizeof(wchar_t)*CHAR_BIT);
	bool lookup[lookup_size] = { 0 };

}

string dedup_cstr_inplace_wrapper(string str) {
	char *copy = (char*)malloc(sizeof(char) * (str.length() + 1));
	memcpy(copy, str.c_str(), str.length() + 1);
	dedup_cstr_inplace(copy);
	string result_string = { copy };
	free(copy);

	return result_string;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::map<string, string> test_cases;
	std::map<string, std::function<string(string)>> solutions;

	test_cases.emplace("Hey now", "Hey now");
	test_cases.emplace("aaaaa", "a");
	test_cases.emplace("ababa", "ab");

	solutions.emplace("Hash map, new string", dedup);
	solutions.emplace("Bitmask, new string", dedup_bitmask);
	solutions.emplace("CString, new string", dedup_cstr_wrapper);
	solutions.emplace("CString, in place", dedup_cstr_inplace_wrapper);

	for (auto sln = solutions.begin(); sln != solutions.end(); sln++) {
		std::cout << "Solution: " << sln->first << "\n";
		for (auto test_case = test_cases.begin(); test_case != test_cases.end(); test_case++) {
			assert(sln->second(test_case->first), test_case->second);
		}
		std::cout << "--\n";
	}

	return 0;
}

