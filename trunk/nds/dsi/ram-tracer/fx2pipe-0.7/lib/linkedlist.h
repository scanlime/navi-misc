/*
 * lib/linkedlist.h
 * 
 * Doubly linked list class template. 
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

#ifndef _INCLUDE_FX2PIPE_LIB_LINKEDLIST_H_
#define _INCLUDE_FX2PIPE_LIB_LINKEDLIST_H_ 1

#include "../oconfig.h"


/**
 * \short Base class for list node types used together with LinkedList. 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 */
template<typename T,bool DoDeinit=1> struct LinkedListBase
{
	/// Pointer to the previous and next nodes in the list. 
	T *prev,*next;
	
	enum _LLB_NoInit { LLB_NoInit };
	
	/// Normal constructor: set prev and next to NULL: 
	inline LinkedListBase()  {  prev=next=NULL;  }
	/// Special constructor which does NOT set up prev and next: 
	inline LinkedListBase(_LLB_NoInit)  { }
	/// Destructor no-op...
	inline ~LinkedListBase()
		{  if(DoDeinit) { prev=NULL; next=NULL; }  }
};


/**
 * \short List class holding a list of objects derived from 
 *   \ref LinkedListBase (not thread-safe). 
 * \author Wolfgang Wieser ] wwieser (a) gmx <*> de [
 * 
 * \see File linkedlist.h for more information. 
 * 
 * LinkedList objects are NOT C++-safe. 
 * 
 * <b>Thread safety:</b> \n
 * A linked list must be externally protected when modifying the list 
 * membes!
 */
