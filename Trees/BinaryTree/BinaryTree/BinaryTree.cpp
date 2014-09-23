// BinaryTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include <iostream>

using std::vector;

struct Node {
	int value;
	Node* left;
	Node* right;
	Node() : left(nullptr), right(nullptr), value(-1) {}
	Node(int v) : left(nullptr), right(nullptr), value(v) {}
};

/*
 Write a function to return all of the nodes from a given node and a given distance. 
 The function has three parameter root, the given node and the given distance. For example

     5
     /\
    4  7
       /\
      6  10
       \
        12

 F(root,7,1) -> 6, 10, 5
 F(root,7,2) -> 12, 4

 */
void FindNodes(Node* start, int target_level, int cur_level, vector<int>& nodes) {
	if (!start) return;
	if (target_level == cur_level) {
		nodes.emplace_back(start->value);
		return;
	}
	FindNodes(start->left, target_level, cur_level + 1, nodes);
	FindNodes(start->right, target_level, cur_level + 1, nodes);
}
int FindNodeLevel(Node* root, int key, int level = 0) {
	if (!root) return -1;
	if (root->value == key) return level;

	return std::max(FindNodeLevel(root->left, key, level + 1), FindNodeLevel(root->right, key, level + 1));
}
vector<int> F(Node* root, int key, int distance) {
	vector<int> result;
	if (!root || key < 0 || distance <= 0) return result;

	int target_level = FindNodeLevel(root, key);
	if (target_level == -1) return result;

	// Find nodes above 
	FindNodes(root, target_level + distance, 0, result); // Below
	FindNodes(root, std::abs(target_level - distance), 0, result); // Above

	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{

	Node* root = new Node(5);
	root->left = new Node(4);
	Node* n_7 = root->right = new Node(7);
	n_7->right = new Node(10);
	Node* n_6 = n_7->left = new Node(6);
	n_6->right = new Node(12);

	std::cout << "Distance 1 from 7: ";
	for (auto v : F(root, 7, 1)) {
		std::cout << v << " ";
	}
	std::cout << "\n";

	std::cout << "Distance 2 from 7: ";
	for (auto v : F(root, 7, 2)) {
		std::cout << v << " ";
	}
	std::cout << "\n";

	char foo;
	std::cin >> foo;
	
	return 0;
}

