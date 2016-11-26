#pragma once

#include "ContainersCommon.h"
#include "List.h"

#include <iostream>

template<
	typename T, /** Type of objects to be stored inside the stack */
	typename ElementDtor = ContainerDefaultElementDestructor<T>, /** Function object which is called by Pop and PopAll or the list destructor */
	bool CallElementDestructorOnStackDestructor = true, /** Wheter or not to call the element destructor function object ElementDtor from the stack destructor */
	bool CallElementDestructorOnStackCopy = false> /** Wheter or not to call the element destructor function object ElementDtor from the list assign operator */
class Stack
{
public:
	/** Useful typedefs */
	typedef T ValueType;
	typedef T& ReferenceType;
	typedef T const& ConstReferenceType;
	typedef T* PointerType;
	typedef T const* ConstPointerType;

	static_assert(!std::is_reference<ValueType>::value, "Stack cannot store references to objects");
	
public:
	/**
	* Default constructor
	* startingSize:	the number of (uninitialized) elements that the list will contain to start with (to prevent reallocations)
	* growBy: the number of items that will be added to the list every time that the list is full and a new element is added
	*/
	Stack(int startingSize = 0, int growBy = 0)
		: _items(startingSize, growBy)
	{ }

	/** Copy constructor. */
	Stack(const Stack& src)
		: _items(src._items)
	{ }

	/** Move constructor */
	Stack(Stack&& src)
		: _items(std::move(src._items))
	{ }

	/** Destructor. Doesn't need to do nothing since the interanl list will be cleaned automatically. */
	~Stack() { }

public:
	/** Assign operator */
	Stack& operator=(const Stack& src)
	{
		if (this != &src)
			_items = src._items;
		return *this;
	}
	
	/** Move operator */
	Stack& operator=(Stack&& src)
	{
		if(this != &src)
			_items = std::move(src._items);
		return *this;
	}
	
public:
	/** Push the element to the top of the stack */
	Stack& Push(ConstReferenceType element) { _items.Add(element); return *this; }

	/**
	* Removes the element at the top of the stack from the stack
	* callElementDestructor: if this parameter is given as true then the element destructor is called on the element before removing it from the stack
	*/
	Stack& Pop(bool callElementDestructor = false)
	{
		if (_items.GetSize() <= 0)
			throw "Stack error[Pop]: cannot pop elements from the stack because the stack is empty";
		_items.ReverseRemoveAt(0, 1, callElementDestructor);
		return *this;
	}

	/**
	* Returns the element that is currently at the top of the stack without removing it
	* If the stack is empty an exception is thrown
	*/
	ReferenceType Top()
	{
		if (_items.GetSize() <= 0)
			throw "Stack error[Top]: cannot return the top element of the stack because the stack is empty";
		return _items.Back();
	}

	/**
	* Returns the element that is currently at the top of the stack without removing it
	* If the stack is empty an exception is thrown
	*/
	ConstReferenceType Top() const
	{
		if (_items.GetSize() <= 0)
			throw "Stack error[Top]: cannot return the top element of the stack because the stack is empty";
		return _items.Back();
	}

	/** Returns the number of elements inside the stack */
	int GetSize() const { return _items.GetSize(); }

	/** Returns the number of elements that the stack can store without the need to allocate more memory */
	int GetCapacity() const { return _items.GetCapacity(); }
	
private:
	/** Use an internal list to store the items */
	List<ValueType, ElementDtor, CallElementDestructorOnStackDestructor, CallElementDestructorOnStackCopy> _items;
};