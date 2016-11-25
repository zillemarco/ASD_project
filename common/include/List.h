#pragma once

#include <iostream>
#include <type_traits>

/** Default element destructor used by List. It does nothing */
template<typename T> struct ListDefaultElementDestructor
{
	typedef T& ReferenceType;
	inline void operator()(ReferenceType element) { }
};

/**
* Utility template to have a default value to use inside of some methods of List
* The user have to define his own default value doing a template specialization of this struct.
* 
* For example, if the user defines a class A to be used by a List a possible template specialization is:
*	template<> struct ListElementDefaultValue<A>
*	{
*		static A Value() { return  A(); }
*	};
*/
template<typename T> struct ListElementDefaultValue
{
	//static_assert(false, "A default value wasn't specified for the type.");
};

/** Default value for basic types */
template<> struct ListElementDefaultValue<int> { inline static int Value() { return 0; } };
template<> struct ListElementDefaultValue<unsigned int> { inline static unsigned int Value() { return 0; } };
template<> struct ListElementDefaultValue<long> { inline static long Value() { return 0; } };
template<> struct ListElementDefaultValue<unsigned long> { inline static unsigned long Value() { return 0; } };
template<> struct ListElementDefaultValue<long long> { inline static long long Value() { return 0; } };
template<> struct ListElementDefaultValue<unsigned long long> { inline static unsigned long long Value() { return 0; } };
template<> struct ListElementDefaultValue<float> { inline static float Value() { return 0.0f; } };
template<> struct ListElementDefaultValue<double> { inline static double Value() { return 0; } };

template<
	typename T, /** Type of objects to be stored inside the list */
	typename ElementDtor = ListDefaultElementDestructor<T>, /** Function object which is called by Clear, RemoveAt, Remove or the list destructor */
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

private:
	void Copy(const List& src)
	{
		// If the source's heads are nullptr or it has no capacity then the source list is invalid
		if ((src._head == nullptr && src._freeItemsHead == nullptr) || src._capacity <= 0)
			return;

		if (_head != nullptr || _freeItemsHead != nullptr)
		{
			// Clear the list and makes sure that this lists capacity matches the source list capacity
			Clear(src._capacity, CallElementDestructorOnListCopy);
		}
		else
		{
			// If the heads are nullptr then Copy is being called from the copy constructor since otherwise they are ALWAYS not nullptr
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
			_freeItemsHead = new ListItem(DefaultValue());
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

		int grow = _capacity / 10;

		// Clamp the growth between 1 and 20
		if (grow < 1) grow = 1;
		if (grow > 20) grow = 20;

		return grow;
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
		if (_freeItemsTail && _freeItemsTail->_next)
			_freeItemsTail->_next = itemToRemove;
		_freeItemsTail = itemToRemove;

		// If there is no free items list head then the valid list was full and the free list was empty,
		// so set the free list head to point to element just freed
		if (_freeItemsHead == nullptr)
			_freeItemsHead = _freeItemsTail;

		// Decrement the size
		_size--;
	}

	/** Utility method used by some method which are non-const */
	ReferenceType DefaultValue()
	{
		static ValueType s_defaultValue = ListElementDefaultValue<ValueType>::Value();
		return s_defaultValue;
	}

	/** Utility method used by some method which are const */
	ConstReferenceType DefaultValue() const
	{
		static ValueType s_defaultValue = ListElementDefaultValue<ValueType>::Value();
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
	* callElementDestructor: if true ElementDestructor will be called passing the element which is being removed
	*/
	List& RemoveAt(int index, bool callElementDestructor = false)
	{
		// Make sure the index is valid
		if (index < 0 || index >= _size)
		{
			std::cerr << "List error [RemoveAt]: invalid index" << std::endl;
			return *this;
		}

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

		// Move the item to the free items list
		RemoveElement(itemToRemove, callElementDestructor);

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
	* Clears the list removing the elements
	* If elementsToKeep is more than 0, then a number of elementsToKeep items will be kept inside the list as unused
	* to prevent reallocation during next calls to Add.
	* If the capacity of the list is less than elementsToKeep, the list is grown to fit the gap.
	* elementsToKeep: number of elements to keep inside the list as unused
	* callElementDestructor: if true ElementDestructor will be called passing every element
	*/
	List& Clear(int elementsToKeep = 0, bool callElementDestructor = false)
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

			// Call ElementDtor only if the item points to a valid element
			if (currentItem->_isUsed && callElementDestructor)
				dtor(currentItem->_element);

			delete currentItem;
			currentItem = nextItem;
		}

		// If the tail is nullptr then we have cleared all the list
		// se set _freeItemsHead to nullptr too
		if(_freeItemsTail == nullptr)
			_freeItemsHead = nullptr;
		
		// Set the size to 0 to mark the list as empty
		_size = 0;

		return *this;
	}

	/**
	* Searched the desired element inside of the list and returns its index
	* It the element isn't found this returns -1
	*/
	int Find(const ValueType& element) const
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

	/** Returns the number of elements inside the list */
	int GetSize() const { return _size; }

	/** Returns the number of elements that the list can store without the need to allocate more memory */
	int GetCapacity() const { return _capacity; }
	
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