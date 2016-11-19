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

		~ListItem()
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
		if (_head == nullptr && _freeItemsHead == nullptr)
		{
			if (_tail != nullptr || _freeItemsTail != nullptr)
			{
				std::cerr << "List error [Grow]: inconsisten list." << std::endl;
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

		int grow = _capacity / 10;

		// Clamp the growth between 1 and 20
		if (grow < 1) grow = 1;
		if (grow > 20) grow = 20;

		return grow;
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
			std::cerr << "List error [Add]: _currentItem is nullptr. The list is invalid" << std::endl;
			return *this;
		}

		// Save the element inside the first free item
		_freeItemsHead->_element = element;
		_freeItemsHead->_isUsed = true;

		// Memorize the current free items list head next element to use it later since
		// if _head is nullptr _head->_next is put to nullptr, which is the same as _freeItemsHead->_next
		// and it can break the list links
		ListItem* freeItemsHeadNext = _freeItemsHead->_next;

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
			else if (freeItemsHeadNext)
			{
				freeItemsHeadNext->_prev = nullptr;
				_freeItemsHead = freeItemsHeadNext;
			}
		}

		// Break the link between the last valid item and the first free item
		_tail->_next = nullptr;

		// Increment the number of stored elements
		_size++;

		return *this;
	}

	/**
	* Removes the element at the given index from the list
	* Returns the list with the element removed
	* index: index of the element to remove
	*/
	List& RemoveAt(int index)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			std::cerr << "List error [RemoveAt]: invalid index" << std::endl;
			return *this;
		}

		// Find the item to remove
		ListItem* itemToRemove = _head;
		while (index > 0 && itemToRemove != nullptr && itemToRemove != _tail)
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
		if(itemToRemove->_next)
			itemToRemove->_next->_prev = itemToRemove->_prev;

		// Put the item to remove at the end of the free items list
		itemToRemove->_next = nullptr;
		itemToRemove->_prev = _freeItemsTail;

		// If there is a free items list tail, change it to point to the item to remove
		if(_freeItemsTail && _freeItemsTail->_next)
			_freeItemsTail->_next = itemToRemove;
		_freeItemsTail = itemToRemove;

		// If there is no free items list head then the valid list was full and the free list was empty,
		// so set the free list head to point to element just freed
		if (_freeItemsHead == nullptr)
			_freeItemsHead = _freeItemsTail;

		// Decrement the size
		_size--;

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
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			return Default;
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
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			return Default;
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
			std::cerr << "List error [GetAt]: invalid index" << std::endl;
			return Default;
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
			std::cerr << "List error [GetAt]: cannot find the item or the item is not valid" << std::endl;
			return Default;
		}

		// Return the element
		return item->_element;
	}

	/**
	* Clears the list removing the elements
	* If elementsToKeep is more than 0, then a number of elementsToKeep items will be kept inside the list as unused
	* to prevent reallocation during next calls to Add.
	* If the capacity of the list is less than elementsToKeep, the list is grown to fit the gap.
	* elementsToKeep: number of elements to keep inside the list as unused
	*/
	List& Clear(int elementsToKeep = 0)
	{
		if (_capacity > 0)
		{
			// Only if _size is more than 0 then there are used items to put at the end of the free list
			if (_size > 0)
			{
				// If the free items list is empty put at least one element inside of it to simplify the next code
				if (_freeItemsHead == nullptr)
				{
					if (_freeItemsTail != nullptr)
					{
						std::cerr << "List error [Clear]: _freeItemsHead is nullptr but _freeItemsTail is not. Invalid list" << std::endl;
						return *this;
					}
					
					// Bring the valid items list head to the free items list
					_freeItemsHead = _head;
					_freeItemsTail = _freeItemsHead;

					if (_head->_next)
						_head->_next->_prev = nullptr;

					if (_head == _tail)
						_tail = nullptr;

					_head = _head->_next;
				}

				// Check again if _head is valid since the previous code could have set it to nullptr if there was only one valid element inside the list
				if (_head)
				{
					if (_tail == nullptr)
					{
						std::cerr << "List error [Clear]: _tail is nullptr but _head is not. Invalid list" << std::endl;
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
			}
		}

		// If the list cannot hold the number of elements that the calee wants to keep, grow the list to fill the gap
		if (_capacity < elementsToKeep)
			Grow(elementsToKeep - _capacity);
		// Otherwise set the capacity of the list equals to the number of elements to keep
		else
			_capacity = elementsToKeep;

		ListItem* currentItem = _freeItemsHead;

		// Mark the element to keep as not used
		while (elementsToKeep > 0 && currentItem != nullptr)
		{
			currentItem->_isUsed = false;
			currentItem = currentItem->_next;
			elementsToKeep--;
		}

		// Set _freeItemsTail as the last item on the free items list
		if(currentItem)
			_freeItemsTail = currentItem->_prev;

		// Delete all the remaining elements
		while (currentItem != nullptr)
		{
			ListItem* nextItem = currentItem->_next;

			// Ensure that currentItem previous item doesn't point to currentItem otherwise there
			// will be errors because _freeItemsTail would have _next to point to dirty memory
			if (currentItem->_prev)
				currentItem->_prev->_next = nullptr;

			delete currentItem;
			currentItem = nextItem;
		}

		_size = 0;

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