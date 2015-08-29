//
// spell-checker-tests.c by Jeff Johnson
//
// test_large_dictionary_large_document & test_benchmark assume a few text files exist in the 'text/..' directory
// relative to where this executable is built. To run without these tests, comment them out in main()
//

#include "spell-checker.h"
#include "spell-checker-private.h"
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Test Cases
void test_is_valid_char();
void test_can_open_close_dict();
void test_can_add_and_check_single_word();
void test_handles_upper_lower_case();
void test_multi_word();
void test_callbacks();
void test_failed_add();
void test_special_characters();
void test_large_dictionary_large_document();
void test_benchmark();

// Helpers
void assert_failure_count(SpellCheckerDictionaryHandle dict, const char* search, int count);
void assert_single_failure_content(SpellCheckerDictionaryHandle dict, const char* search, const char* failure);
void incrementFailureCount(const char *unmatchedWord);
void storeLastFailure(const char *unmatcheWord);
void ignoreFailure(const char *unmatchedWord);
int g_failureCount = 0;
char *g_lastFailure = NULL;
double get_wall_time();
double get_cpu_time();
char* read_entire_file(const char *path);
void add_each_line_to_dict(SpellCheckerDictionaryHandle dict, const char* file_path);
#define TEST_CASE(fn) printf("Test Case: " #fn "\n"); fn();


int main(int cargs, char** vargs)
{
	// Unit/helper tests
	TEST_CASE(test_is_valid_char);

	// API tests
	TEST_CASE(test_can_open_close_dict);
	TEST_CASE(test_can_add_and_check_single_word);
	TEST_CASE(test_handles_upper_lower_case);
	TEST_CASE(test_multi_word);
	TEST_CASE(test_callbacks);
	TEST_CASE(test_failed_add);
	TEST_CASE(test_large_dictionary_large_document);

	// Performance
	TEST_CASE(test_benchmark);
}

//
// Test Cases
//
void test_is_valid_char()
{
	assert(is_valid_char('0')  == true);
	assert(is_valid_char('9')  == true);
	assert(is_valid_char('A')  == true);
	assert(is_valid_char('a')  == true);
	assert(is_valid_char('Z')  == true);
	assert(is_valid_char('z')  == true);
	assert(is_valid_char(':')  == false);
	assert(is_valid_char('\0') == false);
	assert(is_valid_char('[')  == false);
	assert(is_valid_char('{')  == false);
	assert(is_valid_char(0x7f) == false);
	assert(is_valid_char(0x80) == true);
	assert(is_valid_char(0xFF) == true);
}

void test_can_open_close_dict()
{
	SpellCheckerDictionaryHandle dictionaryHandle = createSpellCheckerDictionary();
	assert(dictionaryHandle != NULL);
	int closeResult = closeSpellCheckerDictionary(dictionaryHandle);
	assert(closeResult == 0);
}

// Ensure we can find a single success and invalid word even if it's a subset of a word
void test_can_add_and_check_single_word()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	const char helloworld[] = "helloworld";

	int addResult = spellCheckerAddWord(dict, helloworld);
	assert(addResult == 0);

	spellCheck(dict, helloworld, incrementFailureCount);
	assert_failure_count(dict, helloworld, 0);
	assert_failure_count(dict, "hello", 1);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

// Ensure we can find failures in a multi word stream at the front, end, middle
void test_multi_word()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	const char hello[] = "hello";
	int addResult = spellCheckerAddWord(dict, hello);
	assert(addResult == 0);

	assert_failure_count(dict, "hello", 0);
	assert_failure_count(dict, "hey hello", 1);
	assert_failure_count(dict, "hello hey", 1);
	assert_failure_count(dict, "hello hey hello", 1);
	assert_failure_count(dict, "hey hello hey", 2);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

void test_handles_upper_lower_case()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	const char hello_lower[] = "hello";
	int addResult = spellCheckerAddWord(dict, hello_lower);
	assert(addResult == 0);

	assert_failure_count(dict, "HELLO", 0);
	assert_failure_count(dict, "heLLo", 0);
	assert_failure_count(dict, "Hello", 0);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

void test_callbacks()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	const char heynow[] = "heynow";
	int addResult = spellCheckerAddWord(dict, heynow);
	assert(addResult == 0);

	// It should maintain the valid special characters and down case the alpha chars
	const char complex_str1[] = { 'H', 'E', 0xF8, 'L', 0xFF, ';', '\0' };
	const char expected_complex_str1[] = { 'h', 'e', 0xF8, 'l', 0xFF, '\0'};

	// 0x80 is a valid char so it the callback should get the same string back
	const char complex_str2[] = { 'h', 'e', 'y', 0x80, 'o', 'w', '\0' };

	// 0x7F is not a valid char so it should act as a seperator
	const char complex_str3[] = { 'h', 'e', 'y', 0x7F, 'o', 'w', '\0' };
	const char expected_complex_str3[] = { 'o', 'w', '\0' };

	assert_single_failure_content(dict, "hello", "hello");
	assert_single_failure_content(dict, "HELLO", "hello");
	assert_single_failure_content(dict, "HEL!LO", "lo");
	assert_single_failure_content(dict, complex_str1, expected_complex_str1);
	assert_single_failure_content(dict, complex_str2, complex_str2);
	assert_single_failure_content(dict, complex_str3, expected_complex_str3);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}


