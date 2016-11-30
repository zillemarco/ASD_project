#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "ContainersCommon.h"

#include <iostream>

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif // _DEBUG

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

template<
	typename T, /** Type of objects to be stored inside the list */
	typename ElementDtor = ContainerDefaultElementDestructor<T>, /** Function object which is called by Clear, RemoveAt, Remove or the list destructor */
	bool CallElementDestructorOnListDestructor = true, /** Wheter or not to call the element destructor function object ElementDtor from the list destructor */
	bool CallElementDestructorOnListCopy = false> /** Wheter or not to call the element destructor function object ElementDtor from the list assign operator */
class List
{
public:
	/** Useful typedefs */
	typedef T ValueType;
	typedef T& ReferenceType;
	typedef T const& ConstReferenceType;
	typedef T* PointerType;
	typedef T const* ConstPointerType;

	static_assert(!std::is_reference<ValueType>::value, "List cannot store references to objects");

private:
	struct ListItem
	{
		/**
		* Default constructor
		* element: the element this item contains
		* next: the pointer to the next node on the list
		* prev: the pointer to the previous node on the list
		* isUsed: true if the element contained by this node is valid
		*/
		ListItem(ValueType element, ListItem* next = nullptr, ListItem* prev = nullptr, bool isUsed = false)
			: _element(element)
			, _next(next)
			, _prev(prev)
			, _isUsed(isUsed)
		{ }

		ListItem(const ListItem& src)
			: _element(src._element)
			, _next(src._next)
			, _prev(src._prev)
			, _isUsed(src._isUsed)
		{ }

		ListItem(ListItem&& src)
			: _element(std::move(src._element))
			, _next(std::move(src._next))
			, _prev(std::move(src._prev))
			, _isUsed(std::move(src._isUsed))
		{
			src._next = nullptr;
			src._prev = nullptr;
			src._isUsed = false;
		}

		virtual ~ListItem()
		{
			_isUsed = false;
			_next = nullptr;
			_prev = nullptr;
		}

		ListItem& operator=(const ListItem& src)
		{
			if (&src != this)
			{
				_element = src._element;
				_next = src._next;
				_prev = src._prev;
				_isUsed = src._isUsed;
			}
			return *this;
		}

		ListItem& operator=(ListItem&& src)
		{
			if (&src != this)
			{
				_element = std::move(src._element);
				_next = std::move(src._next);
				_prev = std::move(src._prev);
				_isUsed = std::move(src._isUsed);
				
				src._next = nullptr;
				src._prev = nullptr;
				src._isUsed = false;
			}
			return *this;
		}

		ValueType _element;	/** The element contained by an item of the list */
		ListItem* _next;	/** A pointer to the next item on the list */
		ListItem* _prev;	/** A pointer to the previous item on the list */
		bool _isUsed;		/** When this is true that the element is valid */
	};

public:

	/** Utility struct that can be used to iterate through the elements of the list in both directions */
	class ConstIterator
	{
		friend List;

	public:
		ConstIterator(const List* list, const ListItem* item, bool isBegin, bool isEnd)
			: _list(list)
			, _item(item)
			, _isBegin(isBegin)
			, _isEnd(isEnd)
		{ }

		ConstIterator(const ConstIterator& src)
			: _list(src._list)
			, _item(src._item)
			, _isBegin(src._isBegin)
			, _isEnd(src._isEnd)
		{ }

		ConstIterator(ConstIterator&& src)
			: _list(std::move(src._list))
			, _item(std::move(src._item))
			, _isBegin(std::move(src._isBegin))
			, _isEnd(std::move(src._isEnd))
		{ }

		virtual ~ConstIterator() { }

		ConstIterator& operator=(const ConstIterator& src)
		{
			if (this != &src)
			{
				_list = src._list;
				_item = src._item;
				_isBegin = src._isBegin;
				_isEnd = src._isEnd;
			}
			return *this;
		}

		ConstIterator& operator=(ConstIterator&& src)
		{
			if (this != &src)
			{
				_list = std::move(src._list);
				_item = std::move(src._item);
				_isBegin = std::move(src._isBegin);
				_isEnd = std::move(src._isEnd);

				src._list = nullptr;
				src._item = nullptr;
			}
			return *this;
		}

		operator bool() { return (_list != nullptr && _item != nullptr); }

		bool operator==(const ConstIterator& rhs) { return ((this->_list == rhs._list) && (this->_item == rhs._item) && (this->_isBegin == rhs._isBegin) && (this->_isEnd == rhs._isEnd)); }
		bool operator!=(const ConstIterator& rhs) { return !operator==(rhs); }

