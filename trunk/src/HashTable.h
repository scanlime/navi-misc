#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "LinkedList.h"

class Bucket : public Node
{
	public:
		Bucket(const char *key, void *data);
		~Bucket();

		void setData(void *data)   { this->data = data; }
		
		const char *getKey() const { return key;  }
		void *getData() const      { return data; }

	private:
		const char *key;
		void *data;
};

class HashTable
{
	public:
		HashTable(unsigned short bucketCount = 23);
		~HashTable();

		void add(const char *key, void *data);
		void remove(const char *key);

		void *get(const char *key) const;

		int getCount() const;

	protected:
		int compare(const char *key1, const char *key2) const;
		unsigned short hashkey(const char *key) const;

	private:
		LinkedList *buckets;
		unsigned short bucketCount;
		unsigned short itemCount;
};

#endif /* _HASHTABLE_H_ */
