// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LINKEDLIST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LINKEDLIST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LINKEDLIST_EXPORTS
#define LINKEDLIST_API __declspec(dllexport)
#else
#define LINKEDLIST_API __declspec(dllimport)
#endif

#include <memory>
#define DEFAULT_NODE_ID 0
typedef LINKEDLIST_API int NodeID;

class LINKEDLIST_API Node {
public:
	NodeID GetID() const { return id; }
	const Node* GetNext() const { return next.get(); }
	const Node* GetLink() const { return link; }

protected:
	int id;
	std::shared_ptr<Node> next;
	Node* link;

	Node() = delete; // Default constructor not supported, needs an ID at least
	Node(NodeID id) : id(id), next(nullptr), link(nullptr) {}
	Node(NodeID id, std::shared_ptr<Node> next) : id(id), next(next), link(nullptr) {}

	static int NextID(NodeID last_id) {
		return last_id + 1;
	}

	friend class LinkedList;

	// Required for make_shared to call Node(...) Likely functional in VS2013
	friend class std::_Ref_count_obj < Node > ;
};

class LINKEDLIST_API LinkedList {
public:
	LinkedList() : size(0), head(nullptr), tail(nullptr), last_inserted_node(nullptr) {}

	// Move/Copy
	LinkedList(const LinkedList& ll);
	LinkedList& operator=(const LinkedList& ll);
	
	bool operator==(const LinkedList& ll) const;

	void InsertAtHead();

	int GetSize() const { return size; }
	const Node* GetHead() const { return head.get(); }
	const Node* GetTail() const { return tail.get(); }
	const Node* FindNode(NodeID id) const;

	void Link(NodeID from, NodeID to);
private:
	std::shared_ptr<Node> find_mutable_node(NodeID id) const;
	int size;
	NodeID last_id() const;

	std::shared_ptr<Node> head;
	std::shared_ptr<Node> tail;
	std::shared_ptr<Node> last_inserted_node;

	void deep_copy(const LinkedList& ll);
};