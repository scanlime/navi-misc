#include "List.h"

List::List()
	: firstItem(NULL), lastItem(NULL), itemCount(0)
{
}

List::~List()
{
	clear();
}

void
List::append(ListItem *item)
{
	if (firstItem == NULL)
		firstItem = item;

	if (lastItem != NULL)
	{
		lastItem->setNext(item);
		item->setPrev(lastItem);
	}

	lastItem = item;

	itemCount++;
}

void
List::append(void *data)
{
	append(new ListItem(data));
}

void
List::prepend(ListItem *item)
{
	if (lastItem == NULL)
		lastItem = item;

	if (firstItem != NULL)
	{
		firstItem->setPrev(item);
		item->setNext(firstItem);
	}

	firstItem = item;

	itemCount++;
}

void
List::prepend(void *data)
{
	prepend(new ListItem(data));
}

void
List::remove(ListItem *item)
{
	if (item == firstItem)
		firstItem = item->getNext();

	if (item == lastItem)
		lastItem = item->getPrev();

	if (item->getPrev() != NULL)
		item->getPrev()->setNext(item->getNext());

	if (item->getNext() != NULL)
		item->getNext()->setPrev(item->getPrev());

	itemCount--;
}

void
List::clear()
{
	ListItem *item, *nextItem;

	for (item = firstItem; item != NULL; item = nextItem)
	{
		nextItem = item->getNext();

		delete item;
	}

	firstItem = NULL;
	lastItem = NULL;
	itemCount = 0;
}

ListItem *
List::getFirstItem() const
{
	return firstItem;
}

ListItem *
List::getLastItem() const
{
	return lastItem;
}

size_t
List::getItemCount() const
{
	return itemCount;
}


ListItem::ListItem(void *data)
	: data(data), prev(NULL), next(NULL)
{
}

ListItem::~ListItem()
{
}

void
ListItem::setPrev(ListItem *prev)
{
	this->prev = prev;
}

void
ListItem::setNext(ListItem *next)
{
	this->next = next;
}

void
ListItem::setData(void *data)
{
	this->data = data;
}

ListItem *
ListItem::getPrev() const
{
	return prev;
}

ListItem *
ListItem::getNext() const
{
	return next;
}

void *
ListItem::getData() const
{
	return data;
}
