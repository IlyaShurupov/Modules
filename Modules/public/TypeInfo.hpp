
#pragma once

////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The isAbove copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////


namespace tp {
	template <bool flag, typename T, typename U>
	struct TypeSelect {
		typedef T Result;
	};
	template <typename T, typename U>
	struct TypeSelect<false, T, U> {
		typedef U Result;
	};
}

namespace tp {
////////////////////////////////////////////////////////////////////////////////
// class template IsSameType
// Return true iff two given types are the same
// Invocation: SameType<T, U>::value
// where:
// T and U are types
// Result evaluates to true iff U == T (types equal)
////////////////////////////////////////////////////////////////////////////////

	template <typename T, typename U>
	struct IsSameType {
		enum { value = false };
	};

	template <typename T>
	struct IsSameType<T, T> {
		enum { value = true };
	};

////////////////////////////////////////////////////////////////////////////////
// Helper types Small and Big - guarantee that sizeof(Small) < sizeof(Big)
////////////////////////////////////////////////////////////////////////////////

	namespace Private {
		template <class T, class U>
		struct ConversionHelper {
			typedef char Small;
			struct Big { char dummy[2]; };
			static Big   Test(...);
			static Small Test(U);
			static T MakeT();
		};
	}

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

	template <class T, class U>
	struct Conversion {
		typedef Private::ConversionHelper<T, U> H;
#ifndef __MWERKS__
		enum { exists = sizeof(typename H::Small) == sizeof((H::Test(H::MakeT()))) };
#else
		enum { exists = false };
#endif
		enum { exists2Way = exists && Conversion<U, T>::exists };
		enum { sameType = false };
	};

	template <class T>
	struct Conversion<T, T> {
		enum { exists = 1, exists2Way = 1, sameType = 1 };
	};

	template <class T>
	struct Conversion<void, T> {
		enum { exists = 0, exists2Way = 0, sameType = 0 };
	};

	template <class T>
	struct Conversion<T, void> {
		enum { exists = 0, exists2Way = 0, sameType = 0 };
	};

	template <>
	struct Conversion<void, void> {
	public:
		enum { exists = 1, exists2Way = 1, sameType = 1 };
	};

////////////////////////////////////////////////////////////////////////////////
// class template SuperSubclass
// Invocation: SuperSubclass<B, D>::value where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

	template <class T, class U>
	struct SuperSubclass {
		enum {
			value = (::tp::Conversion<const volatile U*, const volatile T*>::exists && !::tp::Conversion<const volatile T*, const volatile void*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (sizeof(T) == sizeof(U)) };
	};

	template <>
	struct SuperSubclass<void, void> {
		enum { value = false };
	};

	template <class U>
	struct SuperSubclass<void, U> {
		enum {
			value = (::tp::Conversion<const volatile U*, const volatile void*>::exists && !::tp::Conversion<const volatile void*, const volatile void*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (0 == sizeof(U)) };
	};

	template <class T>
	struct SuperSubclass<T, void> {
		enum {
			value = (::tp::Conversion<const volatile void*, const volatile T*>::exists && !::tp::Conversion<const volatile T*, const volatile void*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (sizeof(T) == 0) };
	};

	////////////////////////////////////////////////////////////////////////////////
	// class template SuperSubclassStrict
	// Invocation: SuperSubclassStrict<B, D>::value where B and D are types.
	// Returns true if B is a public base of D.
	//
	// Caveat: might not work if T and U are in a private inheritance hierarchy.
	////////////////////////////////////////////////////////////////////////////////

	template<class T, class U>
	struct SuperSubclassStrict {
		enum {
			value = (::tp::Conversion<const volatile U*, const volatile T*>::exists &&
				!::tp::Conversion<const volatile T*, const volatile void*>::sameType &&
				!::tp::Conversion<const volatile T*, const volatile U*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (sizeof(T) == sizeof(U)) };
	};

	template<>
	struct SuperSubclassStrict<void, void> {
		enum { value = false };
	};

	template<class U>
	struct SuperSubclassStrict<void, U> {
		enum {
			value = (::tp::Conversion<const volatile U*, const volatile void*>::exists &&
				!::tp::Conversion<const volatile void*, const volatile void*>::sameType &&
				!::tp::Conversion<const volatile void*, const volatile U*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (0 == sizeof(U)) };
	};

