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

#include "Deque.h"

//
// Declarations
//

template< typename Type, uint8_t Capacity, typename Container = Deque<Type, Capacity> >
class List;

template< typename Type, uint8_t CapacityValue, typename Container >
class List
{
public:

	//
	// Constraints
	//

	//static_assert(CapacityValue > 0, "Attempt to create List with a capacity less than 1");
	static_assert(CapacityValue < 128, "Attempt to create List with a capacity greater than 127");

	//
	// Type Aliases
	//
	
	using ContainerType = Container;
	using ValueType = typename ContainerType::ValueType;
	using SizeType = typename ContainerType::SizeType;
	using IndexType = typename ContainerType::IndexType;
	using IndexOfType = typename ContainerType::IndexOfType;
	
	//
	// Constants
	//
	
	constexpr static const SizeType Capacity = ContainerType::Capacity;
	constexpr static const IndexOfType InvalidIndex = ContainerType::InvalidIndex;	
	constexpr static const IndexType FirstIndex = ContainerType::FirstIndex;
	constexpr static const IndexType LastIndex = ContainerType::LastIndex;

private:
	
	//
	// Member Variables
	//
	
	ContainerType container;
	
public:

	//
	// Common Member Functions
	//
	
	// O(1)
	bool isEmpty(void) const noexcept
	{
		return this->container.isEmpty();
	}
	
	// O(1)
	bool isFull(void) const noexcept
	{
		return this->container.isFull();
	}
	
	// O(1)
	SizeType getCount(void) const noexcept
	{		
		return this->container.getCount();
	}
	
	// O(1)
	constexpr SizeType getCapacity(void) const noexcept
	{
		return this->container.getCapacity();
	}
	
	// O(1)
	ValueType * getData(void) noexcept
	{
		return this->container.getData();
	}
	
	// O(1)
	const ValueType * getData(void) const noexcept
	{
		return this->container.getData();
	}
	
	// O(1)
	ValueType & operator [](const IndexType & index)
	{
		return this->container[index];
	}
	
	// O(1)
	const ValueType & operator [](const IndexType & index) const
	{
		return this->container[index];
	}
	
	// O(N)
	void clear(void)
	{
		this->container.clear();
	}
	
	// O(N)
	void fill(const ValueType & item)
	{
		this->container.fill(item);
	}
	
	// O(N)
	bool contains(const ValueType & item) const
	{
		return this->container.contains(item);
	}
	
	// O(N)
	IndexOfType indexOfFirst(const ValueType & item) const
	{
		return this->container.indexOfFirst(item);
	}
	
	// O(N)
	IndexOfType indexOfLast(const ValueType & item) const
	{
		return this->container.indexOfLast(item);
	}
	
public:

	//
	// Specific Member Functions
	//

	// O(1)
	bool add(const ValueType & item)
	{
		return this->container.append(item);
	}
	
	// O(N)
	bool removeFirst(const ValueType & item)
	{
		return this->container.removeFirst(item);
	}
	
	// O(N)
	bool removeLast(const ValueType & item)
	{
		return this->container.removeLast(item);
	}
	
	// O(N)
	bool removeAt(const IndexType & index)
	{
		return this->container.removeAt(index);
	}

	// O(N)
	bool insert(const IndexType & index, const ValueType & item)
	{
		return this->container.insert(index, item);
	}
};