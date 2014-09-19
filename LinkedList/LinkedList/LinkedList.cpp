// LinkedList.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "LinkedList.h"
#include <map>


void LinkedList::InsertAtHead() {
	last_inserted_node = head = std::make_shared<Node>(Node::NextID(last_id()), head);
	size++;

	if (tail == nullptr)
		tail = head;
}

void LinkedList::Link(NodeID from_id, NodeID to_id) {
	std::shared_ptr<Node> from = find_mutable_node(from_id);
	std::shared_ptr<Node> to = find_mutable_node(to_id);

	if (from == nullptr || to == nullptr) {
		return;
	}

	from->link = to.get();
}

const Node* LinkedList::FindNode(NodeID id) const {
	return find_mutable_node(id).get();
}

std::shared_ptr<Node> LinkedList::find_mutable_node(NodeID id) const {
	if (GetHead() == nullptr) {
		return nullptr;
	}

	std::shared_ptr<Node> search = head;
	while (search.get() != nullptr) {
		if (search->GetID() == id)
			return search;

		search = search->next;
	}

	return nullptr;
}

NodeID LinkedList::last_id() const {
	if (last_inserted_node == nullptr)
		return DEFAULT_NODE_ID;
	else
		return last_inserted_node->GetID();
}

LinkedList::LinkedList(const LinkedList& ll) : 
			size(0), head(nullptr), tail(nullptr), last_inserted_node(nullptr) {

	if (ll.size == 0)
		return;

	deep_copy(ll);
}

bool LinkedList::operator==(const LinkedList& ll) const {
	if (ll.GetSize() != ll.GetSize())
		return false;

	Node* cur = head.get();
	Node* ll_cur = ll.head.get();
	while (cur || ll_cur) {
		// Someone is longer than the other!
		if (!cur || !ll_cur)
			return false;

		if (cur->GetID() != ll_cur->GetID())
			return false;

		// One has a link and the other does not
		if (cur->GetLink() || ll_cur->GetLink())
			if (!cur->GetLink() || !ll_cur->GetLink())
				return false;

		if (cur->GetLink() && cur->GetLink()->GetID() != ll_cur->GetLink()->GetID())
			return false;

		cur = cur->next.get();
		ll_cur = ll_cur->next.get();
	}

	return true;
}

void LinkedList::deep_copy(const LinkedList& ll) {
	if (head || tail)
		throw "Attempting to perform a deep copy on an initialized LinkedList";

	// First pass: Copy the pointers
	auto lookup = std::map<const Node*, Node*>();
	lookup[nullptr] = nullptr; // handle null links

	// Copy the first node
	const Node* ll_cur = ll.GetHead();
	std::shared_ptr<Node> cur = head = std::make_shared<Node>(ll.GetHead()->GetID());
	lookup[ll_cur] = head.get();

	while (ll_cur->next.get()) {
		cur->next = std::make_shared<Node>(ll_cur->next->id);
		lookup[ll_cur->next.get()] = cur->next.get();

		ll_cur = ll_cur->next.get();
		cur = cur->next;
	}

	// Second pass: Setup links
	ll_cur = ll.GetHead();
	cur = head;

	while (ll_cur) {
		cur->link = lookup[ll_cur->link];

		ll_cur = ll_cur->next.get();
		cur = cur->next;
	}

	size = ll.size;
}