	template<class T>
	struct SuperSubclassStrict<T, void> {
		enum {
			value = (::tp::Conversion<const volatile void*, const volatile T*>::exists &&
				!::tp::Conversion<const volatile T*, const volatile void*>::sameType &&
				!::tp::Conversion<const volatile T*, const volatile void*>::sameType)
		};

// Dummy enum to make sure that both classes are fully defined.
		enum { dontUseWithIncompleteTypes = (sizeof(T) == 0) };
	};


}   // namespace tp

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D, or if B and D are aliases of the
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
// Deprecated: Use SuperSubclass class template instead.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS(T, U) ::tp::SuperSubclass<T,U>::value

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS_STRICT
// Invocation: SUPERSUBCLASS(B, D) where B and D are types.
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
// Deprecated: Use SuperSubclassStrict class template instead.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS_STRICT(T, U) ::tp::SuperSubclassStrict<T,U>::value



namespace tp {

	struct NullType {};

////////////////////////////////////////////////////////////////////////////////
// class template Typelist
// The building block of typelists of any length
// Use it through the LOKI_TYPELIST_NN macros
// Defines nested types:
//     Head (first element, a non-typelist type by convention)
//     Tail (second element, can be another typelist)
////////////////////////////////////////////////////////////////////////////////

	template <class T, class U>
	struct Typelist {
		typedef T Head;
		typedef U Tail;
	};

// Typelist utility algorithms

	namespace TL {

////////////////////////////////////////////////////////////////////////////////
// class template MakeTypelist
// Takes a number of arguments equal to its numeric suffix
// The arguments are type names.
// MakeTypelist<T1, T2, ...>::Result
// returns a typelist that is of T1, T2, ...
////////////////////////////////////////////////////////////////////////////////

		template
			<
				typename T1 = NullType, typename T2 = NullType, typename T3 = NullType,
				typename T4 = NullType, typename T5 = NullType, typename T6 = NullType,
				typename T7 = NullType, typename T8 = NullType, typename T9 = NullType,
				typename T10 = NullType, typename T11 = NullType, typename T12 = NullType,
				typename T13 = NullType, typename T14 = NullType, typename T15 = NullType,
				typename T16 = NullType, typename T17 = NullType, typename T18 = NullType
			>
		struct MakeTypelist {
		private:
			typedef typename MakeTypelist
				<
					T2, T3, T4,
					T5, T6, T7,
					T8, T9, T10,
					T11, T12, T13,
					T14, T15, T16,
					T17, T18
				>
			::Result TailResult;

		public:
			typedef Typelist<T1, TailResult> Result;
		};

		template<>
		struct MakeTypelist<> {
			typedef NullType Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template Length
// Computes the length of a typelist
// Invocation (TList is a typelist):
// Length<TList>::value
// returns a compile-time constant containing the length of TList, not counting
//     the end terminator (which by convention is NullType)
////////////////////////////////////////////////////////////////////////////////

		template <class TList> struct Length;
		template <> struct Length<NullType> {
			enum { value = 0 };
		};

		template <class T, class U>
		struct Length< Typelist<T, U> > {
			enum { value = 1 + Length<U>::value };
		};

////////////////////////////////////////////////////////////////////////////////
// class template TypeAt
// Finds the type at a given index in a typelist
// Invocation (TList is a typelist and index is a compile-time integral
//     constant):
// TypeAt<TList, index>::Result
// returns the type in position 'index' in TList
// If you pass an out-of-bounds index, the result is a compile-time error
////////////////////////////////////////////////////////////////////////////////

		template <class TList, unsigned int index> struct TypeAt;

		template <class Head, class Tail>
		struct TypeAt<Typelist<Head, Tail>, 0> {
			typedef Head Result;
		};

		template <class Head, class Tail, unsigned int i>
		struct TypeAt<Typelist<Head, Tail>, i> {
			typedef typename TypeAt<Tail, i - 1>::Result Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template TypeAtNonStrict
// Finds the type at a given index in a typelist
// Invocations (TList is a typelist and index is a compile-time integral
//     constant):
// a) TypeAt<TList, index>::Result
// returns the type in position 'index' in TList, or NullType if index is
//     out-of-bounds
// b) TypeAt<TList, index, D>::Result
// returns the type in position 'index' in TList, or D if index is out-of-bounds
////////////////////////////////////////////////////////////////////////////////

