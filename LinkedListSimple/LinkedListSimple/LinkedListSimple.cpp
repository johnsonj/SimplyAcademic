// LinkedListSimple.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestSuite.h"
#include<memory>

#define SENTINAL_VALUE -1

/*
struct Node {
	Node* next;
	int val;
	Node() : next(nullptr), val(SENTINAL_VALUE) {}
	Node(int val) : next(nullptr), val(val) {}
};

Node* merge_broken(Node* A, Node* B) {
	if (A == nullptr || B == nullptr) return A;
	if (A->val != SENTINAL_VALUE || B->val != SENTINAL_VALUE) return A;

	Node* merged_list = A;

	while (B->next != nullptr) {
		if (A->next == nullptr) {
			A->next = B;
			break;
		}
		else if (A->next->val > B->next->val) {
			// We need to mere the node B->next into the A list
			Node* old_next = A->next->next;
			A->next->next = B->next;
			B = B->next;
			A->next->next->next = old_next;
		}
		else {
			// A is smaller than the smallest node of B so keep going
			A = A->next;
		}
	}

	return merged_list;
}

Node *merge(Node* A, Node*B) {
	Node * new_list = nullptr;
	Node * smaller = nullptr;
	Node * new_list_start = nullptr;
	while (A && B) {
		if (A->val < B->val) {
			smaller = A;
			A = A->next;
		}
		else {
			smaller = B;
			B = B->next;
		}
		if (new_list) {
			new_list->next = smaller;
			new_list = new_list->next;
		}
		else {
			new_list = new_list_start = smaller;
		}
	}
	if (A) {
		new_list->next = A;
	}
	else {
		new_list->next = B;
	}

	return new_list_start;
}

void print_list(const Node& list) {
	Node* ptr = list.next;
	while (ptr) {
		std::cout << std::to_string(ptr->val) << " ";
		ptr = ptr->next;
	}
	std::cout << "\n";
}

void test_merge_C_style() {
	Node A_1(1);
	Node A_3(3);
	Node A_4(4);
	
	Node B_2(2);
	Node B_5(5);
	Node B_6(6);

	A_1.next = &A_3;
	A_3.next = &A_4;

	B_2.next = &B_5;
	B_5.next = &B_6;

	Node* merged_low_a = merge(&A_1, &B_2);

	print_list(*merged_low_a);
}
*/

using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;

struct Node {
	shared_ptr<Node> next;
	int val;
	Node(int val) : val(val) {}
	Node(int val, shared_ptr<Node> next) : val(val), next(next) {}
};

void print_list(shared_ptr<const Node> list) {
	while (list) {
		std::cout << std::to_string(list->val) << " ";
		list = list->next;
	}
	std::cout << "\n";
}

void insert_and_advance(shared_ptr<Node> &N, shared_ptr<Node> &list) {
	if (list) {
		list->next = N;
		list = list->next;
	}
	else {
		list = N;
	}
	N = N->next;
}
shared_ptr<Node> merge_cpp(shared_ptr<Node> A, shared_ptr<Node> B) {
	// If one or both of the lists are empty, we can't merge it
	// Atempt to the one with elements. Will be empty if both blank.
	if (!A || !B)
		return A ? A : B;

	shared_ptr<Node> new_list;
	shared_ptr<Node> new_list_head;
	while (A && B) {
		insert_and_advance(A->val < B->val ? A : B, new_list);

		if (!new_list_head)
			new_list_head = new_list;
	}
	// Put the remaining items on the list
	insert_and_advance(A ? A : B, new_list);

	return new_list_head;
}

void test_merge() {
	auto A_4 = make_shared<Node>(4);
	auto A_3 = make_shared<Node>(3, A_4);
	auto A_1 = make_shared<Node>(1, A_3);

	auto B_6 = make_shared<Node>(6);
	auto B_5 = make_shared<Node>(5, B_6);
	auto B_2 = make_shared<Node>(2, B_5);

	auto sorted_list = merge_cpp(B_2, A_1);

	print_list(sorted_list);
	print_list(sorted_list);
}

void reverse_list(shared_ptr<Node>& list) {
	shared_ptr<Node> new_list;
	shared_ptr<Node> tmp;
	while (list) {
		tmp = list->next;
		list->next = new_list;
		new_list = list;
		list = tmp;
	}
	list = new_list;
}




void test_reverse() {
	auto A_4 = make_shared<Node>(4);
	auto A_3 = make_shared<Node>(3, A_4);
	auto A_1 = make_shared<Node>(1, A_3);

	auto B_6 = make_shared<Node>(6);
	auto B_5 = make_shared<Node>(5, B_6);
	auto B_2 = make_shared<Node>(2, B_5);

	auto sorted_list = merge_cpp(B_2, A_1);

	print_list(sorted_list);

	reverse_list(sorted_list);

	print_list(sorted_list);
}

int _tmain(int argc, _TCHAR* argv[])
{

//	test_merge();
	test_reverse();
	return 0;
}

