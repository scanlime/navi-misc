#include "tvbuggy.h"

/**************************************************************************
 * Bucket
 **************************************************************************/
Bucket::Bucket(const char *_key, void *_data)
	: Node(), key(_key), data(_data)
{
}

Bucket::~Bucket()
{
	delete key;
}

/**************************************************************************
 * HashTable
 **************************************************************************/
HashTable::HashTable(unsigned short bucketCount)
{
	this->bucketCount = bucketCount;

	itemCount = 0;

	buckets = new LinkedList[bucketCount];
}

HashTable::~HashTable()
{
	delete [] buckets;
}

void
HashTable::add(const char *key, void *data)
{
	unsigned short index;
	Bucket *bucket;
	
	if (key == NULL || data == NULL)
		return;

	index = hashkey(key);

	if (buckets[index].getCount() > 0)
	{
		/* See if this already exists in this bucket. */
		for (bucket = (Bucket *)buckets[index].getNodes();
			 bucket != NULL;
			 bucket = (Bucket *)bucket->getNext())
		{
			if (compare(key, bucket->getKey()) == 0)
			{
				/* It already exists. Replace the value. */
				bucket->setData(data);
				return;
			}
		}
	}

	/* Create the bucket for this. */
	bucket = new Bucket(key, data);

	buckets[index].addFront(bucket);

	itemCount++;
}

void
HashTable::remove(const char *key)
{
	unsigned short index;
	Bucket *bucket;

	index = hashkey(key);

	for (bucket = (Bucket *)buckets[index].getNodes();
		 bucket != NULL;
		 bucket = (Bucket *)bucket->getNext())
	{
		if (compare(key, bucket->getKey()) == 0)
		{
			buckets[index].remove(bucket);

			itemCount--;

			return;
		}
	}
}

void *
HashTable::get(const char *key) const
{
	unsigned short index;
	Bucket *bucket;

	index = hashkey(key);

	for (bucket = (Bucket *)buckets[index].getNodes();
		 bucket != NULL;
		 bucket = (Bucket *)bucket->getNext())
	{
		if (compare(key, bucket->getKey()) == 0)
			return bucket->getData();
	}

	return NULL;
}

int
HashTable::compare(const char *key1, const char *key2) const
{
	return strcmp(key1, key2);
}

unsigned short
HashTable::hashkey(const char *key) const
{
	unsigned short index = 0;
	const char *c;

	for (c = key; *c != '\0'; c++)
		index += (unsigned short)*c;

	return index % bucketCount;
}

