#ifndef _NODE_H_
#define _NODE_H_

class Node
{
	public:
		Node();
		virtual ~Node() {}

		void setPrev(Node *node);
		void setNext(Node *node);

		Node *getPrev() const;
		Node *getNext() const;

	private:
		Node *prev;
		Node *next;
};

#endif /* _NODE_H_ */
