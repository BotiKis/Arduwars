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

#include "TypeTraits.h"

#if defined(USE_NAMESPACE_STD)
namespace stdlib
{
}

namespace std
{
	using namespace stdlib;
}
#endif

namespace stdlib
{

	//
	//
	// Declarations
	//
	//

	template< typename T > void swap( T& a, T& b );
	//template< typename T > void swap( T& a, T& b ) noexcept(is_nothrow_move_constructible<T>::value && is_nothrow_move_assignable<T>::value);


	template< typename T, decltype(sizeof(0)) N > void swap( T (&a)[N], T (&b)[N] ) noexcept(noexcept(swap(*a, *b)));
	//template< typename T, decltype(sizeof(0)) N > void swap( T& a, T& b ) noexcept(std::is_nothrow_swappable<T>::value);


	template< typename T > /*constexpr*/ remove_reference_t<T> && move( T && t ) noexcept;


	template< typename T > /*constexpr*/ T&& forward( remove_reference_t<T> & t ) noexcept;
	template< typename T > /*constexpr*/ T&& forward( remove_reference_t<T> && t ) noexcept;


	// Since C++17
	template <typename T> constexpr add_const_t<T> & as_const( T & t ) noexcept;


	// Since C++17
	template <typename T> void as_const( const T && ) = delete;


	template<typename T, typename U = T> T exchange(T & obj, U && new_value );

	//
	//
	// Definitions
	//
	//

	template< typename T > /*constexpr*/ remove_reference_t<T> && move(T && t) noexcept
	{
		return static_cast<remove_reference_t<T> &&>(t);
	}

	template< typename T > void swap(T& a, T& b)
	{
		T c = move(a);
		a = move(b);
		b = move(c);
	}

	template< typename T, decltype(sizeof(0)) N > void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(swap(*a, *b)))
	{
		for(decltype(sizeof(0)) i = 0; i < N; ++i)
			swap(a[i], b[i]);
	}

	template< typename T > /*constexpr*/ T&& forward(remove_reference_t<T> & t) noexcept
	{
		return static_cast<T &&>(t);
	}


	template< typename T > /*constexpr*/ T&& forward(remove_reference_t<T> && t) noexcept
	{
		static_assert(!is_lvalue_reference<T>::value, "template argument substituting T is an lvalue reference type");
		return static_cast<T &&>(t);
	}


	template <typename T> constexpr add_const_t<T> & as_const(T & t) noexcept
	{
		return t;
	}


	template<typename T, typename U > T exchange(T & obj, U && new_value)
	{
		T old_value = move(obj);
		obj = forward<U>(new_value);
		return old_value;
	}

}