		ConstReferenceType operator*() const
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator*]: the item is not valid";
			return _item->_element;
		}

		ConstReferenceType operator->() const
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator->]: the item is not valid";
			return _item->_element;
		}

		ConstIterator& operator++()
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator++]: the item is not valid";
			_item = _item->_next;
			return *this;
		}
		ConstIterator operator++(int)
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator++]: the item is not valid";

			ConstIterator tmp(*this);
			_item = _item->_next;
			return tmp;
		}

		ConstIterator& operator--()
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator--]: the item is not valid";
			_item = _item->_prev;
			return *this;
		}
		ConstIterator operator--(int)
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator--]: the item is not valid";

			ConstIterator tmp(*this);
			_item = _item->_prev;
			return tmp;
		}

		ConstIterator& operator+=(int n)
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator+=]: the item is not valid";

			while (_item != nullptr && n-- > 0)
				_item = _item->_next;
			return *this;
		}
		ConstIterator& operator-=(int n)
		{
			if (_item == nullptr)
				throw "List ConstIterator error [operator-=]: the item is not valid";

			while (_item != nullptr && n-- > 0)
				_item = _item->_prev;
			return *this;
		}

		friend ConstIterator operator+(ConstIterator it, int n)
		{
			it += n;
			return it;
		}
		friend ConstIterator operator-(ConstIterator it, int n)
		{
			it -= n;
			return it;
		}

		/** Returns true if the given list is the same that created the iterator */
		bool IsOfList(List* list) { return _list == list; }

	private:
		const List* _list;
		const ListItem* _item;
		bool _isEnd;
		bool _isBegin;
	};


	/** Utility struct that can be used to iterate through the elements of the list in both directions */
	class Iterator
	{
		friend List;

	public:
		Iterator(const List* list, ListItem* item, bool isBegin, bool isEnd)
			: _list(list)
			, _item(item)
			, _isBegin(isBegin)
			, _isEnd(isEnd)
		{ }

		Iterator(const Iterator& src)
			: _list(src._list)
			, _item(src._item)
			, _isBegin(src._isBegin)
			, _isEnd(src._isEnd)
		{ }

		Iterator(Iterator&& src)
			: _list(std::move(src._list))
			, _item(std::move(src._item))
			, _isBegin(std::move(src._isBegin))
			, _isEnd(std::move(src._isEnd))
		{ }

		virtual ~Iterator() { }

		Iterator& operator=(const Iterator& src)
		{
			if (this != &src)
			{
				_list = src._list;
				_item = src._item;
				_isBegin = src._isBegin;
				_isEnd = src._isEnd;
			}
			return *this;
		}

		Iterator& operator=(Iterator&& src)
		{
			if (this != &src)
			{
				_list = std::move(src._list);
				_item = std::move(src._item);
				_isBegin = std::move(src._isBegin);
				_isEnd = std::move(src._isEnd);

				src._list = nullptr;
				src._item = nullptr;
			}
			return *this;
		}

		operator bool() { return (_list != nullptr && _item != nullptr); }

		bool operator==(const Iterator& rhs) { return ((this->_list == rhs._list) && (this->_item == rhs._item) && (this->_isBegin == rhs._isBegin) && (this->_isEnd == rhs._isEnd)); }
		bool operator!=(const Iterator& rhs) { return !operator==(rhs); }

		ReferenceType operator*()
		{
			if (_item == nullptr)
				throw "List Iterator error [operator*]: the item is not valid";
			return _item->_element;
		}
		ConstReferenceType operator*() const
		{
			if (_item == nullptr)
				throw "List Iterator error [operator*]: the item is not valid";
			return _item->_element;
		}

		ReferenceType operator->()
		{
			if (_item == nullptr)
				throw "List Iterator error [operator->]: the item is not valid";
			return _item->_element;
		}
		ConstReferenceType operator->() const
		{
			if (_item == nullptr)
				throw "List Iterator error [operator->]: the item is not valid";
			return _item->_element;
		}

		Iterator& operator++()
		{
			if (_item == nullptr)
				throw "List Iterator error [operator++]: the item is not valid";
			_item = _item->_next;
			return *this;
		}
		Iterator operator++(int)
		{
			if (_item == nullptr)
				throw "List Iterator error [operator++]: the item is not valid";

			Iterator tmp(*this);
			_item = _item->_next;
			return tmp;
		}

		Iterator& operator--()
		{
			if (_item == nullptr)
				throw "List Iterator error [operator--]: the item is not valid";
			_item = _item->_prev;
			return *this;
		}
		Iterator operator--(int)
		{
			if (_item == nullptr)
				throw "List Iterator error [operator--]: the item is not valid";

			Iterator tmp(*this);
			_item = _item->_prev;
			return tmp;
		}

		Iterator& operator+=(int n)
		{
			if (_item == nullptr)
				throw "List Iterator error [operator+=]: the item is not valid";

			while (_item != nullptr && n-- > 0)
				_item = _item->_next;
			return *this;
		}
		Iterator& operator-=(int n)
		{
			if (_item == nullptr)
				throw "List Iterator error [operator-=]: the item is not valid";

			while (_item != nullptr && n-- > 0)
				_item = _item->_prev;
			return *this;
		}

		friend Iterator operator+(Iterator it, int n)
		{
			it += n;
			return it;
		}
		friend Iterator operator-(Iterator it, int n)
		{
			it -= n;
			return it;
		}

		/** Returns true if the given list is the same that created the iterator */
		bool IsOfList(List* list) { return _list == list; }

	private:
		const List* _list;
		ListItem* _item;
		bool _isEnd;
		bool _isBegin;
	};

public:
	/**
	* Default constructor
	* startingSize:	the number of (uninitialized) elements that the list will contain to start with (to prevent reallocations)
	* growBy: the number of items that will be added to the list every time that the list is full and a new element is added
	*/
	List(int startingSize = 0, int growBy = 0)
		: _head(nullptr)
		, _tail(nullptr)
		, _freeItemsHead(nullptr)
		, _freeItemsTail(nullptr)
		, _size(0)
		, _capacity(0)
		, _growBy(growBy)
	{
		// Setup the list
		if (startingSize > 0)
			Grow(startingSize);
		else
			Grow(ComputeGrowAmount());
	}

	/**
	* Copy constructor
	* We are in the constructor so we still initialize the members to an invalid state so that Copy won't crash
	*/
	List(const List& src)
		: _head(nullptr)
		, _tail(nullptr)
		, _freeItemsHead(nullptr)
		, _freeItemsTail(nullptr)
		, _size(0)
		, _capacity(0)
		, _growBy(0)
	{
		Copy(src);
	}

	/** Move constructor */
	List(List&& src)
		: _head(std::move(src._head))
		, _tail(std::move(src._tail))
		, _freeItemsHead(std::move(src._freeItemsHead))
		, _freeItemsTail(std::move(src._freeItemsTail))
		, _size(std::move(src._size))
		, _capacity(std::move(src._capacity))
		, _growBy(std::move(src._growBy))
	{
		src._head = nullptr;
		src._tail = nullptr;
		src._freeItemsHead = nullptr;
		src._freeItemsTail = nullptr;
		src._size = 0;
		src._capacity = 0;
		src._growBy = 0;
	}

	/** Destructor */
	~List()
	{
		Clear(0, CallElementDestructorOnListDestructor);
	}

