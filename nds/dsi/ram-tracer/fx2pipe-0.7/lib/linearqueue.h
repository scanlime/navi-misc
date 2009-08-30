/*
 * lib/linearqueue.h
 * 
 * Linear node storage queue template. 
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

#ifndef _TemplateLibrary_LinearQueue_H_
#define _TemplateLibrary_LinearQueue_H_ 1

#include "../oconfig.h"
#include "../lib/linkedlist.h"

#include <stdlib.h>
#include <assert.h>


template<typename T>struct TLDefaultOperators_CDT
{
	static const bool pdt=0;
	static inline size_t size()  {  return(sizeof(T));  }
	static inline T & ass(T &l,T const &r)  {  return(l=r);  }
	static inline void ini(T *p)  {  new(p) T();  }
	static inline void ini(T *p,T const &a)  {  new(p) T(a);  }
	static inline void clr(T *p)  {  p->~T();  }
};


/**
 * \short Liner node storage queue class template. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 */
template<typename T,typename _OP=TLDefaultOperators_CDT<T> >class TLLinearQueue
{
	public:
		/// Internally used chunk (node). Not C++-safe (LLB!)
		struct Chunk : LinkedListBase<Chunk>
		{
			// For GCC, we may use ent[0]. (zero-length array)
			char _ent[];  ///< Array of entries; dynamically allocated. 
			
			inline Chunk() : LinkedListBase<Chunk>() {}
			inline ~Chunk() {}
		};
	private:
		/// Array subscript. 
		inline T &_ent(Chunk *c,size_t idx) const
			{  return(*(T*)(c->_ent+sizeof(T)*idx));  }
		
	public:
		/**
		 * \short Constant iterator for linear queue. 
		 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
		 * 
		 * This is a constant iterator, i.e. an interator to iterate 
		 * through the list elements first-to-last or last-to-first 
		 * which does not allow the elements to be changed/added/removed. 
		 * 
		 * NOTE: Default direction (non-reversed) is first to last which is 
		 *       tail to head. 
		 * 
		 * See TLBTree::ConstIterator on a usage example of ConstIterator's. 
		 */
		class ConstIterator
		{
			private:
				const TLLinearQueue *lq;  ///< Queue we are iterating. 
				Chunk *chk;               ///< Current chunk in queue or NULL. 
				size_t idx;               ///< Current element in chunk chk. 
				
				/// Current element valid? (Or out of list / list empty.)
				inline bool _valid() const
					{  return(chk);  }
				/// Start iteration by assigning a queue. 
				inline void _assign(const TLLinearQueue &_lq,bool reverse)
				{
					lq=&_lq;
					if(/*unlikely*/(reverse))
					{  chk=lq->clist.last(); idx=lq->last_idx;  }
					else
					{  chk=lq->clist.first(); idx=lq->first_idx;  }
				}
				/// Return current element or NULL. 
				inline T *_CurrElem() const
					{  return(_valid() ? &(lq->_ent(chk,idx)) : NULL);  }
			public:
				/**
				 * \short Set up iterator. 
				 * 
				 * Iterator by default begins at tail and advances towards 
				 * head. In order to reverse the direction, set start_last=1. 
				 */
				inline ConstIterator(const TLLinearQueue &_lq,bool reverse=0)
					{  _assign(_lq,reverse);  }
				/// Copy a const iterator preserving the location. 
				inline ConstIterator(const ConstIterator &i) : lq(i.lq),
					chk(i.chk),idx(i.idx)  {}
				/// Destructor: A no-op. 
				inline ~ConstIterator()  {}
				
				/**
				 * \short Check if valid. 
				 * 
				 * Returns true if current element is valid; false if beyond 
				 * end of list or list empty. 
				 */
				inline operator bool() const
					{  return(_valid());  }
				
				/**
				 * \short Return current element or NULL if not valid. 
				 * 
				 * The unary "*" operator is used to access the current elemnt 
				 * during iteration. 
				 */
				inline const T *operator*() const
					{  return(_CurrElem());  }
				
				/// Like unary operator*(): Access to current element. 
				inline const T *operator->() const
					{  return(_CurrElem());  }
				
				/**
				 * \short Return non-const pointer to current element. 
				 * 
				 * This is like operator() but returns a non-const pointer. \n
				 * USE WITH CARE!! THIS IS A CONST ITERATOR!
				 */
				inline T *NonConstCurrElem() const
					{  return(_CurrElem());  }
				
				/**
				 * \short Re-start iteration with passed queue in normal 
				 *    (non-reverse) order. 
				 * 
				 * Returns operator bool() value; i.e. true if valid (list 
				 * not empty); false otherwise. 
				 */
				inline bool operator=(const TLLinearQueue &_lq)
					{  _assign(_lq,/*reverse=*/0);  return(_valid());  }
				
				/// Assign a const iterator preserving the location. 
				inline ConstIterator &operator=(const ConstIterator &i)
					{  lq=i.lq;  chk=i.chk;  idx=i.idx;  return(*this);  }
				
				/**
				 * \short Iterate: Advance to next element.
				 * 
				 * Returns validity status. 
				 */
				inline bool operator++()
				{
					if(!_valid())  return(0);
					++idx;
					if(!chk->next && idx>lq->last_idx)  // reached end of list
					{  chk=NULL;  }
					else if(idx>=lq->chunk_size)
					{  idx=0;  chk=chk->next;  }
					return(_valid());
				}
				/// Postfix version of operator++(). 
				inline bool operator++(int)
					{  return(operator++());  }
				
				/**
				 * \short Iterate: Return to previous element.
				 * 
				 * Returns validity status. 
				 */
				inline bool operator--()
				{
					if(!_valid())  return(0);
					if(!chk->prev && idx<=lq->first_idx)  // reached end of list
					{  chk=NULL;  }
					else if(!idx)
					{  chk=chk->prev;  idx=lq.chunk_size-1;  }
					else --idx;
					return(_valid());
				}
				/// Postfix version of operator--(). 
				inline bool operator--(int)
					{  return(operator--());  }
				
				/**
				 * \short Explicitly restart iteration. 
				 * 
				 * Sets iterator to start (reverse=0) or end (reverse=1). 
				 * Returns validity status. 
				 */
				inline bool restart(bool reverse)
					{  if(lq) _assign(*lq,reverse);  return(_valid());  }
		};
		
	protected:
		size_t chunk_size;   ///< Number of elements per chunk. 
		int keep_old;        ///< Number of old unused chunks to keep around. 
		LinkedList<Chunk> clist;  ///< List of chunks. 
		size_t nelem;        ///< Total number of elements in the queue. 
		
		//----------------------------------------------------------------------
		// Internal layout: 
		// 
		// tail->  [CHUNK]---[CHUNK]---[CHUNK]---[CHUNK]  <-head
		// FIRST,INSERT                              LAST,APPEND
		//----------------------------------------------------------------------
		
		/**
		 * \short Index into ent[] of first chunk (tail). \n
		 * 
		 * NOTE: first_idx is the index of the tail element; this value 
		 *       is in range 0..chunk_size-1. A value of 0 means 
		 *       that a new chunk must be allocated when adding to the tail 
		 *       (as e.g. initially after construction). 
		 */
		size_t first_idx;  // 0
		/**
		 * \short Index into ent[] of last chunk (head). \n
		 * 
		 * NOTE: last_idx is the index of the head element; this value 
		 *       is in range 0..chunk_size-1. A value of chunk_size-1 means 
		 *       that a new chunk must be allocated when adding to the head 
		 *       (as e.g. initially after construction). 
		 */
		size_t last_idx;  // chunk_size-1
		
		LinkedList<Chunk> old_list; ///< List of empty (unused) chunks. 
		int old_nents;              ///< Number of chunks in old_list. 
		
	public:
		/// These are the operators... 
		_OP OP;
	
	private:
		/// Allocate a new chunk on the heap. 
		inline Chunk *_AllocChunk()
		{
			char *mem=(char*)malloc(sizeof(Chunk)+chunk_size*sizeof(T));
			assert(mem);
			Chunk *c = new(mem) Chunk();
			return(c);
		}
		/// Free an indivitual chunk. May not be in any list. 
		inline void _FreeChunk(Chunk *c)
			{  c->~Chunk();  free((char*)c);  }
		
		/// Free all chunks in the old_list. 
		void _FreeAllOld()
		{
			while(!old_list.IsEmpty())
			{  _FreeChunk(old_list.PopLast());  --old_nents;  }
			assert(old_nents==0);
		}
		
		/// Add passed chunk to the cache of old chunks. 
		/// Purge old chunk if we have too many of them. 
		inline void _Add2Cache(Chunk *c)
		{
			if(!keep_old && !old_nents)
			{  _FreeChunk(c);  return;  }
			old_list.append(c);  ++old_nents;
			// Always add to the end of old_list because of caching strategy. 
			// In case there are too many elements, remove the oldest one 
			// (this is the first one). 
			while(old_nents>keep_old)
			{  _FreeChunk(old_list.PopFirst());  --old_nents;  }
		}
		/// Get a new chunk from the cache or (if there is none) by allocation. 
		inline Chunk *_NewChunkCache()
		{
			Chunk *c;
			// See if we can re-use a chunk. 
			if(!old_list.IsEmpty())
			{  c=old_list.PopLast();  --old_nents;  }
			else
			{  c=_AllocChunk();  }
			return(c);
		}
		
		/// Add a chunk at the tail end in order to be able to append a 
		/// further element at the tail. 
		void _AddChunkTail()  // <-- NON-inline. 
			{  clist.insert(_NewChunkCache());  first_idx=chunk_size-1;  }
		/// Add a chunk at the head end in order to be able to append a 
		/// further element at the head. 
		void _AddChunkHead()  // <-- NON-inline. 
			{  clist.append(_NewChunkCache());  last_idx=0;  }
		
		/// Remove a chunk from the tail end. 
		void _PopChunkTail()  // <-- NON-inline. 
			{  _Add2Cache(clist.PopFirst());  first_idx=0;  }
		/// Remove a chunk from the head end. 
		void _PopChunkHead()  // <-- NON-inline. 
			{  _Add2Cache(clist.PopLast());  last_idx=chunk_size-1;  }
		
		/// Internally used by PopTail(). 
		inline void _DoPopTail()
		{
			OP.clr(&_ent(clist.first(),first_idx));  --nelem;
			if(!nelem /*first_idx==last_idx && clist.first()==clist.last()*/)
			{  last_idx=chunk_size-1;  _PopChunkTail();  return;  }
			if(++first_idx==chunk_size)  _PopChunkTail();
		}
		/// Internally used by PopHead(). 
		inline void _DoPopHead()
		{
			OP.clr(&_ent(clist.last(),last_idx));  --nelem;
			if(!nelem /*first_idx==last_idx && clist.first()==clist.last()*/)
			{  first_idx=0;  _PopChunkHead();  return;  }
			if(!last_idx)  _PopChunkHead();  else  --last_idx;
		}
		
	private:
		/// Do not use. 
		TLLinearQueue(const TLLinearQueue &);
		/// Do not use. 
		void operator=(const TLLinearQueue &);
	public:
		/**
		 * \short Construct empty linear queue. 
		 * 
		 * \param _chunk_size is the number of elements to store per chunk. 
		 *        (New chunks are allocated when more elements are stored). 
		 * \param _keep_old is the number of old (empty) chunks to keep 
		 *        around so it can be re-used an that no allocation has to 
		 *        be used if a (new) chunk is needed. 
		 * \param op: operator template prototype. 
		 */
		inline TLLinearQueue(size_t _chunk_size,int _keep_old=4,
			const _OP &op=_OP()) : 
			chunk_size(_chunk_size<1 ? 1 : _chunk_size),
			keep_old(_keep_old<0 ? 0 : _keep_old),
			clist(),nelem(0),first_idx(0),last_idx(chunk_size-1),
			old_list(),old_nents(0),OP(op)  {}
		/// Destructor: cleans up queue. 
		inline ~TLLinearQueue()
			{  clear();  _FreeAllOld();  }
		
		/// Is the queue empty? O(1) (constant) time. 
		inline bool IsEmpty() const
			{  return(clist.IsEmpty());  }
		
		/// Get the number of elements in the queue. O(1) (constant) time. 
		inline size_t NElem() const
			{  return(nelem);  }
		
		/// Clear the whole list (removing all elements). 
		void clear(bool free_cached_chunks=0)
		{
			/// \todo tllinearqueue.h:clear: This could be made more efficient. 
			///  (especially if OP.pdt==0)
			while(!IsEmpty())
			{  _DoPopTail();  }
			assert(nelem==0);
			if(free_cached_chunks)  _FreeAllOld();
		}
		
		/**
		 * \short Get element at the head end (last) of the queue. 
		 * 
		 * Runtime is O(1) (constant). 
		 * 
		 * Returns NULL if the queue is empty. \n
		 * The returned element may (of course) be modified. 
		 */
		inline T *head()
			{  return(clist.last() ? &_ent(clist.last(),last_idx) : NULL);  }
		/// Const version of head(). 
		inline const T *head() const
			{  return(clist.last() ? &_ent(clist.last(),last_idx) : NULL);  }
		
		/**
		 * \short Get element at the tail end (first) of the queue. 
		 * 
		 * Runtime is O(1) (constant). 
		 * 
		 * Returns NULL if the queue is empty. \
		 * The returned element may (of course) be modified. 
		 */
		inline T *tail()
			{  return(clist.first() ? &_ent(clist.first(),first_idx) : NULL);  }
		/// Const version of tail(). 
		inline const T *tail() const
			{  return(clist.first() ? &_ent(clist.first(),first_idx) : NULL);  }
		
		/**
		 * \short Append entry at the tail end (=first) of the queue. 
		 * 
		 * The element e is (of course) copied to be stored in the queue. 
		 * 
		 * Runtime is O(1) (constant). 
		 */
		inline void PushTail(const T &e)
		{
			if(!first_idx)  _AddChunkTail();  else  --first_idx;
			Chunk *c=clist.first();  OP.ini(&_ent(c,first_idx),e);  ++nelem;
		}
		
		/**
		 * \short Append entry at the head end (=last) of the queue. 
		 * 
		 * The element e is (of course) copied to be stored in the queue. 
		 * 
		 * Runtime is O(1) (constant). 
		 */
		inline void PushHead(const T &e)
		{
			if(++last_idx==chunk_size)  _AddChunkHead();
			Chunk *c=clist.last();  OP.ini(&_ent(c,last_idx),e);  ++nelem;
		}
		
		/**
		 * \short Remove element from the tail of the list. 
		 * 
		 * Removes element from the tail. If re is non-NULL, a copy of the 
		 * element is stored there, first (using OP.ass()). 
		 * 
		 * Runtime is O(1) (constant). 
		 * 
		 * Returns 1 if the queue was empty, 0 on success. 
		 */
		int PopTail(T *re=NULL)
		{
			if(IsEmpty())  return(1);
			if(re)  OP.ass(*re,_ent(clist.first(),first_idx));
			_DoPopTail();
			return(0);
		}
		
		/**
		 * \short Remove element from the head of the list. 
		 * 
		 * Removes element from the head. If re is non-NULL, a copy of the 
		 * element is stored there, first (using OP.ass()). 
		 * 
		 * Runtime is O(1) (constant). 
		 * 
		 * Returns 1 if the queue was empty, 0 on success. 
		 */
		int PopHead(T *re=NULL)
		{
			if(IsEmpty())  return(1);
			if(re)  OP.ass(*re,_ent(clist.last(),last_idx));
			_DoPopHead();
			return(0);
		}
};

#endif  /* _TemplateLibrary_LinearQueue_H_ */
