// BinaryTrees.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <iostream>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;

struct Node {
	shared_ptr<Node> left, right;
	int value;
	Node() : value(0) {}
	Node(int v) : value(v) {}
	Node(shared_ptr<Node> left, shared_ptr<Node> right) : value(0), left(left), right(right) { }
};

const int UNBALANCED = -1;
using std::abs;
using std::fmax;

int is_balanced(const shared_ptr<Node>& root) {
	if (!root) return 0;
	int left = is_balanced(root->left);
	int right = is_balanced(root->right);
	if (left == UNBALANCED || right == UNBALANCED || abs(left-right) > 1)
		return UNBALANCED;

	return fmax(left, right) + 1;
}

bool is_balanced_starter(const shared_ptr<Node>& root) {
	int balance = is_balanced(root);
	return (balance != UNBALANCED);
}

using std::cout;
using std::to_string;

void test_balanced() {
	
	auto N_E = make_shared<Node>();
	auto N_F = make_shared<Node>();
	auto N_D = make_shared<Node>(N_E, N_F);
	
	auto N_G = make_shared<Node>();
	auto N_C = make_shared<Node>(N_D, N_G);

	auto N_I = make_shared<Node>();
	auto N_J = make_shared<Node>();
	auto N_H = make_shared<Node>(N_I, N_J);

	auto N_B = make_shared<Node>(N_C, N_H);

	auto N_M = make_shared<Node>();
	auto N_N = make_shared<Node>();
	auto N_L = make_shared<Node>(N_M, N_N);

	auto N_O = make_shared<Node>();
	auto N_K = make_shared<Node>(N_L, N_O);

	auto N_A_root = make_shared<Node>(N_B, N_K);

	auto N_Unbalance = make_shared<Node>();
	// Uncomment to unbalance the tree
	//	N_E->left = N_Unbalance;


	cout << "Is balanced?" << to_string(is_balanced_starter(N_A_root)) << "\n";
}



int _tmain(int argc, _TCHAR* argv[])
{
	test_balanced();

	return 0;
}

