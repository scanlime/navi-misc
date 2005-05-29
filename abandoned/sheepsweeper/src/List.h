#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

class ListItem;

class List
{
	public:
		List();
		~List();

		void append(ListItem *item);
		void append(void *data);

		void prepend(ListItem *item);
		void prepend(void *data);

		void remove(ListItem *item);
		void remove(void *data);

		void clear();

		ListItem *getFirstItem() const;
		ListItem *getLastItem() const;

		size_t getItemCount() const;

	private:
		ListItem *firstItem;
		ListItem *lastItem;
		size_t itemCount;
};

class ListItem
{
	public:
		ListItem(void *data);
		~ListItem();

		void setPrev(ListItem *prev);
		void setNext(ListItem *next);
		void setData(void *data);

		ListItem *getPrev() const;
		ListItem *getNext() const;
		void *getData() const;

	private:
		ListItem *prev, *next;
		void *data;
};

#endif /* _LIST_H_ */
