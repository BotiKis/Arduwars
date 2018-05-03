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

	//
	// Integral Constant
	//

	template< typename T, T value > struct integral_constant;

	//
	// Integral Constant Aliases
	//

	template< bool value > using bool_constant = integral_constant<bool, value>;

	using true_type = bool_constant<true>;

	using false_type = bool_constant<false>;

	//
	// Void T
	//

	template< typename ... > using void_t = void;

	//
	// Conditional
	//

	template< bool B, typename T = void > struct enable_if;

	template< bool B, typename T, typename U > struct conditional;

	template< typename ... B > struct conjunction;

	template< typename ... B > struct disjunction;

	template< typename B > struct negation;

	//
	// Conditional Aliases
	//

	// Since C++14
	template< bool B, typename T = void > using enable_if_t = typename enable_if<B, T>::type;

	// Since C++14
	template< bool B, typename T, typename U > using conditional_t = typename conditional<B, T, U>::type;

	// Since C++17
	//template< typename ... B > inline constexpr bool conjunction_v = conjunction<B ...>::value;

	// Since C++17
	//template< typename ... B > inline constexpr bool disjunction_v = disjunction<B ...>::value;

	// Since C++17
	//template< typename B > inline constexpr bool negation_v = negation<B>::value;

	//
	// Type Modifiers
	//

	template< typename T > struct remove_cv;

	template< typename T > struct remove_const;

	template< typename T > struct remove_volatile;

	template< typename T > struct add_cv;

	template< typename T > struct add_const;

	template< typename T > struct add_volatile;

	template< typename T > struct remove_pointer;

	template< typename T > struct add_pointer;

	template< typename T > struct remove_reference;

	template< typename T > struct add_lvalue_reference;

	template< typename T > struct add_rvalue_reference;

	template< typename T > struct remove_extent;

	template< typename T > struct remove_all_extents;

	// Since C++20
	template< typename T > struct remove_cvref;

	template< typename T > struct make_signed;

	template< typename T > struct make_unsigned;

	//
	// Type Modifier Aliases
	//

	// Since C++14
	template< typename T > using remove_cv_t = typename remove_cv<T>::type;

	// Since C++14
	template< typename T > using remove_const_t = typename remove_const<T>::type;

	// Since C++14
	template< typename T > using remove_volatile_t = typename remove_volatile<T>::type;

	// Since C++14
	template< typename T > using add_cv_t = typename add_cv<T>::type;

	// Since C++14
	template< typename T > using add_const_t = typename add_const<T>::type;

	// Since C++14
	template< typename T > using add_volatile_t = typename add_volatile<T>::type;

	// Since C++14
	template< typename T > using remove_pointer_t = typename remove_pointer<T>::type;

	// Since C++14
	template< typename T > using add_pointer_t = typename add_pointer<T>::type;

	// Since C++14
	template< typename T > using remove_reference_t = typename remove_reference<T>::type;

	// Since C++14
	template< typename T > using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

	// Since C++14
	template< typename T > using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

	// Since C++14
	template< typename T > using remove_extent_t = typename remove_extent<T>::type;

	// Since C++14
	template< typename T > using remove_all_extents_t = typename remove_all_extents<T>::type;

	// Since C++20
	template< typename T > using remove_cvref_t = typename remove_cvref<T>::type;

	// Since C++14
	template< typename T > using make_signed_t = typename make_signed<T>::type;

	// Since C++14
	template< typename T > using make_unsigned_t = typename make_unsigned<T>::type;

	//
	// Type Predicates
	//

	template< typename T, typename U > struct is_same;

	template< typename T > struct is_const;

	template< typename T > struct is_volatile;

	template< typename T > struct is_pointer;

	template< typename T > struct is_reference;

	template< typename T > struct is_lvalue_reference;

	template< typename T > struct is_rvalue_reference;

	template< typename T > struct is_array;

	template< typename T > struct is_void;

	template< typename T > struct is_null_pointer;

	//
	// Type Predicate Aliases
	//

	// Since C++17
	//template< typename T, typename U > inline constexpr bool is_same_v = is_same<T, U>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_const_v = is_const<T>::value;

	// Since C++17
	//template< typename T > inline volatileexpr bool is_volatile_v = is_volatile<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_pointer_v = is_pointer<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_reference_v = is_reference<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_array_v = is_array<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_void_v = is_void<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

	//
	// Type Properties
	//

	template< typename T > struct alignment_of;

	template< typename T > struct rank;

	template< typename T,unsigned N = 0 > struct extent;

	//
	// Special Purpose
	//

	template< typename T > add_rvalue_reference_t<T> declval(void) noexcept;

	template< typename T > struct decay;

	template< typename ... T > struct common_type;

	//
	// Special Purpose Aliases
	//

	// Since C++14
	template< typename T > using decay_t = typename decay<T>::type;

	// Since C++14
	template< typename ... T > using common_type_t = typename common_type<T ...>::type;

	//
	// Type Property Aliases
	//

	// Since C++17
	//template< typename T > inline constexpr decltype(sizeof(0)) alignment_of_v = alignment_of<T>::value;

	// Since C++17
	//template< typename T > inline constexpr decltype(sizeof(0)) rank_v = rank<T>::value;

	// Since C++17
	//template< typename T, unsigned N = 0 > inline constexpr decltype(sizeof(0)) extent_v = extent<T, N>::value;

	//
	// Type Categories
	//

	template< typename T > struct is_integral;

	template< typename T > struct is_floating_point;

	template< typename T > struct is_numeric;

	template< typename T > struct is_signed;

	template< typename T > struct is_unsigned;

	template< typename T > struct is_function;

	template< typename T > struct is_member_function_pointer;

	//
	// Type Category Aliases
	//

	// Since C++17
	//template< typename T > inline constexpr bool is_integral_v = is_integral<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_numeric_v = is_numeric<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_signed_v = is_signed<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_function_v = is_function<T>::value;

	// Since C++17
	//template< typename T > inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;



	//
	//
	// Implementation
	//
	//

	//
	// Constants
	//

	template< typename T, T v > struct integral_constant
	{
		using value_type = T;
		using type = integral_constant;
		
		static constexpr T value = v;
		
		constexpr operator value_type(void) const noexcept { return value; }
		
		// Since C++14
		constexpr T operator ()(void) const noexcept { return value; }
		
	};

	//
	// Conditional
	//

	template< bool B, typename T > struct enable_if {};
	template< typename T > struct enable_if<true, T> { using type = T; };


	template< typename T, typename U > struct conditional<true, T, U> { using type = T; };
	template< typename T, typename U > struct conditional<false, T, U> { using type = U; };


	template< typename ... > struct conjunction : true_type {};
	template< typename B > struct conjunction<B> : B {};
	template< typename B, typename ... BN > struct conjunction<B, BN ...> : conditional_t<bool(B::value), conjunction<BN ...>, B> {};


	template< typename ... B > struct disjunction : false_type {};
	template< typename B > struct disjunction<B> : B {};
	template< typename B, typename ... BN > struct disjunction<B, BN ...> : conditional_t<bool(B::value), B, disjunction<BN ...>> {};


	template< typename B > struct negation : bool_constant<!bool(B::value)> {};


	//
	// Type Modifiers
	//


	template< typename T > struct remove_cv { using type = T; };
	template< typename T > struct remove_cv<const T> { using type = T; };
	template< typename T > struct remove_cv<volatile T> { using type = T; };
	template< typename T > struct remove_cv<const volatile T> { using type = T; };


	template< typename T > struct remove_const { using type = T; };
	template< typename T > struct remove_const<const T> { using type = T; };


	template< typename T > struct remove_volatile { using type = T; };
	template< typename T > struct remove_volatile<volatile T> { using type = T; };


	template< typename T > struct add_cv { using type = const volatile T; };


	template< typename T > struct add_const { using type = const T; };


	template< typename T > struct add_volatile { using type = volatile T; };


	template< typename T > struct remove_pointer { using type = T; };
	template< typename T > struct remove_pointer<T*> { using type = T; };
	template< typename T > struct remove_pointer<T* const> { using type = T; };
	template< typename T > struct remove_pointer<T* volatile> { using type = T; };
	template< typename T > struct remove_pointer<T* const volatile> { using type = T; };


	template< typename T > struct add_pointer { using type = conditional_t<is_function<T>::value, T, remove_reference_t<T>*>; };
	template< typename T, typename ... Args > struct add_pointer<T(Args...)> { using type = T(*)(Args ...); };
	template< typename T, typename ... Args > struct add_pointer<T(Args..., ...)> { using type = T(*)(Args ..., ...); };


	template< typename T > struct remove_reference { using type = T; };
	template< typename T > struct remove_reference<T&> { using type = T; };
	template< typename T > struct remove_reference<T&&> { using type = T; };


	template< typename T > struct add_lvalue_reference { using type = T&; };
	template< typename T > struct add_lvalue_reference<T&> { using type = T&; };
	template< typename T > struct add_lvalue_reference<T&&> { using type = T&; };


	template< typename T > struct add_rvalue_reference { using type = T&&; };
	template< typename T > struct add_rvalue_reference<T&> { using type = T&; };
	template< typename T > struct add_rvalue_reference<T&&> { using type = T&&; };


	template< typename T > struct remove_extent { using type = T; };
	template< typename T > struct remove_extent<T[]> { using type = T; };
	template< typename T, decltype(sizeof(0)) N > struct remove_extent<T[N]> { using type = T; };


	template< typename T > struct remove_all_extents { using type = T; };
	template< typename T > struct remove_all_extents<T[]> { using type = typename remove_all_extents<T>::type; };
	template< typename T, decltype(sizeof(0)) N > struct remove_all_extents<T[N]> { using type = typename remove_all_extents<T>::type; };


	template< typename T > struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };


	template< typename T > struct make_signed {};
	template< typename T > struct make_signed<const T> { using type = const make_signed<T>; };
	template< typename T > struct make_signed<volatile T> { using type = volatile make_signed<T>; };
	template< typename T > struct make_signed<const volatile T> { using type = const volatile make_signed<T>; };
	template<> struct make_signed<char> { using type = signed char; };
	template<> struct make_signed<signed char> { using type = signed char; };
	template<> struct make_signed<unsigned char> { using type = signed char; };
	template<> struct make_signed<signed short> { using type = signed short; };
	template<> struct make_signed<unsigned short> { using type = signed short; };
	template<> struct make_signed<signed int> { using type = signed int; };
	template<> struct make_signed<unsigned int> { using type = signed int; };
	template<> struct make_signed<signed long int> { using type = signed long int; };
	template<> struct make_signed<unsigned long int> { using type = signed long int; };
	template<> struct make_signed<signed long long int> { using type = signed long long int; };
	template<> struct make_signed<unsigned long long int> { using type = signed long long int; };


	template< typename T > struct make_unsigned {};
	template< typename T > struct make_unsigned<const T> { using type = const make_unsigned<T>; };
	template< typename T > struct make_unsigned<volatile T> { using type = volatile make_unsigned<T>; };
	template< typename T > struct make_unsigned<const volatile T> { using type = const volatile make_unsigned<T>; };
	template<> struct make_unsigned<char> { using type = unsigned char; };
	template<> struct make_unsigned<signed char> { using type = unsigned char; };
	template<> struct make_unsigned<unsigned char> { using type = unsigned char; };
	template<> struct make_unsigned<signed short> { using type = unsigned short; };
	template<> struct make_unsigned<unsigned short> { using type = unsigned short; };
	template<> struct make_unsigned<signed int> { using type = unsigned int; };
	template<> struct make_unsigned<unsigned int> { using type = unsigned int; };
	template<> struct make_unsigned<signed long int> { using type = unsigned long int; };
	template<> struct make_unsigned<unsigned long int> { using type = unsigned long int; };
	template<> struct make_unsigned<signed long long int> { using type = unsigned long long int; };
	template<> struct make_unsigned<unsigned long long int> { using type = unsigned long long int; };


	//
	// Type Predicates
	//


	template< typename T, typename U > struct is_same : false_type {};
	template< typename T > struct is_same<T, T> : true_type {};


	template< typename T > struct is_const : false_type {};
	template< typename T > struct is_const<const T> : true_type {};


	template< typename T > struct is_volatile : false_type {};
	template< typename T > struct is_volatile<volatile T> : true_type {};


	template< typename T > struct is_pointer : false_type {};
	template< typename T > struct is_pointer<T*> : true_type {};
	template< typename T > struct is_pointer<const T*> : true_type {};
	template< typename T > struct is_pointer<volatile T*> : true_type {};
	template< typename T > struct is_pointer<const volatile T*> : true_type {};


	template< typename T > struct is_reference : false_type {};
	template< typename T > struct is_reference<T&> : true_type {};
	template< typename T > struct is_reference<T&&> : true_type {};


	template< typename T > struct is_lvalue_reference : false_type {};
	template< typename T > struct is_lvalue_reference<T&> : true_type {};


	template< typename T > struct is_rvalue_reference : false_type {};
	template< typename T > struct is_rvalue_reference<T&&> : true_type {};


	template< typename T > struct is_array : false_type {};
	template< typename T > struct is_array<T[]> : true_type {};
	template< typename T, decltype(sizeof(0)) N > struct is_array<T[N]> : true_type {};


	template< typename T > struct is_void : is_same<void, remove_cv_t<T>> {};


	template< typename T > struct is_null_pointer : is_same<T, remove_cv_t<decltype(nullptr)>> {};


	//
	// Type Properties
	//


	template< typename T > struct alignment_of : integral_constant<decltype(sizeof(0)), alignof(T)> {};


	template< typename T > struct rank : integral_constant<decltype(sizeof(0)), 0> {};
	template< typename T > struct rank<T[]> : integral_constant<decltype(sizeof(0)), rank<T>::value + 1> {};
	template< typename T, decltype(sizeof(0)) N > struct rank<T[N]> : integral_constant<decltype(sizeof(0)), rank<T>::value + 1> {};


	template< typename T, unsigned N > struct extent : integral_constant<decltype(sizeof(0)), 0> {};
	template< typename T > struct extent<T[], 0> : integral_constant<decltype(sizeof(0)), 0> {};
	template< typename T, unsigned N > struct extent<T[], N> : extent<T, N-1> {};
	template< typename T, decltype(sizeof(0)) I > struct extent<T[I], 0>: integral_constant<decltype(sizeof(0)), 0> {};
	template< typename T, decltype(sizeof(0)) I , unsigned N > struct extent<T[I], N> : extent<T, N-1> {};

	//
	// Special Purpose
	//

	template< typename T > struct decay
	{
	private:
		using U = remove_reference_t<T>;
		using V = conditional_t < is_function<U>::value, add_pointer_t<U>, remove_cv_t<U> >;
		using Result = conditional_t< is_array<U>::value, remove_extent_t<U>*, V >;

	public:
		using type = Result;	
	};


	template< typename ... T > struct common_type {};
	template< typename T > struct common_type<T> { using type = decay_t<T>; };
	template< typename T, typename U > struct common_type<T, U> : conditional_t<(is_same<T, decay_t<T>>::value && is_same<U, decay_t<U>>::value), enable_if<(is_void<void_t<decltype(false ? declval<T>() : declval<U>())>>::value), decay_t<decltype(false ? declval<T>() : declval<U>())> >, common_type<decay_t<T>, decay_t<U>>> {};
	template< typename T, typename U, typename ... Ts > struct common_type<T, U, Ts...> : conditional_t<(is_void<void_t<common_type_t<T, U>>>::value), common_type<common_type_t<T, U>, Ts ...>, common_type<>> {};


	//
	// Type Categories
	//

	template< typename T > struct is_integral : false_type {};
	template< typename T > struct is_integral<const T> : is_integral<T> {};
	template< typename T > struct is_integral<volatile T> : is_integral<T> {};
	template< typename T > struct is_integral<const volatile T> : is_integral<T> {};
	template<> struct is_integral<bool> : true_type {};
	template<> struct is_integral<char> : true_type {};
	template<> struct is_integral<signed char> : true_type {};
	template<> struct is_integral<unsigned char> : true_type {};
	template<> struct is_integral<char16_t> : true_type {};
	template<> struct is_integral<char32_t> : true_type {};
	template<> struct is_integral<wchar_t> : true_type {};
	template<> struct is_integral<signed short> : true_type {};
	template<> struct is_integral<unsigned short> : true_type {};
	template<> struct is_integral<signed int> : true_type {};
	template<> struct is_integral<unsigned int> : true_type {};
	template<> struct is_integral<signed long int> : true_type {};
	template<> struct is_integral<unsigned long int> : true_type {};
	template<> struct is_integral<signed long long int> : true_type {};
	template<> struct is_integral<unsigned long long int> : true_type {};


	template< typename T > struct is_floating_point : false_type {};
	template< typename T > struct is_floating_point<const T> : is_floating_point<T> {};
	template< typename T > struct is_floating_point<volatile T> : is_floating_point<T> {};
	template< typename T > struct is_floating_point<const volatile T> : is_floating_point<T> {};
	template<> struct is_floating_point<float> : true_type {};
	template<> struct is_floating_point<double> : true_type {};
	template<> struct is_floating_point<long double> : true_type {};


	template< typename T > struct is_arithmetic : bool_constant<is_integral<T>::value || is_floating_point<T>::value> {};


	template< typename T > struct is_signed : conditional_t<is_arithmetic<T>::value, bool_constant<(T(-1) < T(0))>, false_type> {};


	template< typename T > struct is_unsigned : conditional_t<is_arithmetic<T>::value, bool_constant<(T(-1) > T(0))>, false_type> {};


	template< typename T > struct is_function : false_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...)> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...)> : true_type {}; 
	template< typename R, typename ... Args > struct is_function<R(Args ...) const> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile> : true_type {}; 
	template< typename R, typename ... Args > struct is_function<R(Args ...) &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile &> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile &&> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile &&> : true_type {};
	// Since C++17
	/*template< typename R, typename ... Args > struct is_function<R(Args ...) noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile & noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) volatile && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ...) const volatile && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) volatile && noexcept> : true_type {};
	template< typename R, typename ... Args > struct is_function<R(Args ... ...) const volatile && noexcept> : true_type {};*/


	template< typename T > struct is_member_function_pointer : false_type {};
	template< typename T > struct is_member_function_pointer<const T> : is_member_function_pointer<T> {};
	template< typename T > struct is_member_function_pointer<volatile T> : is_member_function_pointer<T> {};
	template< typename T > struct is_member_function_pointer<const volatile T> : is_member_function_pointer<T> {};
	template< typename T, typename U> struct is_member_function_pointer<T U::*> : is_function<T> {};

}