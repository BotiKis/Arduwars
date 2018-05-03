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

template< typename Type, uint8_t Width, uint8_t Height >
class Grid;

template< typename Type, uint8_t WidthValue, uint8_t HeightValue >
class Grid
{
private:

	//
	// Private Helper Types (Ignore These)
	//

	template< bool Condition, typename T, typename U > struct ContitionalHelper;
	template< typename T, typename U > struct ContitionalHelper<true, T, U> { using RType = T; };
	template< typename T, typename U > struct ContitionalHelper<false, T, U> { using RType = U; };
	template< bool Condition, typename T, typename U > using ConditionalType = typename ContitionalHelper<Condition, T, U>::RType;
	constexpr static const uint16_t SecretCapacity = static_cast<uint16_t>(WidthValue) * static_cast<uint16_t>(HeightValue);
	
public:

	//
	// Type Aliases
	//
	
	using ValueType = Type;
	using SizeType = ConditionalType<(SecretCapacity > 256), uint16_t, uint8_t>;
	using IndexType = SizeType;
	using DimensionType = uint8_t;
	
	//
	// Constants
	//
	
	constexpr static const DimensionType Width = WidthValue;
	constexpr static const DimensionType Height = HeightValue;
	constexpr static const SizeType Capacity = static_cast<SizeType>(Width) * static_cast<SizeType>(Height);

private:
	
	//
	// Member Variables
	//
	
	ValueType items[Capacity];

	//
	// Private Member Functions
	//

	inline IndexType flattenIndex(const DimensionType & x, const DimensionType & y) const
	{
		return (Width * static_cast<IndexType>(y)) + static_cast<IndexType>(x);
	}

public:

	//
	// Public Member Functions
	//
	
	// O(1)
	constexpr DimensionType getWidth(void) const
	{
		return Width;
	}
	
	// O(1)
	constexpr DimensionType getHeight(void) const
	{
		return Height;
	}
	
	// O(1)
	constexpr SizeType getCapacity(void) const noexcept
	{
		return Capacity;
	}

	// O(1)
	ValueType & getItem(const uint8_t & x, const uint8_t & y)
	{
		return this->items[this->flattenIndex(x, y)];
	}
	
	// O(1)
	const ValueType & getItem(const uint8_t & x, const uint8_t & y) const
	{
		return this->items[this->flattenIndex(x, y)];
	}

	// O(N)
	void fill(const ValueType & value)
	{
		for(IndexType i = 0; i < Capacity; ++i)
			items[i] = value;	
	}
	
	// O(N)
	void clear(void)
	{
		for(IndexType i = 0; i < Capacity; ++i)
			items[i].~ValueType();
	}
};