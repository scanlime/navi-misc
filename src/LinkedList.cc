#include "tvbuggy.h"

LinkedList::LinkedList()
	: firstNode(NULL), lastNode(NULL), count(0)
{
}

LinkedList::~LinkedList()
{
	Node *node, *nextNode = NULL;

	for (node = firstNode; nextNode != NULL; node = nextNode)
	{
		nextNode = node->getNext();

		delete node;
	}
}

void
LinkedList::add(Node *node)
{
	if (node == NULL)
		return;

	if (firstNode == NULL)
		firstNode = node;

	node->setPrev(lastNode);

	if (lastNode != NULL)
		lastNode->setNext(node);

	lastNode = node;

	count++;
}

void
LinkedList::addFront(Node *node)
{
	if (node == NULL)
		return;

	if (firstNode != NULL)
	{
		firstNode->setPrev(node);
		node->setNext(firstNode);
	}
	else
		lastNode = node;

	firstNode = node;

	count++;
}

void
LinkedList::remove(Node *node)
{
	if (node == NULL)
		return;

	if (node->getPrev() == NULL)
		firstNode = node->getNext();
	else
		node->getPrev()->setNext(node->getNext());

	if (node->getNext() == NULL)
		lastNode = node->getPrev();
	else
		node->getNext()->setPrev(node->getPrev());

	count--;
}

Node *
LinkedList::getNodes() const
{
	return firstNode;
}

int
LinkedList::getCount() const
{
	return count;
}

