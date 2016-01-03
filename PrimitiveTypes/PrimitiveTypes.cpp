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

template<int R, int C>
std::vector<int> spiral_fsm(int (A)[R][C], int width, int height) {
	using std::cout;
	using std::vector;

	int top = 0;
	int bottom = height - 1;
	int left = 0;
	int right = width - 1;
	
	int row = 0;
	int column = 0;

	enum STATES { LEFT, RIGHT, UP, DOWN };
	STATES state = RIGHT;

	vector<int> result;

	for (int i = 0; i <= (width*height) + 2*(width-1) + 1; i++) {
		if (state == RIGHT) {
			if (column == right) {
				state = DOWN;
				top += 1;
			}
			else {
				result.emplace_back(A[row][column]);
				column += 1;
			}
		}
		else if (state == DOWN) {
			if (row == bottom) {
				state = LEFT;
				right -= 1;
			}
			else {
				result.emplace_back(A[row][column]);
				row += 1;
			}
		}
		else if (state == LEFT) {
			if (column == left) {
				state = UP;
				bottom -= 1;
			}
			else {
				result.emplace_back(A[row][column]);
				column -= 1;
			}
		}
		else if (state == UP) {
			if (row == top) {
				state = RIGHT; 
				left += 1;
			}
			else {
				result.emplace_back(A[row][column]);
				row -= 1;
			}
		}
	}

	return result;
};

template<int RC>
void spiral_rings(const int (A)[RC][RC]) {
	for (int i = 0; i < (ceil((RC)/2)); i++) {
		print_single_ring<RC>(A, i);
	}

	// For odd sized matricies we need to print the center element
	// We can't print a ring of a single element 
	if (RC % 2 == 1) {
		std::cout << std::to_string(A[(RC-1) / 2][(RC-1) / 2]);
	}
	std::cout << "\n";
};

template<int RC>
void print_single_ring(const int(A)[RC][RC], int offset) {
	using std::cout;
	using std::to_string;

	if (RC % 2 == 1 && floor(RC / 2) == offset - 1) {
		cout << to_string(A[offset][offset]) << " ";
		return;
	}

	// Right
	// A[offset][offset] -> A[offset][ RC-offset - 1 ]
	for (int j = offset; j < (RC - offset - 1); j++) {
		cout << to_string(A[offset][j]) << " ";
	}
	// Down
	// A[offset][RC-offset] -> A[ RC-offset-1 ][RC-offset]
	for (int i = offset; i < (RC - offset - 1); i++) {
		cout << to_string(A[i][RC - offset - 1]) << " ";
	}

	// Left
	// A[RC-offset][RC-offset] -> A[RC-offset][ offset - 1 ]
	for (int j = RC - offset - 1; j > offset; j--) {
		cout << to_string(A[RC - offset - 1][j]) << " ";
	}

	// Up
	// A[RC-offset][offset] -> A[offset -1][offset]
	for (int i = RC - offset - 1; i > offset; i--) {
		cout << to_string(A[i][offset]) << " ";
	}
};



void spiral() {

	using std::vector;
	using std::cout;

	int spiral_3x3[3][3] = { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 } };

	spiral_rings<3>(spiral_3x3);
	
	cout << "\n";

	int spiral_4x4[4][4] = { { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 }, { 13, 14, 15, 16 } };
	spiral_rings<4>(spiral_4x4);

	cout << "\n";

	int spiral_5x5[5][5] = { { 1, 2, 3, 4, 5 }, { 6, 7, 8, 9, 10 }, { 11, 12, 13, 14, 15 }, { 16, 17, 18, 19, 20 }, { 21, 22, 23, 24, 25 } };
	spiral_rings<5>(spiral_5x5);

	cout << "\n";
	
	/*	
vector<int> spiral_3x3_sln{ 0, 1, 2, 5, 8, 7, 6, 3, 4 };

	TestSuite<vector<int>, int[3][3], int, int> test_3x3("3x3");
	test_3x3.AddTestCase(spiral_3x3_sln, spiral_3x3, 3, 3);
	test_3x3.AddSolution("State Macine", spiral_fsm<3,3>);

	test_3x3.Execute();
	*/
	///TestSuite<
}

int _tmain(int argc, _TCHAR* argv[])
{
	spiral();
	char f;
	std::cin >> f;
}

