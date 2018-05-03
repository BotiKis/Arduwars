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

//
// Declarations
//

template< typename Type, uint8_t Capacity >
class Array;

template< typename Type, uint8_t CapacityValue >
class Array
{
public:

	//
	// Constraints
	//

	static_assert(CapacityValue > 0, "Attempt to create Array with a capacity less than 1");
	static_assert(CapacityValue < 128, "Attempt to create Array with a capacity greater than 127");

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
	constexpr static const IndexType FirstIndex = 0;
	constexpr static const IndexType LastIndex = Capacity - 1;
	constexpr static const IndexOfType InvalidIndex = -1;

private:
	
	//
	// Member Variables
	//
	
	ValueType items[Capacity] = {};
	
public:

	//
	// Common Member Functions
	//
	
	// O(1)
	constexpr bool isEmpty(void) const noexcept
	{
		return false;
	}
	
	// O(1)
	constexpr bool isFull(void) const noexcept
	{
		return true;
	}
	
	// O(1)
	constexpr SizeType getCount(void) const noexcept
	{
		return Capacity;
	}
	
	// O(1)
	constexpr SizeType getCapacity(void) const noexcept
	{
		return Capacity;
	}
	
	// O(1)
	/*constexpr*/ ValueType * getData(void) noexcept
	{
		return &this->items[FirstIndex];
	}
	
	// O(1)
	constexpr const ValueType * getData(void) const noexcept
	{
		return &this->items[FirstIndex];
	}
	
	// O(1)
	ValueType & operator [](const IndexType & index)
	{
		return this->items[index];
	}
	
	// O(1)
	const ValueType & operator [](const IndexType & index) const
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
};

//
// Definition
//

// O(N)
template< typename Type, uint8_t Capacity >
void Array<Type, Capacity>::clear(void)
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		this->items[i].~ValueType();
}

// O(N)
template< typename Type, uint8_t Capacity >
void Array<Type, Capacity>::fill(const ValueType & item)
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		this->items[i] = item;
}

// O(N)
template< typename Type, uint8_t Capacity >
bool Array<Type, Capacity>::contains(const ValueType & item) const
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		if (this->items[i] == item)
			return true;
	return false;
}

// O(N)
template< typename Type, uint8_t Capacity >
auto Array<Type, Capacity>::indexOfFirst(const ValueType & item) const -> IndexOfType
{
	for (IndexType i = 0; i < this->getCount(); ++i)
		if (this->items[i] == item)
			return i;
	return InvalidIndex;
}

// O(N)
template< typename Type, uint8_t Capacity >
auto Array<Type, Capacity>::indexOfLast(const ValueType & item) const -> IndexOfType
{
	for (IndexType i = this->getCount() - 1; i > 0; --i)
		if (this->items[i] == item)
			return i;
	return InvalidIndex;
}

//
// Empty Array
//

template< typename Type >
class Array<Type, 0>
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
	
public:

	//
	// Common Member Functions
	//
	
	// O(1)
	constexpr bool isEmpty(void) const noexcept
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
	/*constexpr*/ ValueType & operator [](const IndexType & index) = delete;
	
	// O(1)
	constexpr const ValueType & operator [](const IndexType & index) const = delete;
	
	// O(1)
	constexpr void clear(void) noexcept
	{
	}
	
	// O(1)
	constexpr void fill(void) noexcept
	{
	}
	
	// O(1)
	constexpr bool contains(const ValueType & item) const noexcept
	{
		return false;
	}
	
	// O(1)
	constexpr IndexOfType indexOfFirst(const ValueType & item) const noexcept
	{
		return InvalidIndex;
	}
	
	// O(1)
	constexpr IndexOfType indexOfLast(const ValueType & item) const noexcept
	{
		return InvalidIndex;
	}
};