template<typename T,typename LLB=LinkedListBase<T> > class LinkedList
{
	private:
		T *lfirst,*llast;
		
		/// Don't use: (use AssignList in rare cases): 
		inline LinkedList &operator=(const LinkedList &l)
			{  lfirst=l.lfirst;  llast=l.llast;  return(*this);  }
		/// Don't use: (use AssignList in rare cases): 
		inline LinkedList(const LinkedList &l)
			{  lfirst=l.lfirst;  llast=l.llast;  }
	public:
		/// Construct empty list: 
		inline LinkedList()  {  lfirst=llast=NULL;  }
		/**
		 * \short Destructor. 
		 * 
		 * It is your responsibility to properly empty the 
		 * list before destroying it. 
		 */
		inline ~LinkedList()  {  lfirst=llast=NULL;  }
		
		/**
		 * \short Get first and last element of the list (or NULL) 
		 */
		/// \{
		inline T *first()  const  {  return(lfirst);  }
		inline T *last()   const  {  return(llast);   }
		/// \}
		
		/// Returns true if the list is empty otherwise false. 
		inline bool IsEmpty()  const  {  return(!lfirst);  }
		
		/**
		 * \short Get next/prev element from passed element. 
		 * 
		 * May be used if you experience ambiguities using multiple 
		 * inheritance. 
		 */
		/// \{
		inline T *next(T *p)  const  {  return(p->LLB::next);  }
		inline T *prev(T *p)  const  {  return(p->LLB::prev);  }
		inline const T *next(const T *p)  const  {  return(p->LLB::next);  }
		inline const T *prev(const T *p)  const  {  return(p->LLB::prev);  }
		/// \}
		
		/// Use with care!! Just copies pointer to head and tail of 
		/// list; if you modify one of the lists, the other one 
		/// will get inconsistent if lfirst/llast get modified. 
		inline void AssignList(const LinkedList<T> *l)
			{  lfirst=l->lfirst;  llast=l->llast;  }
		
		/// This is similar, just that the list pointers are swapped. 
		inline void SwapList(LinkedList<T> *l)
			{  T *tmp=lfirst;  lfirst=l->lfirst;  l->lfirst=tmp;
			      tmp=llast;   llast= l->llast;   l->llast= tmp;  }
		
		/// Insert p at the beginning of the list: 
		inline void insert(T *p)
		{
			if(!p)  return;
			p->LLB::next=lfirst;
			p->LLB::prev=NULL;
			//if(lfirst)  lfirst->LLB::prev=p;
			//else  llast=p;
			(lfirst ? lfirst->LLB::prev : llast)=p;
			lfirst=p;
		}
		
		/// Append p to the end of the list: 
		inline void append(T *p)
		{
			if(!p)  return;
			p->LLB::prev=llast;
			p->LLB::next=NULL;
			//if(llast)  llast->LLB::next=p;
			//else  lfirst=p;
			(llast ? llast->LLB::next : lfirst)=p;
			llast=p;
		}
		
		/// Dequeues *oldp and puts newp at the place where oldp was. 
		/// Make sure neither oldp nor newp are NULL. 
		/// Returns oldp (so that you may pass it to operator delete). 
		inline T *replace(T *newp,T *oldp)
		{
			newp->LLB::next=oldp->LLB::next;
			newp->LLB::prev=oldp->LLB::prev;
			if(lfirst==oldp)  lfirst=newp;
			if(llast==oldp)   llast=newp;
			oldp->LLB::prev=oldp->LLB::next=NULL;
			return(oldp);
		}
		
		/// Put p before/after `where´ into the queue: 
		/// Make sure, `where´ is not NULL. 
		inline void QueueBefore(T *p,T *where)
		{
			if(!p)  return;
			if(where==lfirst)
			{  insert(p);  return;  }
			p->LLB::prev=where->LLB::prev;
			where->LLB::prev->LLB::next=p;
			where->LLB::prev=p;
			p->LLB::next=where;
		}
		inline void QueueAfter(T *p,T *where)
		{
			if(!p)  return;
			if(where==llast)
			{  append(p);  return;  }
			p->LLB::next=where->LLB::next;
			where->LLB::next->LLB::prev=p;
			where->LLB::next=p;
			p->LLB::prev=where;
		}
		
		/// Queue before/after `where´, depending on loc. 
		/// loc<0 -> before; loc>0 -> after; loc==0 -> do nothing 
		void queue(T *p,T *where,int loc)
		{
			if(loc<0)  QueueBefore(p,where);
			if(loc>0)  QueueAfter(p,where);
		}
		
		/// Dequeues element p (and returns p). 
		inline T *dequeue(T *p)
		{
			if(!p)  return(p);
			if(p==lfirst)  lfirst=lfirst->LLB::next;
			else  p->LLB::prev->LLB::next=p->LLB::next;
			if(p==llast)  llast=llast->LLB::prev;
			else  p->LLB::next->LLB::prev=p->LLB::prev;
			p->LLB::next=p->LLB::prev=NULL;
			return(p);
		}
		
		/// Dequeue first/last element and return it (or NULL): 
		inline T *PopFirst()
		{
			if(!lfirst)  return(lfirst);
			T *p=lfirst;
			lfirst=lfirst->LLB::next;
			if(lfirst)  lfirst->LLB::prev=NULL;
			else  llast=NULL;
			p->LLB::next=NULL;
			return(p);
		}
		inline T *PopLast()
		{
			if(!llast)  return(llast);
			T *p=llast;
			llast=llast->LLB::prev;
			if(llast)  llast->LLB::next=NULL;
			else  lfirst=NULL;
			p->LLB::prev=NULL;
			return(p);
		}
		
		/// Find element p in this list; returns either p or NULL. 
		inline const T *search(const T *p)  const
		{
			for(T *i=lfirst; i; i=i->LLB::next)
			{  if(i==p)  return(p);  }
			return(NULL);
		}
		/// The same as search() but traverse the list from last to first. 
		inline const T *SearchRev(const T *p)  const
		{
			for(T *i=llast; i; i=i->LLB::prev)
			{  if(i==p)  return(p);  }
			return(NULL);
		}
		
		/// Cound the elements in the list: 
		inline int count()  const
		{
			int cnt=0;
			for(T *i=lfirst; i; i=i->LLB::next,cnt++);
			return(cnt);
		}
		
		/// Clear list by deleting all entries. 
		inline void clear()
			{  while(!IsEmpty()) delete PopFirst();  }
		
		/**
		 * \short Append content of passed list to this list. 
		 * 
		 * All elements in the passed list will be appended to this list 
		 * and the passed list will be empty afterwards. 
		 * 
		 * This is \b much \b faster \b than
		 * \code
		 *   while(!b.IsEmpty())  a.append(b.PopFirst());  
		 * \endcode
		 * (Runtime is O(1).)
		 */
		inline void AppendList(LinkedList<T> &b)
		{
			if(b.IsEmpty()) return;
			
			b.lfirst->prev=llast;
			//if(llast)  llast->LLB::next=b.first();
			//else  lfirst=b.first();
			(llast ? llast->LLB::next : lfirst)=b.lfirst;
			llast=b.llast;
			
			b.lfirst=NULL;  b.llast=NULL;
		}
};

#endif /* _INCLUDE_FX2PIPE_LIB_LINKEDLIST_H_ */
