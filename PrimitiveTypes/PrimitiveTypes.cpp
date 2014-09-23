#include "stdafx.h"
#include<cmath>
#include<functional>
#include<iostream>
#include "TestSuite.h"

#define CHAR_BIT 8

/*******************************************
 * Parity of a integer value
 * Even parity: 1011 = 1, 1001 = 0
 ******************************************/

// This won't ever finish. sizeof(unsigned int) * CHAR_BIT is pretty big.
bool parity_naive(unsigned int value) {
	bool parity = 0;
	for (int i = 0; i < pow(2, sizeof(unsigned int) * CHAR_BIT); i++) {
		if (value & 1)
			parity = !parity;
		value = value >> 1;
	}

	return parity;
}

bool parity_optimized(unsigned int value) {
	bool parity = 0;

	for (int i = 0; i < pow(2, sizeof(unsigned int) * CHAR_BIT); i++) {
		// We're out of 1's to analyze here
		// Note: x & ~(x-1) returns the lowest 1 
		if (!(value & ~(value - 1)))
			break;

		if (value & 1)
			parity = !parity;
		value = value >> 1;
	}

	return parity;
}

bool parity_very_optimized(unsigned int value) {
	bool parity = 0;

	while (value) {
		unsigned int lowest_one = (value & ~(value - 1));
		
		while (lowest_one = lowest_one >> 1)
			value = value >> 1;

		if (value & 1)
			parity = !parity;

		value = value >> 1;
	}

	return parity;
}

// Warm up the cache and keep it internal.
// in a real scenario this would live outside this method	
const int lookup_length = 256;
const int lookup_size = 8;
const int lookup_mask = 255;

bool parity_lookup[lookup_length];

void parity_cached_warmup() {
	for (int i = 0; i < lookup_length; i++) {
		parity_lookup[i] = parity_optimized(i);
	}
}

bool parity_cached(unsigned int value) {
	bool parity = 0;
	for (int i = 0; i < pow(2, sizeof(unsigned int) * CHAR_BIT) / lookup_size; i++) {
		// We're out of 1's to analyze here
		// Note: x & ~(x-1) returns the lowest 1 
		if (!(value & ~(value - 1)))
			break;

		if (parity_lookup[value & lookup_mask])
			parity = !parity;

		value = value >> lookup_size;
	}

	return parity;
}

void parity()
{
	TestSuite<bool, unsigned int> runner;
	
	runner.AddTestCase(false, 0);
	runner.AddTestCase(true, 1);
	runner.AddTestCase(true, 11);
	runner.AddTestCase(true, 14);
	runner.AddTestCase(false, 10);

	runner.AddSolution("Optimized", parity_optimized);
	parity_cached_warmup();
	runner.AddSolution("Cached Results", parity_cached);
	runner.AddSolution("Very Optimized", parity_very_optimized);

	runner.Execute();
}

// TODO
void dutch_flag()
{
	using std::vector;
	struct {
		void operator()(vector<int>& values, int pivot_point) {

		}
	} sln;

	vector<int> simple { 1, 2, 3, 4, 5 };
	vector<int> simple_sln = simple;
	
	sln(simple, 3);


	
}


int _tmain(int argc, _TCHAR* argv[])
{
	// parity()

	char f;
	std::cin >> f;
}