		template <class TList, unsigned int index,
			typename DefaultType = NullType>
		struct TypeAtNonStrict {
			typedef DefaultType Result;
		};

		template <class Head, class Tail, typename DefaultType>
		struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType> {
			typedef Head Result;
		};

		template <class Head, class Tail, unsigned int i, typename DefaultType>
		struct TypeAtNonStrict<Typelist<Head, Tail>, i, DefaultType> {
			typedef typename
			TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template IndexOf
// Finds the index of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// IndexOf<TList, T>::value
// returns the position of T in TList, or NullType if T is not found in TList
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T> struct IndexOf;

		template <class T>
		struct IndexOf<NullType, T> {
			enum { value = -1 };
		};

		template <class T, class Tail>
		struct IndexOf<Typelist<T, Tail>, T> {
			enum { value = 0 };
		};

		template <class Head, class Tail, class T>
		struct IndexOf<Typelist<Head, Tail>, T> {
		private:
			enum { temp = IndexOf<Tail, T>::value };
		public:
			enum { value = (temp == -1 ? -1 : 1 + temp) };
		};

////////////////////////////////////////////////////////////////////////////////
// class template Append
// Appends a type or a typelist to another
// Invocation (TList is a typelist and T is either a type or a typelist):
// Append<TList, T>::Result
// returns a typelist that is TList followed by T and NullType-terminated
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T> struct Append;

		template <> struct Append<NullType, NullType> {
			typedef NullType Result;
		};

		template <class T> struct Append<NullType, T> {
			typedef Typelist<T, NullType> Result;
		};

		template <class Head, class Tail>
		struct Append<NullType, Typelist<Head, Tail> > {
			typedef Typelist<Head, Tail> Result;
		};

		template <class Head, class Tail, class T>
		struct Append<Typelist<Head, Tail>, T> {
			typedef Typelist<Head, typename Append<Tail, T>::Result> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template Erase
// Erases the first occurence, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// Erase<TList, T>::Result
// returns a typelist that is TList without the first occurence of T
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T> struct Erase;

		template <class T>                         // Specialization 1
		struct Erase<NullType, T> {
			typedef NullType Result;
		};

		template <class T, class Tail>             // Specialization 2
		struct Erase<Typelist<T, Tail>, T> {
			typedef Tail Result;
		};

		template <class Head, class Tail, class T> // Specialization 3
		struct Erase<Typelist<Head, Tail>, T> {
			typedef Typelist<Head, typename Erase<Tail, T>::Result> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template EraseAll
// Erases all first occurences, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// EraseAll<TList, T>::Result
// returns a typelist that is TList without any occurence of T
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T> struct EraseAll;
		template <class T>
		struct EraseAll<NullType, T> {
			typedef NullType Result;
		};
		template <class T, class Tail>
		struct EraseAll<Typelist<T, Tail>, T> {
			// Go all the way down the list removing the type
			typedef typename EraseAll<Tail, T>::Result Result;
		};
		template <class Head, class Tail, class T>
		struct EraseAll<Typelist<Head, Tail>, T> {
			// Go all the way down the list removing the type
			typedef Typelist<Head, typename EraseAll<Tail, T>::Result> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template NoDuplicates
// Removes all duplicate types in a typelist
// Invocation (TList is a typelist):
// NoDuplicates<TList, T>::Result
////////////////////////////////////////////////////////////////////////////////

		template <class TList> struct NoDuplicates;

		template <> struct NoDuplicates<NullType> {
			typedef NullType Result;
		};

		template <class Head, class Tail>
		struct NoDuplicates< Typelist<Head, Tail> > {
		private:
			typedef typename NoDuplicates<Tail>::Result L1;
			typedef typename Erase<L1, Head>::Result L2;
		public:
			typedef Typelist<Head, L2> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template Replace
// Replaces the first occurence of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which the first occurence of T is replaced with U
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T, class U> struct Replace;

		template <class T, class U>
		struct Replace<NullType, T, U> {
			typedef NullType Result;
		};

		template <class T, class Tail, class U>
		struct Replace<Typelist<T, Tail>, T, U> {
			typedef Typelist<U, Tail> Result;
		};

