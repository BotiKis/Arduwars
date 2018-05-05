#pragma once

/*
   Copyright (C) 2018 Pharap (@Pharap)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdint.h>

#include "TypeTraits.h"

//
// Declarations
//

template< typename Type, uint8_t Capacity >
class Deque;

template< typename Type, uint8_t CapacityValue >
class Deque
{
public:

	//
	// Constraints
	//

	static_assert(CapacityValue > 0, "Attempt to create Deque with a capacity less than 1");
	static_assert(CapacityValue < 128, "Attempt to create Deque with a capacity greater than 127");

	//
	// Type Aliases
	//

	using ValueType = Type;
	using SizeType = uint8_t;
	using IndexType = uint8_t;
	using IndexOfType = int8_t;
	
	//
	// Constants
	//
	
	constexpr static const SizeType Capacity = CapacityValue;
	constexpr static const IndexOfType InvalidIndex = -1;
	constexpr static const IndexType FirstIndex = 0;
	constexpr static const IndexType LastIndex = Capacity - 1;

private:
	
	//
	// Member Variables
	//
	
	ValueType items[Capacity] = {};
	IndexType next = 0;
	
public:

	//
	// Common Member Functions
	//
	
	// O(1)
	bool isEmpty(void) const noexcept
	{
		return (this->next == FirstIndex);
	}
	
	// O(1)
	bool isFull(void) const noexcept
	{
		return (this->next > LastIndex);
	}
	
	// O(1)
	SizeType getCount(void) const noexcept
	{
		return this->next;
	}
	
	// O(1)
	constexpr SizeType getCapacity(void) const noexcept
	{
		return Capacity;
	}
	
	// O(1)
	ValueType * getData(void) noexcept
	{
		return &this->items[FirstIndex];
	}
	
	// O(1)
	const ValueType * getData(void) const noexcept
	{
		return &this->items[FirstIndex];
	}
	
	// O(1)
	ValueType & operator [](IndexType index)
	{
		return this->items[index];
	}
	
	// O(1)
	const ValueType & operator [](IndexType index) const
	{
		return this->items[index];
	}
	
	// O(N)
	void clear(void);
	
	// O(N)
	void fill(const ValueType & item);
	
	// O(N)
	bool contains(const ValueType & item) const;
	
	// O(N)
	IndexOfType indexOfFirst(const ValueType & item) const;
	
	// O(N)
	IndexOfType indexOfLast(const ValueType & item) const;
	
public:

	//
	// Specific Member Functions
	//

	// O(1)
	ValueType & getFirst(void)
	{
		return this->items[FirstIndex];
	}

	// O(1)
	const ValueType & getFirst(void) const
	{
		return this->items[FirstIndex];
	}

	// O(1)
	ValueType & getLast(void)
	{
		return this->items[this->next];
	}

	// O(1)
	const ValueType & getLast(void) const
	{
		return this->items[this->next];
	}

	// O(1)
	bool append(const ValueType & item);

	// O(1)
	bool prepend(const ValueType & item);

	// O(1)
	void unappend(void);

	// O(1)
	void unprepend(void);
	
	// O(N)
	bool removeFirst(const ValueType & item);
	
	// O(N)
	bool removeLast(const ValueType & item);
	
	// O(N)
	bool removeAt(IndexType index);

	// O(N)
	bool insert(IndexType index, const ValueType & item);
};

//
// Definition
//

// O(N)
template< typename Type, uint8_t Capacity >
void Deque<Type, Capacity>::clear(void)
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		this->items[i].~ValueType();
	this->next = 0;
}

// O(N)
template< typename Type, uint8_t Capacity >
void Deque<Type, Capacity>::fill(const ValueType & item)
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		this->items[i] = item;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::contains(const ValueType & item) const
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		if (this->items[i] == item)
			return true;
	return false;
}

// O(N)
template< typename Type, uint8_t Capacity >
auto Deque<Type, Capacity>::indexOfFirst(const ValueType & item) const -> IndexOfType
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		if (this->items[i] == item)
			return i;
	return InvalidIndex;
}

// O(N)
template< typename Type, uint8_t Capacity >
auto Deque<Type, Capacity>::indexOfLast(const ValueType & item) const -> IndexOfType
{
	for (IndexType i = this->getCount() - 1; i > 0; --i)
		if (this->items[i] == item)
			return i;
	return InvalidIndex;
}

// O(1)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::append(const ValueType & item)
{
	if (this->isFull())
		return false;

	this->items[this->next] = item;
	++this->next;
	return true;
}

// O(1)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::prepend(const ValueType & item)
{
	if (this->isFull())
		return false;

	++this->next;
	for (IndexType i = this->next; i > 0; --i)
			this->items[i] = stdlib::move(this->items[i - 1]);
	this->items[0] = item;
	return true;
}

// O(1)
template< typename Type, uint8_t Capacity >
void Deque<Type, Capacity>::unappend(void)
{
	if (this->isEmpty())
		return;

	--this->next;
	this->items[this->next].~ValueType();
	return true;
}

// O(1)
template< typename Type, uint8_t Capacity >
void Deque<Type, Capacity>::unprepend(void)
{
	if (this->isEmpty())
		return;

	--this->next;
	for (IndexType i = 0; i < this->next; ++i)
			this->items[i] = stdlib::move(this->items[i + 1]);
	this->items[this->next].~ValueType();
	return true;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::removeFirst(const ValueType & item)
{
	for(IndexType i = 0; i < this->next; ++i)
		if (this->items[i] == item)
		{
			--this->next;
			for (IndexType j = i; j < this->next; ++j)
				this->items[j] = stdlib::move(this->items[j + 1]);
			this->items[this->next].~ValueType();
			return true;
		}
	return false;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::removeLast(const ValueType & item)
{
	for(IndexType i = this->next - 1; i > 0; --i)
		if (this->items[i] == item)
		{
			--this->next;
			for (IndexType j = i; j < this->next; ++j)
				this->items[j] = stdlib::move(this->items[j + 1]);
			this->items[this->next].~ValueType();
			return true;
		}
	return false;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::removeAt(IndexType index)
{
	if(index >= this->next)
		return false;

	--this->next;
	for (IndexType i = index; i < this->next; ++i)
			this->items[i] = stdlib::move(this->items[i + 1]);
	this->items[this->next].~ValueType();
	return true;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Deque<Type, Capacity>::insert(IndexType index, const ValueType & item)
{
	if(index >= this->next)
		return false;

	if (this->isFull())
		return false;

	++this->next;
	for (IndexType i = index + 1; i < this->next; ++i)
		this->items[i] = stdlib::move(this->items[i + 1]);
	this->items[index] = item;
	return true;
}

//
// Empty Deque
//

template< typename Type >
class Deque<Type, 0>
{
public:

	//
	// Type Aliases
	//

	using ValueType = Type;
	using SizeType = uint8_t;
	using IndexType = uint8_t;
	using IndexOfType = int8_t;
	
	//
	// Constants
	//
	
	constexpr static const SizeType Capacity = 0;
	constexpr static const IndexOfType InvalidIndex = -1;
	constexpr static const IndexType FirstIndex = InvalidIndex;
	constexpr static const IndexType LastIndex = InvalidIndex;
	
public:

	//
	// Common Member Functions
	//
	
	// O(1)
	constexpr bool isEmpty(void) const
	{
		return true;
	}
	
	// O(1)
	constexpr bool isFull(void) const noexcept
	{
		return true;
	}
	
	// O(1)
	constexpr SizeType getCount(void) const noexcept
	{
		return 0;
	}
	
	// O(1)
	constexpr SizeType getCapacity(void) const noexcept
	{
		return 0;
	}
	
	// O(1)
	/*constexpr*/ ValueType * getData(void) noexcept
	{
		return nullptr;
	}
	
	// O(1)
	constexpr const ValueType * getData(void) const noexcept
	{
		return nullptr;
	}
	
	// O(1)
	/*constexpr*/ ValueType & operator [](IndexType index) = delete;
	
	// O(1)
	constexpr const ValueType & operator [](IndexType index) const = delete;
	
	// O(1)
	constexpr void clear(void) noexcept
	{
	}
	
	// O(1)
	constexpr bool contains(const ValueType & item) const noexcept
	{
		return false;
	}
	
	// O(1)
	constexpr IndexOfType indexOf(const ValueType & item) const noexcept
	{
		return InvalidIndex;
	}
	
public:

	//
	// Specific Member Functions
	//

	// O(1)
	/*constexpr*/ ValueType & getFirst(void) = delete;

	// O(1)
	constexpr const ValueType & getFirst(void) const = delete;

	// O(1)
	/*constexpr*/ ValueType & getLast(void) = delete;
	
	// O(1)
	constexpr const ValueType & getLast(void) const = delete;

	// O(1)
	constexpr bool append(const ValueType & item)
	{
		return false;
	}

	// O(1)
	constexpr bool prepend(const ValueType & item)
	{
		return false;
	}

	// O(1)
	constexpr void unappend(void)
	{
	}

	// O(1)
	constexpr void unprepend(void)
	{
	}
	
	// O(N)
	constexpr bool removeFirst(const ValueType & item)
	{
		return false;
	}
	
	// O(N)
	constexpr bool removeLast(const ValueType & item)
	{
		return false;
	}
	
	// O(N)
	constexpr bool removeAt(IndexType index)
	{
		return false;
	}

	// O(N)
	constexpr bool insert(IndexType index, const ValueType & item)
	{
		return false;
	}
};