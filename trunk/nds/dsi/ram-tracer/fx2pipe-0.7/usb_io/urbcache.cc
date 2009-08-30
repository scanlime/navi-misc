/*
 * usb_io/urbcache.cc
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

#include "urbcache.h"

#include <assert.h>


void URBCache::_size_oops()
{
	assert(0);
}


void *URBCache::_GetAlloc()
{
	void *ptr=malloc(elem_size);
	if(!ptr)
	{  fprintf(stderr,"URB alloc failire\n");  abort();  }
	return(ptr);
}


void URBCache::clear()
{
	for(int i=0; i<top_idx; i++)
	{
		if(stack[i])  // <-- This should never fail!
		{  ::free(stack[i]);  }
	}
	top_idx=0;
}


URBCache::URBCache(int max_elem,size_t _elem_size) : 
	stack_size(max_elem),top_idx(0),stack(new void*[stack_size]),
	elem_size(_elem_size)
{
	// Nothing to do...
}

URBCache::~URBCache()
{
	clear();
	delete[] stack;
}