		template <class Head, class Tail, class T, class U>
		struct Replace<Typelist<Head, Tail>, T, U> {
			typedef Typelist<Head, typename Replace<Tail, T, U>::Result> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template ReplaceAll
// Replaces all occurences of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which all occurences of T is replaced with U
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T, class U> struct ReplaceAll;

		template <class T, class U>
		struct ReplaceAll<NullType, T, U> {
			typedef NullType Result;
		};

		template <class T, class Tail, class U>
		struct ReplaceAll<Typelist<T, Tail>, T, U> {
			typedef Typelist<U, typename ReplaceAll<Tail, T, U>::Result> Result;
		};

		template <class Head, class Tail, class T, class U>
		struct ReplaceAll<Typelist<Head, Tail>, T, U> {
			typedef Typelist<Head, typename ReplaceAll<Tail, T, U>::Result> Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template Reverse
// Reverses a typelist
// Invocation (TList is a typelist):
// Reverse<TList>::Result
// returns a typelist that is TList reversed
////////////////////////////////////////////////////////////////////////////////

		template <class TList> struct Reverse;

		template <>
		struct Reverse<NullType> {
			typedef NullType Result;
		};

		template <class Head, class Tail>
		struct Reverse< Typelist<Head, Tail> > {
				typedef typename Append<typename Reverse<Tail>::Result, Head>::Result Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template MostDerived
// Finds the type in a typelist that is the most derived from a given type
// Invocation (TList is a typelist, T is a type):
// MostDerived<TList, T>::Result
// returns the type in TList that's the most derived from T
////////////////////////////////////////////////////////////////////////////////

		template <class TList, class T> struct MostDerived;

		template <class T>
		struct MostDerived<NullType, T> {
			typedef T Result;
		};

		template <class Head, class Tail, class T>
		struct MostDerived<Typelist<Head, Tail>, T> {
		private:
			typedef typename MostDerived<Tail, T>::Result Candidate;
		public:
			typedef typename TypeSelect<SuperSubclass<Candidate, Head>::value, Head, Candidate>::Result Result;
		};

////////////////////////////////////////////////////////////////////////////////
// class template DerivedToFront
// Arranges the types in a typelist so that the most derived types appear first
// Invocation (TList is a typelist):
// DerivedToFront<TList>::Result
// returns the reordered TList
////////////////////////////////////////////////////////////////////////////////

		template <class TList> struct DerivedToFront;

		template <>
		struct DerivedToFront<NullType> {
			typedef NullType Result;
		};

		template <class Head, class Tail>
		struct DerivedToFront< Typelist<Head, Tail> > {
		private:
			typedef typename MostDerived<Tail, Head>::Result TheMostDerived;
			typedef typename Replace<Tail, TheMostDerived, Head>::Result Temp;
			typedef typename DerivedToFront<Temp>::Result L;
		public:
			typedef Typelist<TheMostDerived, L> Result;
		};

	}   // namespace TL


	template <
		class T01 = NullType, class T02 = NullType, class T03 = NullType, class T04 = NullType, class T05 = NullType,
		class T06 = NullType, class T07 = NullType, class T08 = NullType, class T09 = NullType, class T10 = NullType,
		class T11 = NullType, class T12 = NullType, class T13 = NullType, class T14 = NullType, class T15 = NullType,
		class T16 = NullType, class T17 = NullType, class T18 = NullType, class T19 = NullType, class T20 = NullType
	>
	class Seq {
		typedef typename Seq<T02, T03, T04, T05, T06, T07, T08, T09, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20>::list TailResult;
	public:
		typedef Typelist<T01, TailResult> list;
	};