public:
	/** Assign operator */
	List& operator=(const List& src)
	{
		if (this != &src)
			Copy(src);
		return *this;
	}
	
	/** Move operator */
	List& operator=(List&& src)
	{
		if(this != &src)
		{
			_head = std::move(src._head);
			_tail = std::move(src._tail);
			_freeItemsHead = std::move(src._freeItemsHead);
			_freeItemsTail = std::move(src._freeItemsTail);
			_size = src._size;
			_capacity = src._capacity;
			_growBy = src._growBy;
			
			src._head = nullptr;
			src._tail = nullptr;
			src._freeItemsHead = nullptr;
			src._freeItemsTail = nullptr;
			src._size = 0;
			src._capacity = 0;
			src._growBy = 0;
		}

		return *this;
	}

	/**
	* Index operator
	* Returns the element at the given index
	* If the index is invalid the value returned is the default specified on the template.
	*/
	ReferenceType operator[](int index) { return GetAt(index); }

	/**
	* Index operator
	* Returns the element at the given index
	* If the index is invalid the value returned is the default specified on the template.
	*/
	ConstReferenceType operator[](int index) const { return GetAt(index); }
	
	/** Equality operator. Calls the equality operator of the element type to see if the lists are the same */
	friend bool operator==(const List& lhs, const List& rhs)
	{
		// If the two lists have different size then they are different
		if (lhs.GetSize() != rhs.GetSize())
			return false;

		// Loop through the items. The first one we find that is different means that the other list
		// is different from the current one
		ListItem* item_a = lhs._head;
		ListItem* item_b = rhs._head;

		while (item_a != nullptr && item_b != nullptr)
		{
			if (item_a->_element != item_b->_element)
				return false;

			// Move to the next item
			item_a = item_a->_next;
			item_b = item_b->_next;
		}

		// The elements are also the same so the two lists are equal
		return true;
	}

	/**
	* Inequality operator.
	* Calls the equality operator of the element type to see if the lists are the same.
	* Internally uses the equality operator of the list
	*/
	friend bool operator!=(const List& lhs, const List& rhs) { return (lhs == rhs) == false; }

private:
	void Copy(const List& src)
	{
		// If the source's heads are nullptr or it has no capacity then the source list is invalid
		if ((src._head == nullptr && src._freeItemsHead == nullptr) || src._capacity <= 0)
			return;

		Clear(src._capacity, CallElementDestructorOnListCopy);

		//if (_head != nullptr || _freeItemsHead != nullptr)
		//{
		//	// Clear the list and makes sure that this lists capacity matches the source list capacity
		//	Clear(src._capacity, CallElementDestructorOnListCopy);
		//}
		//else
		//{
		//	// If the heads are nullptr then Copy is being called from the copy constructor since otherwise they are ALWAYS not nullptr
		//	// Call grow to initialize the pointers and have the same capacity as the source
		//	Grow(src._capacity);
		//}

		// Copy the elements from the source list
		// Loop only until the item is valid or the item isn't used since the
		// first unused item marks the end of the used elements of a list
		ListItem* item = src._head;
		while (item != nullptr && item->_isUsed)
		{
			Add(item->_element);
			item = item->_next;
		}
	}

	void Grow(int amount)
	{
		//// First initialization of the list
		//if (_head == nullptr && _freeItemsHead == nullptr)
		//{
		//	if (_tail != nullptr || _freeItemsTail != nullptr)
		//	{
		//		std::cerr << "List error [Grow]: inconsisten list." << std::endl;
		//		return;
		//	}

		//	// Create the first free element and setup all the pointers and the sizes
		//	_freeItemsHead = new ListItem(DefaultValue());
		//	_freeItemsTail = _freeItemsHead;

		//	_head = nullptr;
		//	_tail = nullptr;
		//	_capacity = 1;
		//	_size = 0;
		//	
		//	// Since we just added an item, the head, decrease the number of items to add
		//	amount--;
		//}

		// Add the items to the list
		while (amount > 0)
		{
			// Create a new item
			ListItem* newItem = new ListItem(DefaultValue());

			// Set the previous item of the new item as the current tail of the list
			newItem->_prev = _freeItemsTail;

			// Set the next item of the current tail of the list as the new item
			if(_freeItemsTail)
				_freeItemsTail->_next = newItem;

			// Change the last free item of the list to be the new item
			_freeItemsTail = newItem;

			// If _freeItemsHead is nullptr then a call to Add was made and the list has been filled
			// so make _freeItemsHead point to the first free item
			if (_freeItemsHead == nullptr)
				_freeItemsHead = newItem;

			amount--;
			_capacity++;
		}
	}

	/**
	* Computes the number of items to add to the list
	* It returns _growBy if it's more than 0, otherwise returns 1/10 of the current capacity clamped between [1..20]
	*/
	int ComputeGrowAmount() const
	{
		if (_growBy > 0)
			return _growBy;

		// Double the capacity to use the exponential grow
		// in order to meet the ammortized constant time
		return _capacity > 0 ? _capacity : 1;
	}

	/**
	* Utility function used by RemoveAt and Remove.
	* Moves itemToRemove from the used items list to the free list and calls ElementDtor if told so
	*/
	void RemoveElement(ListItem* itemToRemove, bool callElementDestructor)
	{
		// Make sure itemToRemove is valid
		if (itemToRemove == nullptr)
			return;

		// Call ElementDtor if told so
		if (callElementDestructor)
		{
			ElementDtor dtor;
			dtor(itemToRemove->_element);
		}

		// Set the item as not used
		itemToRemove->_isUsed = false;

		// If the item to remove is the head, make the head point to the item next to the item to remove
		if (itemToRemove == _head)
			_head = itemToRemove->_next;

		// If the item to remove is the tail, make the tail point to the item previous to the item to remove
		if (itemToRemove == _tail)
			_tail = itemToRemove->_prev;

		// If the item has a previous item, make it point the the item next to the item to remove
		if (itemToRemove->_prev)
			itemToRemove->_prev->_next = itemToRemove->_next;

		// If the item has a next item, make it point the the item previous to the item to remove
		if (itemToRemove->_next)
			itemToRemove->_next->_prev = itemToRemove->_prev;

		// Put the item to remove at the end of the free items list
		itemToRemove->_next = nullptr;
		itemToRemove->_prev = _freeItemsTail;

		// If there is a free items list tail, change it to point to the item to remove
		if (_freeItemsTail)
			_freeItemsTail->_next = itemToRemove;
		_freeItemsTail = itemToRemove;

		// If there is no free items list head then the valid list was full and the free list was empty,
		// so set the free list head to point to element just freed
		if (_freeItemsHead == nullptr)
			_freeItemsHead = _freeItemsTail;

		// Decrement the size
		_size--;
	}