void test_failed_add()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	char heynow[] = "heynow";
	char invalid_word[] = "ell!o";
	int addResult = spellCheckerAddWord(dict, heynow);
	assert(addResult == 0);
	addResult = spellCheckerAddWord(dict, invalid_word);
	assert(addResult == -1);

	assert_failure_count(dict, heynow, 0);
	assert_failure_count(dict, invalid_word, 2);
	// Ensure the invalid sub string isn't in the tree
	assert_failure_count(dict, "ell", 1);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

void test_large_dictionary_large_document()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	char* unix_words_file = read_entire_file("text/words.txt");
	add_each_line_to_dict(dict, "text/words.txt");

	assert_failure_count(dict, unix_words_file, 0);

	free(unix_words_file);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

void test_benchmark()
{
	SpellCheckerDictionaryHandle dict = createSpellCheckerDictionary();
	assert(dict != NULL);

	char* collateral_titus_andronicus = read_entire_file("titus_andronicus.txt");
	char* collateral_english_lectures = read_entire_file("williams_stevens_lecture.txt");
	char* unix_words_file = read_entire_file("text/words.txt");
	add_each_line_to_dict(dict, "text/words.txt");

	double cpu_time_start = get_cpu_time();
	double wall_time_start = get_wall_time();

	spellCheck(dict, unix_words_file, ignoreFailure);
	spellCheck(dict, collateral_titus_andronicus, ignoreFailure);
	spellCheck(dict, collateral_english_lectures, ignoreFailure);

	double cpu_time_end = get_cpu_time();
	double wall_time_end = get_wall_time();

	free(collateral_titus_andronicus);
	free(collateral_english_lectures);
	free(unix_words_file);

	printf("CPU Time: \t%fs\n", cpu_time_end - cpu_time_start);
	printf("Wall Time: \t%fs\n", wall_time_end - wall_time_start);

	int closeResult = closeSpellCheckerDictionary(dict);
	assert(closeResult == 0);
}

//
// Helpers
//
void assert_failure_count(SpellCheckerDictionaryHandle dict, const char *search, int count)
{
	g_failureCount = 0;
	spellCheck(dict, search, incrementFailureCount);
	assert(g_failureCount == count);
}

void assert_single_failure_content(SpellCheckerDictionaryHandle dict, const char *search, const char *expected)
{
	spellCheck(dict, search, storeLastFailure);

	assert(strncmp(g_lastFailure, expected, strlen(expected)) == 0);

	if (g_lastFailure != NULL)
	{
		free(g_lastFailure);
		g_lastFailure = NULL;
	}
}

// Callbacks suitable for SpellCheckerCallback
void ignoreFailure(const char *unmatchedWord)
{
}

void incrementFailureCount(const char *unmatchedWord)
{
	g_failureCount += 1;
}

void storeLastFailure(const char *unmatchedWord)
{
	if (g_lastFailure != NULL)
	{
		free(g_lastFailure);
		g_lastFailure = NULL;
	}

	// Copy the word locally
	size_t length = strlen(unmatchedWord);
	g_lastFailure = malloc(length + 1);
	strncpy(g_lastFailure, unmatchedWord, length);

	// Ensure it's null terminated
	g_lastFailure[length] = '\0';
}


//
// Function mostly from: http://www.fundza.com/c4serious/fileIO_reading_all/
//
char* read_entire_file(const char *path)
{
	FILE *in_file;
	char *buffer;
	long num_bytes;

	in_file = fopen(path, "r");
	if (in_file == NULL)
		return NULL;

	// Get the number of bytes
	fseek(in_file, 0L, SEEK_END);
	num_bytes = ftell(in_file);

	// Get the number of bytes
	fseek(in_file, 0L, SEEK_END);
	num_bytes = ftell(in_file);

	// reset the file position indicator to the beginning of the file
	fseek(in_file, 0L, SEEK_SET);

	// grab sufficient memory for the buffer to hold the text
	buffer = (char*)calloc(num_bytes, sizeof(char));

	// memory error
	if (buffer == NULL)
		return NULL;

	// copy all the text into the buffer
	fread(buffer, sizeof(char), num_bytes, in_file);
	fclose(in_file);

	return buffer;
}

void add_each_line_to_dict(SpellCheckerDictionaryHandle dict, const char* file_path)
{
	FILE* file;
	char buffer[100];

	file = fopen(file_path, "r");
	assert(file != NULL);

	while (fgets(buffer, sizeof(buffer), file) != NULL)
	{
		// Overwrite the newline from the buffer with a null char so it doesn't get added to the word
		buffer[strlen(buffer) - 1] = '\0';
		int addResult = spellCheckerAddWord(dict, buffer);

		if (addResult != 0)
		{
			// Out of memory or bad input line.
			assert(false);
		}
	}
}

//
// Snippet from StackOverflow: http://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows
//

//  Windows
#ifdef _WIN32
#include <Windows.h>
double get_wall_time() {
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)) {
		//  Handle error
		return 0;
	}
	if (!QueryPerformanceCounter(&time)) {
		//  Handle error
		return 0;
	}
	return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time() {
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}
	else {
		//  Handle error
		return 0;
	}
}

//  Posix/Linux
#else
#include <sys/time.h>
double get_wall_time() {
	struct timeval time;
	if (gettimeofday(&time, NULL)) {
		//  Handle error
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time() {
	return (double)clock() / CLOCKS_PER_SEC;
}
#endif
