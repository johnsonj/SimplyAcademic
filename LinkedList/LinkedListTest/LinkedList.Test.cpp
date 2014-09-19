#include "stdafx.h"
#include "CppUnitTest.h"
#include "../LinkedList/LinkedList.h"
#include <string>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Node>(const Node* t) { 
				std::wstring msg = L"Node<ID: ";
				msg += std::to_wstring(t->GetID());
				msg += L", Link: ";
				if (t->GetLink()) {
					msg += std::to_wstring(t->GetLink()->GetID());
				}
				else {
					msg += L" NULL";
				}
				msg += L">";
				return msg;
			}
			template <> static std::wstring ToString<LinkedList>(const LinkedList& ll) {
				std::wstring msg = L"LinkedList<size: ";
				msg += std::to_wstring(ll.GetSize());
				msg += L", Nodes: [";
				
				const Node* cur = ll.GetHead();
				while (cur) {
					msg += ToString(cur);
					cur = cur->GetNext();
					if (cur)
						msg += L",";
				}

				msg += L">";
				return msg;
			}
		}
	}
}

	
TEST_CLASS(LinkedListTest)
{
public:
		
	TEST_METHOD(LinkedListConstruct)
	{
		LinkedList ll;
		Assert::IsNull(ll.GetHead(), L"Head is properly initialized", LINE_INFO());
		Assert::IsNull(ll.GetTail(), L"Tail is properly initialized", LINE_INFO());
		Assert::AreEqual(ll.GetSize(), 0, L"List is empty", LINE_INFO());
	}
	TEST_METHOD(InsertAtHead)
	{
		LinkedList ll;

		ll.InsertAtHead();
		Assert::AreEqual(ll.GetSize(), 1, L"Size increments with insert", LINE_INFO());
		Assert::IsNotNull(ll.GetHead(), L"Head node is created", LINE_INFO());
		Assert::AreEqual(ll.GetHead(), ll.GetTail(), L"Head and tail are the same", LINE_INFO());

		const Node* head = ll.GetHead();
		Assert::AreEqual(head->GetID(), 1, L"Initial node ID", LINE_INFO());

		ll.InsertAtHead();
		Assert::AreEqual(ll.GetSize(), 2, L"Size increments with insert", LINE_INFO());
		Assert::AreNotEqual(ll.GetHead(), ll.GetTail(), L"Head and tail are different nodes", LINE_INFO());
		Assert::AreEqual(ll.GetHead()->GetNext(), ll.GetTail(), L"Head is next to tail", LINE_INFO());
	}

	TEST_METHOD(FindNode) {
		LinkedList ll;
		const Node *n_1, *n_2, *n_3;

		Assert::IsNull(ll.FindNode(0));

		ll.InsertAtHead();
		n_1 = ll.GetHead();
		ll.InsertAtHead();
		n_2 = ll.GetHead();
		ll.InsertAtHead();
		n_3 = ll.GetHead();

		Assert::AreEqual(ll.FindNode(n_1->GetID()), n_1);
		Assert::AreEqual(ll.FindNode(n_2->GetID()), n_2);
		Assert::AreEqual(ll.FindNode(n_3->GetID()), n_3);
	}

	TEST_METHOD(LinkNodes)
	{
		LinkedList ll;
		NodeID nid_1, nid_2, nid_3;

		ll.InsertAtHead();
		nid_1 = ll.GetHead()->GetID();
		ll.InsertAtHead();
		nid_2 = ll.GetHead()->GetID();
		ll.InsertAtHead();
		nid_3 = ll.GetHead()->GetID();

		ll.Link(nid_1, nid_2);
		ll.Link(nid_2, nid_3);
		ll.Link(nid_3, nid_1);

		Assert::AreEqual(ll.FindNode(nid_1)->GetLink()->GetID(), nid_2);
		Assert::AreEqual(ll.FindNode(nid_2)->GetLink()->GetID(), nid_3);
		Assert::AreEqual(ll.FindNode(nid_3)->GetLink()->GetID(), nid_1);
	}

	TEST_METHOD(Equality)
	{
		LinkedList ll_1, ll_2;
		
		// This only works for integer based IDs.
		// TODO: Update to handle other ID generators

		NodeID nid_1, nid_2, nid_3;

		ll_1.InsertAtHead();
		nid_1 = ll_1.GetHead()->GetID();
		ll_1.InsertAtHead();
		nid_2 = ll_1.GetHead()->GetID();
		ll_1.InsertAtHead();
		nid_3 = ll_1.GetHead()->GetID();

		ll_1.Link(nid_1, nid_2);
		ll_1.Link(nid_2, nid_3);
		ll_1.Link(nid_3, nid_1);

		ll_2.InsertAtHead();
		nid_1 = ll_2.GetHead()->GetID();
		ll_2.InsertAtHead();
		nid_2 = ll_2.GetHead()->GetID();

		Assert::AreNotEqual(ll_1, ll_2, L"Equality of different sized lists", LINE_INFO());
		ll_2.InsertAtHead();
		nid_3 = ll_2.GetHead()->GetID();

		ll_2.Link(nid_1, nid_2);
		ll_2.Link(nid_2, nid_3);
		Assert::AreNotEqual(ll_1, ll_2, L"Equality lists with a missing link", LINE_INFO());

		ll_2.Link(nid_3, nid_1);
		Assert::AreEqual(ll_1, ll_2, L"Equality of equal lists", LINE_INFO());
	}
	
	TEST_METHOD(Copy)
	{
		LinkedList ll_1;
		NodeID nid_1, nid_2, nid_3;

		ll_1.InsertAtHead();
		nid_1 = ll_1.GetHead()->GetID();
		ll_1.InsertAtHead();
		nid_2 = ll_1.GetHead()->GetID();
		ll_1.InsertAtHead();
		nid_3 = ll_1.GetHead()->GetID();

		ll_1.Link(nid_1, nid_2);
		ll_1.Link(nid_2, nid_3);
		ll_1.Link(nid_3, nid_1);

		LinkedList ll_2(ll_1);

		Assert::AreEqual(ll_1, ll_2, L"Copy Constructor of LinkedList", LINE_INFO());
	}
};
