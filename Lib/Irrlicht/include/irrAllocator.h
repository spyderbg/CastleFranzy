//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine" and the "irrXML" project.
// For conditions of distribution and use, see copyright notice in irrlicht.h and irrXML.h

#ifndef __IRR_ALLOCATOR_H_INCLUDED__
#define __IRR_ALLOCATOR_H_INCLUDED__

#include "irrTypes.h"
#include <cstddef>
#include <new>
#if USE_MEMORY_H
#include <memory.h>
#endif

namespace irr
{
namespace core
{

//#ifdef DEBUG_CLIENTBLOCK
//#undef DEBUG_CLIENTBLOCK
//#define DEBUG_CLIENTBLOCK irrnew
//#endif

//! Very simple allocator implementation, containers using it can be used across dll boundaries
template<typename T>
class irrAllocator
{
public:

	// STL allocator compliance defs

	typedef size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	template <typename T1>
	struct rebind
	{
		typedef irrAllocator<T1> other;
	};

	pointer address(reference r) const
	{
		return &r;
	}

	const_pointer address(const_reference r) const
	{
		return &r;
	}

	irrAllocator() {}

	template <typename T2>
	irrAllocator(const irrAllocator<T2>& other) {}

	//! Destructor
	// not a good idea to make this destructor virtual: it eats up an extra word
	// of memory for nothing
	//virtual ~irrAllocator() {}

	//! Allocate memory for an array of objects
	T* allocate(size_t cnt, const void* hint = 0)
	{
		return (T*)internal_new(cnt* sizeof(T));
	}

	size_type max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	//! Deallocate memory for an array of objects
	void deallocate(T* ptr, size_type n = 0)
	{
		internal_delete(ptr);
	}

	//! Construct an element
	void construct(T* ptr, const T& e)
	{
		new ((void*)ptr) T(e);
	}

	//! Destruct an element
	void destruct(T* ptr)
	{
		ptr->~T();
	}

	//! Synonym for destruct (for STL compliance).
	void destroy(T* ptr)
	{
		destruct(ptr);
	}

protected:

	void* internal_new(size_t cnt)
	{
		return operator_irrnew(cnt);
	}

	void internal_delete(void* ptr)
	{
		operator delete(ptr);
	}

};


//! Fast allocator, only to be used in containers inside the same memory heap.
/** Containers using it are NOT able to be used it across dll boundaries. Use this
when using in an internal class or function or when compiled into a static lib */
template<typename T>
class irrAllocatorFast
{
public:

	//! Allocate memory for an array of objects
	T* allocate(size_t cnt)
	{
		return (T*)operator_irrnew(cnt* sizeof(T));
	}

	//! Deallocate memory for an array of objects
	void deallocate(T* ptr)
	{
		operator delete(ptr);
	}

	//! Construct an element
	void construct(T* ptr, const T&e)
	{
		new ((void*)ptr) T(e);
	}

	//! Destruct an element
	void destruct(T* ptr)
	{
		ptr->~T();
	}
};


template<typename T>
class irrDummyAllocator
{
public:

	//! Destructor
	//virtual ~irrDummyAllocator() {}

	//! Allocate memory for an array of objects
	T* allocate(size_t cnt)
	{
		return 0;
	}

	//! Deallocate memory for an array of objects
	void deallocate(T* ptr)
	{
	}

	//! Construct an element
	void construct(T* ptr, const T&e)
	{
		new ((void*)ptr) T(e);
	}

	//! Destruct an element
	void destruct(T* ptr)
	{
		ptr->~T();
	}

protected:

	virtual void* internal_new(size_t cnt)
	{
		return operator_irrnew(cnt);
	}

	virtual void internal_delete(void* ptr)
	{
		operator delete(ptr);
	}

};


//#ifdef DEBUG_CLIENTBLOCK
//#undef DEBUG_CLIENTBLOCK
//#define DEBUG_CLIENTBLOCK irrnew //( _CLIENT_BLOCK, __FILE__, __LINE__)
//#endif


} // end namespace core
} // end namespace irr

#endif