	template<>
	struct Seq<> {
		typedef NullType list;
	};

}   // namespace tp

#if 0
#define TYPELIST1(T1)                                       ::compiler::Typelist<T1, ::compiler::NullType>
#define TYPELIST2(T1, T2)                                   ::compiler::Typelist<T1, TYPELIST1(T2) >
#define TYPELIST3(T1, T2, T3)                               ::compiler::Typelist<T1, TYPELIST2(T2, T3) >
#define TYPELIST4(T1, T2, T3, T4)                           ::compiler::Typelist<T1, TYPELIST3(T2, T3, T4) >
#define TYPELIST5(T1, T2, T3, T4, T5)                       ::compiler::Typelist<T1, TYPELIST4(T2, T3, T4, T5) >
#define TYPELIST6(T1, T2, T3, T4, T5, T6)                   ::compiler::Typelist<T1, TYPELIST5(T2, T3, T4, T5, T6) >
#define TYPELIST7(T1, T2, T3, T4, T5, T6, T7)               ::compiler::Typelist<T1, TYPELIST6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST8(T1, T2, T3, T4, T5, T6, T7, T8)           ::compiler::Typelist<T1, TYPELIST7(T2, T3, T4, T5, T6, T7, T8) >
#define TYPELIST9(T1, T2, T3, T4, T5, T6, T7, T8, T9)       ::compiler::Typelist<T1, TYPELIST8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) ::compiler::Typelist<T1, TYPELIST9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#endif

#include <limits>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4180 ) //qualifier applied to function type has no meaning; ignored
#endif

namespace tp {
////////////////////////////////////////////////////////////////////////////////
// class template IsCustomUnsignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomUnsignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in unsigned
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	struct IsCustomUnsignedInt {
		enum { value = 0 };
	};

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomSignedInt
// Offers a means to integrate nonstandard built-in unsigned integral types
// (such as unsigned __int64 or unsigned long long int) with the TypeTraits
//     class template defined below.
// Invocation: IsCustomSignedInt<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in signed
//     integral type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	struct IsCustomSignedInt {
		enum { value = 0 };
	};

////////////////////////////////////////////////////////////////////////////////
// class template IsCustomFloat
// Offers a means to integrate nonstandard floating point types with the
//     TypeTraits class template defined below.
// Invocation: IsCustomFloat<T> where T is any type
// Defines 'value', an enum that is 1 iff T is a custom built-in
//     floating point type
// Specialize this class template for nonstandard unsigned integral types
//     and define value = 1 in those specializations
////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	struct IsCustomFloat {
		enum { value = 0 };
	};

////////////////////////////////////////////////////////////////////////////////
// Helper types for class template TypeTraits defined below
////////////////////////////////////////////////////////////////////////////////

	namespace Private {

		typedef Seq<unsigned char, unsigned short int, unsigned int, unsigned long int>::list StdUnsignedInts;
		typedef Seq<signed char, short int, int, long int>::list StdSignedInts;
		typedef Seq<bool, char, wchar_t>::list StdOtherInts;
		typedef Seq<float, double, long double>::list StdFloats;

		template <typename U> struct AddPointer { typedef U* Result; };
		template <typename U> struct AddPointer<U&> { typedef U* Result; };

		template <class U> struct AddReference { typedef U& Result; };
		template <class U> struct AddReference<U&> { typedef U& Result; };
		template <> struct AddReference<void> { typedef NullType Result; };

		template <class U> struct AddParameterType { typedef const U& Result; };
		template <class U> struct AddParameterType<U&> { typedef U& Result; };
		template <> struct AddParameterType<void> { typedef NullType Result; };

	}// namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template TypeTraits
//
// Figures out at compile time various properties of any given type
// Invocations (T is a type, TypeTraits<T>::Property):
//
// - isPointer       : returns true if T is a pointer type
// - PointeeType     : returns the type to which T points if T is a pointer
//                     type, NullType otherwise
// - isReference     : returns true if T is a reference type
// - ReferredType    : returns the type to which T refers if T is a reference
//                     type, NullType otherwise
// - isMemberPointer : returns true if T is a pointer to member type
// - isStdUnsignedInt: returns true if T is a standard unsigned integral type
// - isStdSignedInt  : returns true if T is a standard signed integral type
// - isStdIntegral   : returns true if T is a standard integral type
// - isStdFloat      : returns true if T is a standard floating-point type
// - isStdArith      : returns true if T is a standard arithmetic type
// - isStdFundamental: returns true if T is a standard fundamental type
// - isUnsignedInt   : returns true if T is a unsigned integral type
// - isSignedInt     : returns true if T is a signed integral type
// - isIntegral      : returns true if T is a integral type
// - isFloat         : returns true if T is a floating-point type
// - isArith         : returns true if T is a arithmetic type
// - isFundamental   : returns true if T is a fundamental type
// - ParameterType   : returns the optimal type to be used as a parameter for
//                     functions that take Ts
// - isConst         : returns true if T is a const-qualified type
// - NonConstType    : Type with removed 'const' qualifier from T, if any
// - isVolatile      : returns true if T is a volatile-qualified type
// - NonVolatileType : Type with removed 'volatile' qualifier from T, if any
// - UnqualifiedType : Type with removed 'const' and 'volatile' qualifiers from
//                     T, if any
// - ParameterType   : returns the optimal type to be used as a parameter
//                       for functions that take 'const T's
//
////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	class TypeTraits {
	private:

