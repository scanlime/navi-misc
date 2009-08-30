/*
 * usb_io/urbcache.h
 * 
 * URB cache class. 
 * 
 * Copyright (c) 2006 by Wolfgang Wieser ] wwieser (a) gmx <*> de [ 
 * 
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation. (See COPYING.GPL for details.)
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 */

#ifndef _INCLUDE_FX2PIPE_USBIO_URBCACHE_H_
#define _INCLUDE_FX2PIPE_USBIO_URBCACHE_H_ 1

#include "../oconfig.h"
#include "../lib/linkedlist.h"

#include <stdlib.h>


/**
 * \short User request block cache. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 * This is a generic memory chunk chache for equally-sized memory 
 * chunks. 
 * 
 * It has a size limit and get() and put() statements which use 
 * normal libc-malloc/free (de)allocation if limits are exceeded. 
 * 
 * Not thread-safe. Not "C++-safe". 
 */
class URBCache
{
	private:
		/// Max number of elements to store. 
		int stack_size;
		/// Index of next free element on stack; 0 if empty. 
		int top_idx;
		/// Stack of elements; array of size stack_size. 
		void **stack;
		/// Size of each element just for enforcing equal sizes. 
		/// This is only needed to enforce that get() and put() are 
		/// dealing with equally-sized memory chunks all the time...
		size_t elem_size;
		
		/// Slow path of the get() operation. 
		void *_GetAlloc();
		/// Internally used; see get(), put(). 
		void _size_oops();
		
	private:
		/// Do not use. 
		void operator=(const URBCache &);
		/// Do not use. 
		URBCache(const URBCache &);
		
	public:
		/**
		 * \short Create an URB cache. 
		 * 
		 * The cache will store at most max_elem URBs. 
		 * Each URB must have the size elem_size. 
		 */
		URBCache(int max_elem,size_t _elem_size);
		/// Destructor will free all elements in the free list. 
		~URBCache();
		
		/// Clear the cache freeing all elements. 
		void clear();
		
		/// Get a memory chunk of passed size which must match elem_size. 
		inline void *get(size_t size)
		{
			if(size>elem_size)  _size_oops();
			return(top_idx ? stack[--top_idx] : _GetAlloc());
		}
		
		/// Put back ("free") memory chunk; passed size must match again...
		inline void put(void *ptr,size_t size)
		{
			if(size>elem_size)  _size_oops();
			if(top_idx<stack_size)
			{  stack[top_idx++]=ptr;  }
			else
			{  ::free(ptr);  }
		}
};

#endif  /* _INCLUDE_FX2PIPE_USBIO_URBCACHE_H_ */
