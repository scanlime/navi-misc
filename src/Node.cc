#include "tvbuggy.h"

Node::Node()
	: prev(NULL), next(NULL)
{
}

void
Node::setPrev(Node *node)
{
	prev = node;
}

void
Node::setNext(Node *node)
{
	next = node;
}

Node *
Node::getPrev() const
{
	return prev;
}

Node *
Node::getNext() const
{
	return next;
}