		template <class U> struct ReferenceTraits {
			enum { result = false };
			typedef U ReferredType;
		};

		template <class U> struct ReferenceTraits<U&> {
			enum { result = true };
			typedef U ReferredType;
		};

		template <class U> struct PointerTraits {
			enum { result = false };
			typedef NullType PointeeType;
		};

		template <class U> struct PointerTraits<U*> {
			enum { result = true };
			typedef U PointeeType;
		};

		template <class U> struct PointerTraits<U*&> {
			enum { result = true };
			typedef U PointeeType;
		};

		template <class U> struct PToMTraits {
			enum { result = false };
		};

		template <class U, class V> struct PToMTraits<U V::*> {
			enum { result = true };
		};

		template <class U, class V> struct PToMTraits<U V::*&> {
			enum { result = true };
		};

		template <class U> struct UnConst {
			typedef U Result;
			enum { isConst = 0 };
		};

		template <class U> struct UnConst<const U> {
			typedef U Result;
			enum { isConst = 1 };
		};

		template <class U> struct UnConst<const U&> {
			typedef U& Result;
			enum { isConst = 1 };
		};

		template <class U> struct UnVolatile {
			typedef U Result;
			enum { isVolatile = 0 };
		};

		template <class U> struct UnVolatile<volatile U> {
			typedef U Result;
			enum { isVolatile = 1 };
		};

		template <class U> struct UnVolatile<volatile U&> {
			typedef U& Result;
			enum { isVolatile = 1 };
		};

	public:
		typedef typename UnConst<T>::Result NonConstType;
		typedef typename UnVolatile<T>::Result NonVolatileType;
		typedef typename UnVolatile<typename UnConst<T>::Result>::Result UnqualifiedType;
		typedef typename PointerTraits<UnqualifiedType>::PointeeType PointeeType;
		typedef typename ReferenceTraits<T>::ReferredType ReferredType;

		enum { isConst = UnConst<T>::isConst };
		enum { isVolatile = UnVolatile<T>::isVolatile };
		enum { isReference = ReferenceTraits<UnqualifiedType>::result };
		enum { isMemberPointer = PToMTraits<typename ReferenceTraits<UnqualifiedType>::ReferredType >::result };
		enum { isPointer = PointerTraits<typename ReferenceTraits<UnqualifiedType>::ReferredType >::result };

		enum {
			isStdUnsignedInt = TL::IndexOf<Private::StdUnsignedInts, UnqualifiedType>::value >= 0 ||
				TL::IndexOf<Private::StdUnsignedInts,
				typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0
		};
		enum {
			isStdSignedInt = TL::IndexOf<Private::StdSignedInts, UnqualifiedType>::value >= 0 ||
				TL::IndexOf<Private::StdSignedInts,
				typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0
		};
		enum {
			isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
				TL::IndexOf<Private::StdOtherInts, UnqualifiedType>::value >= 0 ||
				TL::IndexOf<Private::StdOtherInts,
				typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0
		};
		enum {
			isStdFloat = TL::IndexOf<Private::StdFloats, UnqualifiedType>::value >= 0 ||
				TL::IndexOf<Private::StdFloats,
				typename ReferenceTraits<UnqualifiedType>::ReferredType>::value >= 0
		};

		enum { isStdArith = isStdIntegral || isStdFloat };
		enum { isStdFundamental = isStdArith || isStdFloat || Conversion<T, void>::sameType };

		enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<UnqualifiedType>::value };
		enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<UnqualifiedType>::value };
		enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
		enum { isFloat = isStdFloat || IsCustomFloat<UnqualifiedType>::value };
		enum { isArith = isIntegral || isFloat };
		enum { isFundamental = isStdFundamental || isArith };

		typedef typename TypeSelect<isStdArith || isPointer || isMemberPointer, T, typename Private::AddParameterType<T>::Result>::Result ParameterType;
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER