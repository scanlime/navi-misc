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
          lru(new LRUNode[_size]),
          generator(_generator),
          head(LRU_NIL),
          tail(LRU_NIL)
    {
        // Link the LRU list together in arbitrary order.
        for (int i = 0; i < size; i++) {
            LRUInsertTail(i);
        }
    }

    ~LRUCache() {
        delete[] values;
        delete[] keys;
        delete[] lru;
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
        if (map.find(k) == map.end()) {
            return false;
        } else {
            index = map.at(k);
            return true;
        }
    }

    // Allocate a fresh Value to fill in, freeing the oldest Value.
    Value &alloc(int &index) {
        index = head;
        map.erase(keys[index]);
        LRURemove(index);
        return values[index];
    }

    // After a value has been written to 'index', make it available to find.
    void store(Key k, int index) {
        map[k] = index;
        keys[index] = k;
        LRUInsertTail(index);
    }

    Value& retrieve(int index) {
        LRURemove(index);
        LRUInsertTail(index);
        return values[index];
    }

    generator_t *generator;

private:
    static const int LRU_NIL = -1;

    typedef boost::unordered_map<Key, int> map_t;

    struct LRUNode {
        int prev, next;
    };

    void LRURemove(int i) {
        if (lru[i].prev == LRU_NIL)
            head = lru[i].next;
        else
            lru[lru[i].prev].next = lru[i].next;

        if (lru[i].next == LRU_NIL)
            tail = lru[i].prev;
        else
            lru[lru[i].next].prev = lru[i].prev;
    }

    void LRUInsertTail(int i) {
        lru[i].next = LRU_NIL;
        lru[i].prev = tail;

        if (tail == LRU_NIL)
            head = tail = i;
        else
            lru[tail].next = i;

        tail = i;
    }

    int size;
    int head, tail;
    Value *values;
    Key *keys;
    LRUNode *lru;
    map_t map;
};

#endif /* __LRU_CACHE_H */
