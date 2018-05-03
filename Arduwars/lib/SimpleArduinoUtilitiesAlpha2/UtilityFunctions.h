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

//
// absT
//

template< typename T >
constexpr T absT(T value)
{
	return (value < 0) ? -value : value;
}

//
// minT
//

template< typename T >
constexpr T minT(T left, T right)
{
	return (left < right) ? left : right;
}

template< typename T >
constexpr T & minT(T & left, T & right)
{
	return (left < right) ? left : right;
}

template< typename T >
constexpr T const & minT(T const & left, T const & right)
{
	return (left < right) ? left : right;
}

//
// maxT
//

template< typename T >
constexpr T maxT(T left, T right)
{
	return (left > right) ? left : right;
}

template< typename T >
constexpr T & maxT(T & left, T & right)
{
	return (left > right) ? left : right;
}

template< typename T >
constexpr T const & maxT(T const & left, T const & right)
{
	return (left > right) ? left : right;
}

//
// clampT
//

template< typename T >
constexpr T clampT(T value, T left, T right)
{
	return (value < left) ? left : (value > right) ? right : value;
}

template< typename T >
constexpr T & clampT(T & value, T & left, T & right)
{
	return (value < left) ? left : (value > right) ? right : value;
}

template< typename T >
constexpr T const & clampT(T const & value, T const & left, T const & right)
{
	return (value < left) ? left : (value > right) ? right : value;
}