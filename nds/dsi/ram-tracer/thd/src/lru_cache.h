/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * lru_cache.h -- A simple object cache, with LRU replacement.
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

#ifndef __LRU_CACHE_H
#define __LRU_CACHE_H

#include <boost/unordered_map.hpp>
#include <vector>


/*
 * An auxiliary class which defines a 'generator' function which knows
 * how to generate a cache value when a miss occurs.
 */

template <typename Key, typename Value>
struct CacheGenerator {
    virtual void fn(Key &key, Value &value) = 0;
};


/*
 * This is a utility class which represents an ordered list of integer
 * slots. It can be used as a fast linked list which can provide an
 * ordering for an external array, without any extra memory allocation
 * overhead.
 *
 * By default, the SlotList contains all items from 0..size-1.
 */

template <typename tn = int, tn nilValue = -1>
class SlotList {
public:
    static const tn NIL = nilValue;

    SlotList(tn size)
        : nodes(new node_t[size]),
          head(NIL),
          tail(NIL)
    {
        for (tn i = 0; i < size; i++)
            append(i);
    }

    ~SlotList()
    {
        delete[] nodes;
    }

    void remove(tn i)
    {
        if (nodes[i].prev == NIL)
            head = nodes[i].next;
        else
            nodes[nodes[i].prev].next = nodes[i].next;

        if (nodes[i].next == NIL)
            tail = nodes[i].prev;
        else
            nodes[nodes[i].next].prev = nodes[i].prev;
    }

    void append(tn i)
    {
        nodes[i].next = NIL;
        nodes[i].prev = tail;

        if (tail == NIL)
            head = tail = i;
        else
            nodes[tail].next = i;

        tail = i;
    }

    void prepend(tn i)
    {
        nodes[i].next = head;
        nodes[i].prev = NIL;

        if (head == NIL)
            head = tail = i;
        else
            nodes[head].prev = i;

        head = i;
    }

    void moveToHead(tn i)
    {
        remove(i);
        prepend(i);
    }

    void moveToTail(tn i)
    {
        remove(i);
        append(i);
    }

    tn head, tail;

private:
    struct node_t {
        tn prev, next;
    };
    node_t *nodes;
};


/*
 * Creates a fixed-size cache which maps Key to Value, storing 'size'
 * values. When a value is missing, we generate it using the provided
 * 'generator' class.
 */

template <typename Key, typename Value>
class LRUCache {
public:
    typedef CacheGenerator<Key, Value> generator_t;

    LRUCache(int _size, generator_t *_generator)
        : size(_size),
          values(new Value[_size]),
          keys(new Key[_size]),
          lru(_size),
          generator(_generator)
    {}

    ~LRUCache() {
        delete[] values;
        delete[] keys;
    }

    Value& get(Key k) {
        int index;

        if (find(k, index)) {
            return retrieve(index);
        } else {
            Value &v = alloc(index);
            generator->fn(k, v);
            store(k, index);
            return v;
        }
    }

protected:
    bool find(Key k, int &index) {
        iterator_t i = map.find(k);
        if (i == map.end()) {
            return false;
        } else {
            index = i->second;
            return true;
        }
    }

    // Allocate a fresh Value to fill in, freeing the oldest Value.
    Value &alloc(int &index) {
        index = lru.head;
        map.erase(keys[index]);
        lru.remove(index);
        return values[index];
    }

    // After a value has been written to 'index', make it available to find.
    void store(Key k, int index) {
        map[k] = index;
        keys[index] = k;
        lru.append(index);
    }

    Value& retrieve(int index) {
        lru.moveToTail(index);
        return values[index];
    }

    generator_t *generator;

private:
    typedef typename boost::unordered_map<Key, int> map_t;
    typedef typename boost::unordered_map<Key, int>::iterator iterator_t;

    int size;
    Value *values;
    Key *keys;
    SlotList<> lru;
    map_t map;
};


/*
 * A specialized LRU cache that uses an ordered map rather than a hash
 * table. Queries can look for the closest cached item to a specified
 * key.
 */

template <typename Key, typename Value>
class FuzzyCache {
public:
    FuzzyCache(int _size, Value _defaultValue)
        : lru(_size),
          defaultValue(_defaultValue)
    {}

    static Key distance(Key a, Key b)
    {
        if (a >= b)
            return a - b;
        else
            return b - a;
    }

    Value &findClosest(Key k)
    {
        iterator_t below = cacheMap.upper_bound(k);
        iterator_t above = cacheMap.lower_bound(k);
        bool belowExists = below != cacheMap.end();
        bool aboveExists = above != cacheMap.end();

        if (belowExists && (!aboveExists || (k - below->first <= above->first - k))) {
            touch(below->first);
            return below->second;
        }

        if (aboveExists && (!belowExists || (k - below->first >= above->first - k))) {
            touch(above->first);
            return above->second;
        }

        // Map is empty. Return a blank instant.
        return defaultValue;
    }

    void store(Key &k, Value &v)
    {
        // Recycle the oldest slot
        int slot = lru.head;
        lru.moveToTail(slot);

        // Free the old item, if any
        slotMapIter_t slotIter = slotMap.find(slot);
        if (slotIter != slotMap.end()) {
            Key oldKey = slotIter->second;
            slotMap.erase(slotIter);
            keyMap.erase(oldKey);
            cacheMap.erase(oldKey);
        }

        // Insert the new item
        cacheMap.insert(value_t(k, v));
        slotMap.insert(slotMapValue_t(slot, k));
        keyMap.insert(keyMapValue_t(k, slot));
    }

private:
    void touch(Key k)
    {
        // Mark a key as most recently used
        lru.moveToTail(keyMap.at(k));
    }

    typedef typename std::map<Key, Value> map_t;
    typedef typename std::map<Key, Value>::iterator iterator_t;
    typedef typename std::map<Key, Value>::value_type value_t;

    typedef typename std::map<int, Key> slotMap_t;
    typedef typename std::map<int, Key>::iterator slotMapIter_t;
    typedef typename std::map<int, Key>::value_type slotMapValue_t;

    typedef typename std::map<Key, int> keyMap_t;
    typedef typename std::map<Key, int>::iterator keyMapIter_t;
    typedef typename std::map<Key, int>::value_type keyMapValue_t;

    Value defaultValue;
    map_t cacheMap;
    slotMap_t slotMap;
    keyMap_t keyMap;
    SlotList<> lru;
};


#endif /* __LRU_CACHE_H */
