#pragma once

#include <iostream>

template<typename T, T Default = 0>
class List
{
public:
	/** Useful typedefs */
	typedef T ValueType;
	typedef T& ReferenceType;
	typedef T const& ConstReferenceType;
	typedef T* PointerType;
	typedef T const* ConstPointerType;

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
		ListItem(ValueType element = Default, ListItem* next = nullptr, ListItem* prev = nullptr, bool isUsed = false)
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
			: _element(src._element)
			, _next(src._next)
			, _prev(src._prev)
			, _isUsed(src._isUsed)
		{
			src._next = nullptr;
			src._prev = nullptr;
			src._isUsed = false;
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
				_element = src._element;
				_next = src._next;
				_prev = src._prev;
				_isUsed = src._isUsed;
				
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
	/**
	* Default constructor
	* startingSize:	the number of (uninitialized) elements that the list will contain to start with (to prevent reallocations)
	* growBy: the number of items that will be added to the list every time that the list is full and a new element is added
	*/
	List(int startingSize = 0, int growBy = 0)
		: _head(nullptr)
		, _tail(nullptr)
		, _freeItemsHead(src._freeItemsHead)
		, _freeItemsTail(src._freeItemsTail)
		, _size(0)
		, _capacity(0)
		, _growBy(growBy)
	{
		// Setup the list
		Grow(ComputeGrowAmount());
	}

	/**
	* Copy constructor
	* We are in the constructor so we still initialize the members to an invalid state so that Copy won't crash
	*/
	List(const List& src)
		: _head(nullptr)
		, _tail(nullptr)
		, _currentItem(nullptr)
		, _size(0)
		, _capacity(0)
		, _growBy(growBy)
	{
		Copy(src);
	}

	/** Move constructor */
	List(List&& src)
		: _head(std::move(src._head))
		, _tail(std::move(src._tail))
		, _freeItemsHead(std::move(src._freeItemsHead))
		, _freeItemsTail(std::move(src._freeItemsTail))
		, _size(src._size)
		, _capacity(src._capacity)
		, _growBy(src._growBy)
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
		Clear();
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

private:
	void Copy(const List& src)
	{
		// If the source's heads are nullptr or it has no capacity then the source list is invalid
		if ((src._head == nullptr && src._freeItemsHead == nullptr) || src._capacity <= 0)
			return;

		if (_head != nullptr || _freeItemsHead != nullptr)
		{
			// Clear the list and makes sure that this lists capacity matches the source list capacity
			Clear(src._capacity);
		}
		else
		{
			// If the heada are nullptr then Copy is being called from the copy constructor since otherwise they are ALWAYS not nullptr
			// Call grow to initialize the pointers and have the same capacity as the source
			Grow(src._capacity);
		}

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
		// First initialization of the list
		if (_head == nullptr && _freeItemsHead = nullptr)
		{
			if (_tail != nullptr || _freeItemsTail != nullptr)
			{
				std::cerr << "List error [Grow]: inconsisten list.";
				return;
			}

			// Create the first free element and setup all the pointers and the sizes
			_freeItemsHead = new ListItem();
			_freeItemsTail = _freeItemsHead;

			_head = nullptr;
			_tail = nullptr;
			_capacity = 1;
			_size = 0;
			
			// Since we just added an item, the head, decrease the number of items to add
			amount--;
		}

		// Add the items to the list
		while (amount > 0)
		{
			// Create a new item
			ListItem* newItem = new ListItem();

			// Set the previous item of the new item as the current tail of the list
			newItem->_prev = _freeItemsTail;

			// Set the next item of the current tail of the list as the new item
			_freeItemsTail->_next = newItem;

			// Change the last free item of the list to be the new item
			_freeItemsTail = newItem;

			// If _freeItemsHead is nullptr then a call to Add was made and the list has been filled
			// so make _freeItemsHead point to the first free item
			if (_freeItemsHead == nullptr)
				_freeItemsHead = newItem;

			amount--;
		}

		_capacity += amount;
	}

	/**
	* Computes the number of items to add to the list
	* It returns _growBy if it's more than 0, otherwise returns 1/10 of the current capacity clamped between [1..20]
	*/
	int ComputeGrowAmount() const
	{
		if (_growBy > 0)
			return _growBy;

		int grow = _capacity / 10;

		// Clamp the growth between 1 and 20
		if (grow < 1) grow = 1;
		if (grow > 20) grow = 20;

		return grow;
	}

public:
	List& Add(ValueType element)
	{
		// If the capacity and size are the same then the list is full, so make it grow
		if (_capacity == _size)
			Grow(ComputeGrowAmount());

		if (_freeItemsHead == nullptr)
		{
			std::cerr << "List error [Add]: _currentItem is nullptr. The list is invalid";
			return *this;
		}

		// Save the element inside the first free item
		_freeItemsHead->_element = element;

		// If _head is nullptr the list was empty so set it up
		if (_head == nullptr)
		{
			// Head and tail are the same
			_head = _tail = _freeItemsHead;
			
			_head->_next = nullptr;
			_head->_prev = nullptr;
		}
		else
		{
			// Make the new last item _prev point to the current tail of the list
			_freeItemsHead->_prev = _tail;

			// Make the current list tail _next point to the new last item
			_tail->_next = _freeItemsHead;

			// Make the new last item the tail of the list
			_tail = _freeItemsHead;
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
			else if (_freeItemsHead->_next)
			{
				_freeItemsHead->_next->_prev = nullptr;
				_freeItemsHead = _freeItemsHead->_next
			}
		}

		return *this;
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