public:
	/** Utility method used to get the default value for an element of the list */
	ReferenceType DefaultValue()
	{
		static ValueType s_defaultValue = ContainerElementDefaultValue<ValueType>::Value();
		return s_defaultValue;
	}

	/** Utility method used to get the default value for an element of the list */
	ConstReferenceType DefaultValue() const
	{
		static ValueType s_defaultValue = ContainerElementDefaultValue<ValueType>::Value();
		return s_defaultValue;
	}

public:
	/**
	* Adds and element to the list
	* Returns the list with the new element added
	* element: the element to add to the list
	*/
	List& Add(ValueType element)
	{
		// If the capacity and size are the same then the list is full, so make it grow
		if (_capacity == _size)
			Grow(ComputeGrowAmount());

		if (_freeItemsHead == nullptr)
		{
			std::cerr << "List error [Add]: _freeItemsHead is nullptr. The list is invalid" << std::endl;
			return *this;
		}

		ListItem* itemToAdd = _freeItemsHead;
		ListItem* itemToAddNext = itemToAdd->_next;

		// Save the element inside the first free item
		itemToAdd->_element = element;
		itemToAdd->_isUsed = true;

		// If _head is nullptr the list was empty so set it up
		if (_head == nullptr)
		{
			// Head and tail are the same
			_head = _tail = itemToAdd;

			_head->_next = nullptr;
			_head->_prev = nullptr;
		}
		else
		{
			// Make the new last item _prev point to the current tail of the list
			itemToAdd->_prev = _tail;

			// Make the current list tail _next point to the new last item
			_tail->_next = itemToAdd;

			// Make the new last item the tail of the list
			_tail = itemToAdd;
		}


		// Remove the first free item from the free items list
		{
			// If the free items head and tail are equal then there was only one free element so set both free items head and tail to nullptr
			if (_freeItemsTail == _freeItemsHead)
			{
				_freeItemsHead = nullptr;
				_freeItemsTail = nullptr;
			}
			// Set the sencond free item as the free items list head
			else if (itemToAddNext)
			{
				itemToAddNext->_prev = nullptr;
				_freeItemsHead = itemToAddNext;
			}
		}

		// Break the link between the last valid item and the first free item
		_tail->_next = nullptr;

		// Increment the number of stored elements
		_size++;

		// // Save the element inside the first free item
		// _freeItemsHead->_element = element;
		// _freeItemsHead->_isUsed = true;
		// 
		// // Memorize the current free items list head next element to use it later since
		// // if _head is nullptr _head->_next is put to nullptr, which is the same as _freeItemsHead->_next
		// // and it can break the list links
		// ListItem* freeItemsHeadNext = _freeItemsHead->_next;
		// 
		// // If _head is nullptr the list was empty so set it up
		// if (_head == nullptr)
		// {
		// 	// Head and tail are the same
		// 	_head = _tail = _freeItemsHead;
		// 	
		// 	_head->_next = nullptr;
		// 	_head->_prev = nullptr;
		// }
		// else
		// {
		// 	// Make the new last item _prev point to the current tail of the list
		// 	_freeItemsHead->_prev = _tail;
		// 
		// 	// Make the current list tail _next point to the new last item
		// 	_tail->_next = _freeItemsHead;
		// 
		// 	// Make the new last item the tail of the list
		// 	_tail = _freeItemsHead;
		// }
		// 
		// // Remove the first free item from the free items list
		// {
		// 	// If the free items head and tail are equal then there was only one free element so set both free items head and tail to nullptr
		// 	if (_freeItemsTail == _freeItemsHead)
		// 	{
		// 		_freeItemsHead = nullptr;
		// 		_freeItemsTail = nullptr;
		// 	}
		// 	// Set the sencond free item as the free items list head
		// 	else if (freeItemsHeadNext)
		// 	{
		// 		freeItemsHeadNext->_prev = nullptr;
		// 		_freeItemsHead = freeItemsHeadNext;
		// 	}
		// }
		// 
		// // Break the link between the last valid item and the first free item
		// _tail->_next = nullptr;
		// 
		// // Increment the number of stored elements
		// _size++;

		return *this;
	}

	/**
	* Removes the element at the given index from the list
	* Returns the list with the element removed
	* index: index of the element to remove
	* count: number of elements to remove starting at index
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& RemoveAt(int index, int count = 1, bool callElementDestructor = false)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			std::cerr << "List error [RemoveAt]: invalid index" << std::endl;
			return *this;
		}

		// Make sure count is valid
		if (count <= 0)
			return *this;

		// Memorize the index where we will start removing elements
		int indexRemoving = index;

		// Find the item to remove
		ListItem* itemToRemove = _head;
		while (index > 0 && itemToRemove != nullptr)
		{
			itemToRemove = itemToRemove->_next;
			index--;
		}

		// Make sure we have found the item and that it is valid
		if (index != 0 || itemToRemove == nullptr || itemToRemove->_isUsed == false)
		{
			std::cerr << "List error [RemoveAt]: cannot find the item or the item is not valid" << std::endl;
			return *this;
		}

		// Memorize the exact number of removed items
		int itemsRemoved = 0;

		ListItem* nextItemToRemove = itemToRemove->_next;
		while (count-- > 0 && itemToRemove != nullptr && itemToRemove->_isUsed == true && indexRemoving < _size)
		{
			// Move the item to the free items list
			RemoveElement(itemToRemove, callElementDestructor);

			// Intialize the data needed to remove the next item
			itemToRemove = nextItemToRemove;
			
			if(itemToRemove)
				nextItemToRemove = itemToRemove->_next;

			itemsRemoved++;
			indexRemoving++;
		}

		_size -= itemsRemoved;

		return *this;
	}

	/**
	* Removes the element at the given index from the list starting from the end of the list
	* Returns the list with the element removed
	* index: index of the element to remove from the end of the list (index 0 means the end of the list)
	* count: number of elements to remove starting at index and going backwards (e.g. count 2 and index 0 removes the last and the second-last elements int this sequence)
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& ReverseRemoveAt(int index, int count = 1, bool callElementDestructor = false)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			std::cerr << "List error [ReverseRemoveAt]: invalid index" << std::endl;
			return *this;
		}

		// Make sure count is valid
		if (count <= 0)
			return *this;

		// Memorize the index where we will start removing elements
		int indexRemoving = index;

		// Find the item to remove
		ListItem* itemToRemove = _tail;
		while (index < _size && itemToRemove != nullptr)
		{
			itemToRemove = itemToRemove->_prev;
			index++;
		}

		// Make sure we have found the item and that it is valid
		if (index != _size || itemToRemove == nullptr || itemToRemove->_isUsed == false)
		{
			std::cerr << "List error [ReverseRemoveAt]: cannot find the item or the item is not valid" << std::endl;
			return *this;
		}

		// Memorize the exact number of removed items
		int itemsRemoved = 0;

		ListItem* nextItemToRemove = itemToRemove->_prev;
		while (count-- > 0 && itemToRemove != nullptr && itemToRemove->_isUsed == true && indexRemoving < _size)
		{
			// Move the item to the free items list
			RemoveElement(itemToRemove, callElementDestructor);

			// Intialize the data needed to remove the next item
			itemToRemove = nextItemToRemove;

			if (itemToRemove)
				nextItemToRemove = itemToRemove->_prev;

			itemsRemoved++;
		}

		return *this;
	}

	/**
	* Removes the element at the given index from the list
	* Returns the list with the element removed
	* index: index of the element to remove
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& Remove(ConstReferenceType element, bool callElementDestructor = false)
	{
		// Find the item to remove
		ListItem* itemToRemove = _head;
		while (itemToRemove != nullptr && itemToRemove->_element != element)
			itemToRemove = itemToRemove->_next;

		// Make sure we have found the item and that it is valid
		if (itemToRemove == nullptr || itemToRemove->_isUsed == false)
		{
			std::cerr << "List error [Remove]: cannot find the item or the item is not valid" << std::endl;
			return *this;
		}

		// Move the item to the free items list
		RemoveElement(itemToRemove, callElementDestructor);
		
		return *this;
	}

	/**
	* Makes the iterator point to the next element inside the list and then removes the element pointed by the iterator from the list
	* Returns the list with the element removed
	* index: index of the element to remove
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& Remove(Iterator& it, bool callElementDestructor = false)
	{
		if (it._list != this)
			throw "List error [Remove]: cannot remove an element with an iterator from another list";
		
		ListItem* itemToRemove = it._item;

		// Make sure the item to remove is valid
		if (it._item == nullptr || it._item->_isUsed == false)
		{
			std::cerr << "List error [Remove]: cannot remove an invalid item" << std::endl;
			return *this;
		}

		// Make the iterator point to the next element
		it++;

		// Move the item to the free items list
		RemoveElement(itemToRemove, callElementDestructor);
		
		return *this;
	}

	/**
	* Makes the iterator point to the next element inside the list and then removes the element pointed by the iterator from the list
	* Returns the list with the element removed
	* index: index of the element to remove
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& Remove(ConstIterator& it, bool callElementDestructor = false)
	{
		if (it._list != this)
			throw "List error [Remove]: cannot remove an element with an iterator from another list";

		ListItem* itemToRemove = it._item;

		// Make sure the item to remove is valid
		if (it._item == nullptr || it._item->_isUsed == false)
		{
			std::cerr << "List error [Remove]: cannot remove an invalid item" << std::endl;
			return *this;
		}

		// Make the iterator point to the next element
		it++;

		// Move the item to the free items list
		RemoveElement(itemToRemove, callElementDestructor);

		return *this;
	}
	
	/**
	* Returns the element at the given index
	* If the index is invalid the value returned is the default specified on the template.
	* index: index of the element to get
	*/
	ReferenceType GetAt(int index)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			throw "List error [GetAt]: invalid index";
		}

		// Find the item to remove
		ListItem* item = _head;
		while (index > 0 && item != nullptr && item != _tail)
		{
			item = item->_next;
			index--;
		}

		// Make sure we have found the item and that it is valid
		if (index != 0 || item == nullptr || item->_isUsed == false)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			throw "List error [GetAt]: cannot find the item or the item is not valid";
		}

		// Return the element
		return item->_element;
	}

	/**
	* Returns the element at the given index
	* If the index is invalid the value returned is the default specified on the template.
	* index: index of the element to get
	*/
	ConstReferenceType GetAt(int index) const
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			throw "List error [GetAt]: invalid index";
		}

		// Find the item to remove
		ListItem* item = _head;
		while (index > 0 && item != nullptr && item != _tail)
		{
			item = item->_next;
			index--;
		}

		// Make sure we have found the item and that it is valid
		if (index != 0 || item == nullptr || item->_isUsed == false)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			throw "List error [GetAt]: cannot find the item or the item is not valid";
		}

		// Return the element
		return item->_element;
	}
	
	/**
	* Returns an iterator starting at the given index
	* index: index of the element to get
	*/
	Iterator GetIteratorAt(int index)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			throw "List error [GetAt]: invalid index";
		}

		// Find the item to remove
		ListItem* item = _head;
		while (index > 0 && item != nullptr && item != _tail)
		{
			item = item->_next;
			index--;
		}

		// Make sure we have found the item and that it is valid
		if (index != 0 || item == nullptr || item->_isUsed == false)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			throw "List error [GetAt]: cannot find the item or the item is not valid";
		}

		// Return the iterator
		return Iterator(this, item, item != _tail, item == _tail);
	}

	/**
	* Returns an iterator starting at the given index
	* index: index of the element to get
	*/
	ConstIterator GetIteratorAt(int index) const
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			throw "List error [GetAt]: invalid index";
		}

		// Find the item to remove
		ListItem* item = _head;
		while (index > 0 && item != nullptr && item != _tail)
		{
			item = item->_next;
			index--;
		}

		// Make sure we have found the item and that it is valid
		if (index != 0 || item == nullptr || item->_isUsed == false)
		{
			// Output the error to the standard output and throw an exception to stop the execution
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			throw "List error [GetAt]: cannot find the item or the item is not valid";
		}

		// Return the iterator
		return ConstIterator(this, item, item != _tail, item == _tail);
	}

	/**
	* Clears the list removing the elements
	* If elementsToKeep is more than 0, then a number of elementsToKeep items will be kept inside the list as unused
	* to prevent reallocation during next calls to Add.
	* If the capacity of the list is less than elementsToKeep, the list is grown to fit the gap.
	* elementsToKeep: number of elements to keep inside the list as unused
	* callElementDestructor: if true ElementDestructor will be called passing every element
	*/
	List& Clear(int elementsToKeep = 0, bool callElementDestructor = false)
	{
		int oldCapacity = _capacity;
		bool freeItemsHeadWasNullptr = _freeItemsHead == nullptr;
		bool freeItemsTailWasNullptr = _freeItemsTail == nullptr;
		bool freeItemsHeadPrevIsValid = _freeItemsHead != nullptr && _freeItemsHead->_prev && _freeItemsHead->_prev->_next != nullptr;
		bool grown = _capacity < elementsToKeep;
		bool keepElements = elementsToKeep > 0;

		if (_capacity < _size)
		{
			std::cerr << "List error [Clear]: the list contains more elements than it can store" << std::endl;
			return *this;
		}

		// If the size is already 0 then all the items are already inside the free items list
		if (_capacity > 0 && _size > 0)
		{
			// If the free items list is empty move all the used items to the free list items
			if (_freeItemsHead == nullptr)
			{
				if (_freeItemsTail != nullptr)
				{
					std::cerr << "List error [Clear]: _freeItemsHead is nullptr but _freeItemsTail is not. Invalid list" << std::endl;
					return *this;
				}

				_freeItemsHead = _head;
				_freeItemsTail = _tail;

				_head = nullptr;
				_tail = nullptr;
			}
			// Otherwise if there are used items move the used items at the end of the free items
			else if(_head != nullptr)
			{
				if (_tail == nullptr)
				{
					std::cerr << "List error [Clear]: _tail is nullptr but _head is not. Invalid list" << std::endl;
					return *this;
				}

				if (_freeItemsTail == nullptr)
				{
					std::cerr << "List error [Clear]: _freeItemsHead is not nullptr but _freeItemsTail is. Invalid list" << std::endl;
					return *this;
				}

				// Put the valid items list at the end of the free items list
				_freeItemsTail->_next = _head;
				_head->_prev = _freeItemsTail;

				_freeItemsTail = _tail;

				// Set the used items list head and tail as nullptr to mark the used items list as empty
				_head = nullptr;
				_tail = nullptr;
			}
			else
			{
				std::cerr << "List error [Clear]: the list has a size and capacity but both the heads are not valid" << std::endl;
				return *this;
			}
		}

		// if (_capacity > 0)
		// {
		// 	// Only if _size is more than 0 then there are used items to put at the end of the free list
		// 	if (_size > 0)
		// 	{
		// 		// If the free items list is empty put at least one element inside of it to simplify the next code
		// 		if (_freeItemsHead == nullptr)
		// 		{
		// 			if (_freeItemsTail != nullptr)
		// 			{
		// 				std::cerr << "List error [Clear]: _freeItemsHead is nullptr but _freeItemsTail is not. Invalid list" << std::endl;
		// 				return *this;
		// 			}
		// 			
		// 			// Bring the valid items list head to the free items list
		// 			_freeItemsHead = _head;
		// 			_freeItemsTail = _freeItemsHead;
		// 
		// 			if (_head->_next)
		// 				_head->_next->_prev = nullptr;
		// 
		// 			if (_head == _tail)
		// 				_tail = nullptr;
		// 
		// 			_head = _head->_next;
		// 
		// 			_freeItemsHead->_next = nullptr;
		// 			_freeItemsTail->_next = nullptr;
		// 		}
		// 
		// 		// Check again if _head is valid since the previous code could have set it to nullptr if there was only one valid element inside the list
		// 		if (_head)
		// 		{
		// 			if (freeItemsHeadWasNullptr == false)
		// 			{
		// 				int j = 0;
		// 				j++;
		// 			}
		// 
		// 			if (_tail == nullptr)
		// 			{
		// 				std::cerr << "List error [Clear]: _tail is nullptr but _head is not. Invalid list" << std::endl;
		// 				return *this;
		// 			}
		// 
		// 			// Put the valid items list at the end of the free items list
		// 			_freeItemsTail->_next = _head;
		// 			_head->_prev = _freeItemsTail;
		// 
		// 			_freeItemsTail = _tail;
		// 
		// 			// Set the used items list head and tail as nullptr to mark the used items list as empty
		// 			_head = nullptr;
		// 			_tail = nullptr;
		// 		}
		// 	}
		// }

		freeItemsHeadPrevIsValid = _freeItemsHead != nullptr && _freeItemsHead->_prev && _freeItemsHead->_prev->_next != nullptr;

		// If the list cannot hold the number of elements that the calee wants to keep, grow the list to fill the gap
		if (_capacity < elementsToKeep)
			Grow(elementsToKeep - _capacity);
		// Otherwise set the capacity of the list equals to the number of elements to keep
		else
			_capacity = elementsToKeep;

		freeItemsHeadPrevIsValid = _freeItemsHead != nullptr && _freeItemsHead->_prev && _freeItemsHead->_prev->_next != nullptr;

		ListItem* currentItem = _freeItemsHead;
		ElementDtor dtor;

		// Mark the element to keep as not used
		while (elementsToKeep > 0 && currentItem != nullptr)
		{
			// Call ElementDtor only if the item points to a valid element
			if (currentItem->_isUsed && callElementDestructor)
				dtor(currentItem->_element);

			currentItem->_isUsed = false;
			currentItem = currentItem->_next;
			elementsToKeep--;
		}

		freeItemsHeadPrevIsValid = _freeItemsHead != nullptr && _freeItemsHead->_prev && _freeItemsHead->_prev->_next != nullptr;

		// If we have elements to delete from the empty list
		if (currentItem)
		{
			// If we are deleting the rest of the free items and we start from the free items list head
			// we will be deleting all the free list items so set both the free list items head and tail to nullptr
			if (currentItem == _freeItemsHead)
			{
				_freeItemsHead = nullptr;
				_freeItemsTail = nullptr;
			}
			else
			{
				// Set _freeItemsTail as the last item on the free items list
				_freeItemsTail = currentItem->_prev;

				// Make sure that the free items list tail doesn't point to a next item
				if (_freeItemsTail)
					_freeItemsTail->_next = nullptr;
			}

			int cycles = 0;

			// Delete all the remaining elements
			while (currentItem != nullptr)
			{
				ListItem* nextItem = currentItem->_next;

				// // Ensure that currentItem previous item doesn't point to currentItem otherwise there
				// // will be errors because _freeItemsTail would have _next to point to dirty memory
				// if (currentItem->_prev)
				// 	currentItem->_prev->_next = nullptr;

				// Call ElementDtor only if the item points to a valid element
				if (currentItem->_isUsed && callElementDestructor)
					dtor(currentItem->_element);

				delete currentItem;
				currentItem = nextItem;

				freeItemsHeadPrevIsValid = _freeItemsHead != nullptr && _freeItemsHead->_prev && _freeItemsHead->_prev->_next != nullptr;

				cycles++;
			}

			//// If the tail is nullptr then we have cleared all the list
			//// so set _freeItemsHead to nullptr too
			//if (_freeItemsTail == nullptr)
			//	_freeItemsHead = nullptr;
		}
		
		// Set the size to 0 to mark the list as empty
		_size = 0;

		return *this;
	}
	
	/**
	* Searches the desired element inside of the list and returns its index
	* It the element isn't found this returns -1
	*/
	int Find(ConstReferenceType element) const
	{
		int index = 0;

		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_element != element && item->_isUsed)
		{
			// Increment the index
			index++;

			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return index;

		// The element wasn't found
		return -1;
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* Is the element isn't found -1 is returned.
	* comparator: function object that must implement a member function compliant to the below specifications
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> int Find(Comparator comparator) const
	{
		int index = 0;

		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure)
		while (item != nullptr && item->_isUsed && comparator(item->_element) == false)
		{
			// Increment the index
			index++;

			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return index;

		// The element wasn't found
		return -1;
	}
	
	/** Searches the desired element inside of the list and returns an iterator to it */
	Iterator FindIterator(ConstReferenceType element)
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_element != element && item->_isUsed)
		{
			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return Iterator(this, item, item != _tail, item == _tail);

		// Return the iterator
		return Iterator(this, nullptr, false, false);
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* comparator: function object that must implement a member function compliant to the below specifications
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> Iterator FindIterator(Comparator comparator)
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure)
		while (item != nullptr && item->_isUsed && comparator(item->_element) == false)
		{
			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return Iterator(this, item, item != _tail, item == _tail);

		// The element wasn't found
		return Iterator(this, nullptr, false, false);
	}

	/** Searches the desired element inside of the list and returns an iterator to it */
	ConstIterator FindIterator(ConstReferenceType element) const
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_element != element && item->_isUsed)
		{
			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return ConstIterator(this, item, item != _tail, item == _tail);

		// Return the iterator
		return ConstIterator(this, nullptr, false, false);
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* comparator: function object that must implement a member function compliant to the below specifications
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> ConstIterator FindIterator(Comparator comparator) const
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure)
		while (item != nullptr && item->_isUsed && comparator(item->_element) == false)
		{
			// Move to the next item
			item = item->_next;
		}

		// If the item is valid and it used then we have found the element we are looking for
		// so return the index
		if (item != nullptr && item->_isUsed)
			return ConstIterator(this, item, item != _tail, item == _tail);

		// The element wasn't found
		return ConstIterator(this, nullptr, false, false);
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* Is the element isn't found the default value for the list element is returned and found is set to false.
	* comparator: function object that must implement a member function compliant to the below specifications
	* found: if the element the user is looking for in found this gets set to true, false otherwise
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> ReferenceType FindElement(Comparator comparator, bool& found)
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_isUsed)
		{
			// Call the comparator function object and if it returns true
			// then the element is the one the user is looking for
			if (comparator(item->_element))
			{
				found = true;
				return item->_element;
			}

			// Move to the next item
			item = item->_next;
		}

		// The element wasn't found so set found to false and return the list's element default value
		found = false;
		return DefaultValue();
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* Is the element isn't found the default value for this list element is returned.
	* comparator: function object that must implement a member function compliant to the below specifications
	* found: if the element the user is looking for in found this gets set to true, false otherwise
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> ConstReferenceType FindElement(Comparator comparator, bool& found) const
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_isUsed)
		{
			// Call the comparator function object and if it returns true
			// then the element is the one the user is looking for
			if (comparator(item->_element))
			{
				found = true;
				return item->_element;
			}

			// Move to the next item
			item = item->_next;
		}

		// The element wasn't found so return the default
		found = false;
		return DefaultValue();
	}

	/**
	* This method searches for an element inside the list using a given comparator to see if an element is the one the user is looking for.
	* Is the element isn't found the given alternative is returned and found is set to false.
	* comparator: function object that must implement a member function compliant to the below specifications
	* found: if the element the user is looking for in found this gets set to true, false otherwise
	* alternative: value that is returned if the element which is being searched isn't found
	*
	* Comparator member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element);
	*/
	template<typename Comparator> ConstReferenceType FindElement(Comparator comparator, bool& found, ConstReferenceType alternative) const
	{
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_isUsed)
		{
			// Call the comparator function object and if it returns true
			// then the element is the one the user is looking for
			if (comparator(item->_element))
			{
				found = true;
				return item->_element;
			}

			// Move to the next item
			item = item->_next;
		}
		
		// The element wasn't found so return the default
		found = false;
		return alternative;
	}

	/**
	* This method loops through all the elements inside the list and excecutes the given function passing the element
	* function: function object that is excecuted giving each element of the list, one a the time.
	*			Must implement a member function compliant to the below specifications.
	*			If the function object returns false, then the loop stops.
	*
	* Function object member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element, int index, bool lastElement);
	*/
	template<typename Function> void ForEach(Function function)
	{
		int index = 0;
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_isUsed)
		{
			// Call the function object and if it returns false then stop the loop
			if (function(item->_element, index, index == (_size - 1)) == false)
				return;

			// Move to the next item
			item = item->_next;
			index++;
		}
	}

	/**
	* This method loops through all the elements inside the list and excecutes the given function passing the element
	* function: function object that is excecuted giving each element of the list, one a the time.
	*			Must implement a member function compliant to the below specifications.
	*			If the function object returns false, then the loop stops.
	*
	* Function object member function signature specifications:
	*	bool operator()([ValueType | ReferenceType | ConstReferenceType] element, int index, bool lastElement);
	*/
	template<typename Function> void ForEach(Function function) const
	{
		int index = 0;
		ListItem* item = _head;

		// Loop until the item is not valid, the element contained by the item is not the one we are looking for
		// the item is not used (should never happen, but to be sure) 
		while (item != nullptr && item->_isUsed)
		{
			// Call the function object and if it returns false then stop the loop
			if (function(item->_element, index, index == (_size - 1)) == false)
				return;

			// Move to the next item
			item = item->_next;
			index++;
		}
	}

	/**
	* Sets the size of the list growing or shrinking accordingly.
	* size: the new size that the list will have
	* element: default values that will be set to the elements if the new size is bigger than the current one
	* callElementsDestructor: if the new size is smaller than the current one and this parameter is given as true then the element destructor will be called for the elements to remove
	*/
	void SetSize(int size, ConstReferenceType element, bool callElementsDestructor = false)
	{
		// If the size is the name we can finish immediately
		if (size == _size)
			return;
		
		// If the new size is smaller than the current one we simply remove
		// the excess and move it to the free items list
		if (size < _size)
			RemoveAt(size, _size - size, callElementsDestructor);
		// Otherwise we need to add elements to the list
		else
		{
			// If the new size is bigger than the list capacity it will also be bigger than the list size
			// so grow the number elements that the list can store to speed up the Add method needed
			if (size > _capacity)
				Grow(size - _capacity);

			// Compute the number of elements to add
			int amount = size - _size;

			// Add the necessary elements
			// The Add method won't need to grow the list since a call to Grow has been made before
			while (amount-- > 0)
				Add(element);
		}
	}

	/** Returns the number of elements inside the list */
	int GetSize() const { return _size; }

	/** Returns the number of elements that the list can store without the need to allocate more memory */
	int GetCapacity() const { return _capacity; }

	/** Returns an iterator that points to the first element of the list */
	Iterator Begin() { return Iterator(this, _head, true, false); }

	/** Returns an iterator that points to the first element of the list */
	ConstIterator Begin() const { return ConstIterator(this, _head, true, false); }
	
	/** Returns an iterator that points to the last element of the list */
	Iterator End() { return Iterator(this, _tail, false, true); }

	/** Returns an iterator that points to the last element of the list */
	ConstIterator End() const { return ConstIterator(this, _tail, false, true); }

	/** Returns the first element of the list. If the list has no elements this throws an exception */
	ReferenceType Front()
	{
		if (_size == 0)
			throw "List error[Front]: the list has no elements";
		return *(Begin());
	}

	/** Returns the first element of the list. If the list has no elements this throws an exception */
	ConstReferenceType Front() const
	{
		if (_size == 0)
			throw "List error[Front]: the list has no elements";
		return *(Begin());
	}
	
	/** Returns the last element of the list. If the list has no elements this throws an exception */
	ReferenceType Back()
	{
		if (_size == 0)
			throw "List error[Back]: the list has no elements";
		return *(End());
	}

	/** Returns the last element of the list. If the list has no elements this throws an exception */
	ConstReferenceType Back() const
	{
		if (_size == 0)
			throw "List error[Back]: the list has no elements";
		return *(End());
	}
	
private:
	/** Pointer to the first item on the list */
	ListItem* _head;

	/** Pointer to the last item on the list */
	ListItem* _tail;

	/** Pointer to the first free item of the list */
	ListItem* _freeItemsHead;

	/** Pointer to the last free item of the list */
	ListItem* _freeItemsTail;
	
	/** The number of valid objects inside the list */
	int _size;

	/** The number of objects that the list can contain */
	int _capacity;

	/**
	* The number of items that will be added to the list every time that the list is full and a new element is added
	* If this is 0 then the grow amount will be computed runtime
	*/
	int _growBy;
};