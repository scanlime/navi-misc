/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * lazy_cache.h -- An LRU cache which services cache misses asynchronously,
 *                 generating the results on a background thread.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __LAZY_CACHE_H
#define __LAZY_CACHE_H

#include <wx/thread.h>
#include <boost/unordered_set.hpp>
#include <list>

#include "lru_cache.h"


// An exception to throw on cache miss.
struct LazyCacheMiss {};


/*
 * The WorkQueue is a bounded set of keys which are sorted by
 * insertion order. When a duplicate key is inserted, that key is
 * moved to the front of the queue as if it was just inserted for the
 * first time. When the queue fills up, the oldest items are
 * overwritten.
 */

template <typename Key>
class WorkQueue
{
public:
    WorkQueue(int _size)
        : size(_size),
          itemCount(0),
          keys(new Key[_size]),
          slots(_size)
    {}

    ~WorkQueue()
    {
        delete[] keys;
    }

    void insert(Key &k)
    {
        if (map.find(k) == map.end()) {
            // Inserting 'k' for the first time.

            // Oldest slot (may or may not be occupied)
            int index = slots.head;

            if (itemCount == size) {
                // Remove oldest item from the map
                map.erase(keys[index]);
            } else {
                // Oldest slot was empty
                itemCount++;
            }

            // Make this the newest item
            slots.moveToTail(index);

            // Remember the item's current slot
            map[k] = index;
            keys[index] = k;

        } else {
            // 'k' is already in the list. Bump its priority

            slots.moveToTail(map[k]);
        }
    }

    bool empty()
    {
        return itemCount == 0;
    }

    Key &oldest()
    {
        return keys[slots.head];
    }

    Key &newest()
    {
        return keys[slots.tail];
    }

    void removeOldest()
    {
        map.erase(keys[slots.head]);
        itemCount--;
    }

    void removeNewest()
    {
        // Make it the oldest, then remove it. This way we'll recycle the node next.
        slots.moveToHead(slots.tail);
        removeOldest();
    }

private:
    typedef boost::unordered_map<Key, int> map_t;

    int size;
    int itemCount;
    Key *keys;
    SlotList<> slots;
    map_t map;
};


template <typename Key, typename Value>
class LazyCache : public LRUCache<Key, Value>
{
public:
    typedef CacheGenerator<Key, Value> generator_t;

    LazyCache(int _size, generator_t *_generator)
        : LRUCache<Key, Value>(_size, _generator),
          workQueue(_size)
    {
        thread = new Thread(this);
        thread->Create();
        thread->Run();
    }

    ~LazyCache()
    {
        thread->Delete();
    }

    // Throws LazyCacheMiss on miss
    Value &get(Key k)
    {
        wxCriticalSectionLocker locker(lock);
        int index;

        if (find(k, index)) {
            return LRUCache<Key, Value>::retrieve(index);
        } else {
            workQueue.insert(k);
            thread->wake();
            throw LazyCacheMiss();
        }
    }

private:

    class Thread : public wxThread
    {
    public:
        Thread(LazyCache<Key, Value> *_cache)
            : cache(_cache),
              mutex(),
              condvar(mutex)
        {}

        virtual ExitCode Entry()
        {
            while (!TestDestroy()) {
                condvar.WaitTimeout(1000);
                while (processWorkQueue());
            }
        }

        void wake()
        {
            condvar.Signal();
        }

    private:

        // Returns true if there is more work, false if the queue is empty.
        bool processWorkQueue()
        {
            /*
             * First critical section: Extract a work item.
             *
             * Currently we're extracting the most *recently* added
             * item, in order to improve interactive responsiveness.
             */

            cache->lock.Enter();

            if (cache->workQueue.empty()) {
                cache->lock.Leave();
                return false;
            }

            Key k = cache->workQueue.newest();
            cache->workQueue.removeNewest();

            int index;
            if (cache->find(k, index)) {
                // Duplicate work item
                cache->lock.Leave();
                return true;
            }

            // Allocate a spot for the result
            Value &v = cache->alloc(index);

            cache->lock.Leave();

            /*
             * Do the work, without holding any locks.
             * The value we're writing to has been reserved.
             */

            cache->generator->fn(k, v);

            /*
             * Second critical section: Store the result.
             */

            cache->lock.Enter();
            cache->store(k, index);
            cache->lock.Leave();

            return true;
        }

        wxMutex mutex;
        wxCondition condvar;
        LazyCache<Key, Value> *cache;
    };

    Thread *thread;
    wxCriticalSection lock;
    WorkQueue<Key> workQueue;
};

#endif /* __LAZY_CACHE_H */
