#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "Node.h"

class LinkedList
{
	public:
		LinkedList();
		virtual ~LinkedList();

		void add(Node *node);
		void addFront(Node *node);
		void remove(Node *node);

		Node *getNodes() const;

		int getCount() const;
		
	private:
		Node *firstNode;
		Node *lastNode;

		int count;
};

#endif /* _LINKEDLIST_H_ */
