////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef KTYPETRAITS_H
#define KTYPETRAITS_H

#include <ktypelist.h>

#include "ktypemanip.h"

namespace Gadget
{
    /**
     * class template IsCustomUnsignedInt
     * Offers a means to integrate nonstandard built-in unsigned integral types
     * (such as unsigned __int64 or unsigned long long int) with the TypeTraits
     *     class template defined below.
     * Invocation: IsCustomUnsignedInt<T> where T is any type
     * Defines 'value', an enum that is 1 iff T is a custom built-in unsigned
     *     integral type
     * Specialize this class template for nonstandard unsigned integral types
     *     and define value = 1 in those specializations
     */
    template <typename T>
    struct IsCustomUnsignedInt
    {
        enum { value = 0 };
    };

    /**
    * class template IsCustomSignedInt
    * Offers a means to integrate nonstandard built-in unsigned integral types
    * (such as unsigned __int64 or unsigned long long int) with the TypeTraits
    *     class template defined below.
    * Invocation: IsCustomSignedInt<T> where T is any type
    * Defines 'value', an enum that is 1 iff T is a custom built-in signed
    *     integral type
    * Specialize this class template for nonstandard unsigned integral types
    *     and define value = 1 in those specializations
    */
    template <typename T>
    struct IsCustomSignedInt
    {
        enum { value = 0 };
    };

    /**
     * class template IsCustomFloat
     * Offers a means to integrate nonstandard floating point types with the
     *     TypeTraits class template defined below.
     * Invocation: IsCustomFloat<T> where T is any type
     * Defines 'value', an enum that is 1 iff T is a custom built-in
     *     floating point type
     * Specialize this class template for nonstandard unsigned integral types
     *     and define value = 1 in those specializations
     */
    template <typename T>
    struct IsCustomFloat
    {
        enum { value = 0 };
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Helper types for class template TypeTraits defined below
    ////////////////////////////////////////////////////////////////////////////////
    namespace Private
    {
        typedef K_TYPELIST_4(unsigned char, unsigned short int,
                             unsigned int, unsigned long int) StdUnsignedInts;
        typedef K_TYPELIST_4(signed char, short int,
                             int, long int) StdSignedInts;
        typedef K_TYPELIST_3(bool, char, wchar_t) StdOtherInts;
        typedef K_TYPELIST_3(float, double, long double) StdFloats;

        template <class U> struct AddReference
        {
            typedef U & Result;
        };

        template <class U> struct AddReference<U &>
        {
            typedef U & Result;
        };

        template <> struct AddReference<void>
        {
            typedef KDE::NullType Result;
        };
    }

    /**
     * class template TypeTraits
     * Figures out various properties of any given type
     * Invocations (T is a type):
     * a) TypeTraits<T>::isPointer
     * returns (at compile time) true if T is a pointer type
     * b) TypeTraits<T>::PointeeType
     * returns the type to which T points is T is a pointer type, NullType otherwise
     * a) TypeTraits<T>::isReference
     * returns (at compile time) true if T is a reference type
     * b) TypeTraits<T>::ReferredType
     * returns the type to which T refers is T is a reference type, NullType
     * otherwise
     * c) TypeTraits<T>::isMemberPointer
     * returns (at compile time) true if T is a pointer to member type
     * d) TypeTraits<T>::isStdUnsignedInt
     * returns (at compile time) true if T is a standard unsigned integral type
     * e) TypeTraits<T>::isStdSignedInt
     * returns (at compile time) true if T is a standard signed integral type
     * f) TypeTraits<T>::isStdIntegral
     * returns (at compile time) true if T is a standard integral type
     * g) TypeTraits<T>::isStdFloat
     * returns (at compile time) true if T is a standard floating-point type
     * h) TypeTraits<T>::isStdArith
     * returns (at compile time) true if T is a standard arithmetic type
     * i) TypeTraits<T>::isStdFundamental
     * returns (at compile time) true if T is a standard fundamental type
     * j) TypeTraits<T>::isUnsignedInt
     * returns (at compile time) true if T is a unsigned integral type
     * k) TypeTraits<T>::isSignedInt
     * returns (at compile time) true if T is a signed integral type
     * l) TypeTraits<T>::isIntegral
     * returns (at compile time) true if T is a integral type
     * m) TypeTraits<T>::isFloat
     * returns (at compile time) true if T is a floating-point type
     * n) TypeTraits<T>::isArith
     * returns (at compile time) true if T is a arithmetic type
     * o) TypeTraits<T>::isFundamental
     * returns (at compile time) true if T is a fundamental type
     * p) TypeTraits<T>::ParameterType
     * returns the optimal type to be used as a parameter for functions that take Ts
     * q) TypeTraits<T>::isConst
     * returns (at compile time) true if T is a const-qualified type
     * r) TypeTraits<T>::NonConstType
     * removes the 'const' qualifier from T, if any
     * s) TypeTraits<T>::isVolatile
     * returns (at compile time) true if T is a volatile-qualified type
     * t) TypeTraits<T>::NonVolatileType
     * removes the 'volatile' qualifier from T, if any
     * u) TypeTraits<T>::UnqualifiedType
     * removes both the 'const' and 'volatile' qualifiers from T, if any
     */
    template <typename T>
    class TypeTraits
    {
    private:
        template <class U> struct PointerTraits
        {
            enum { result = false };
            typedef KDE::NullType PointeeType;
        };

        template <class U> struct PointerTraits<U*>
        {
            enum { result = true };
            typedef U PointeeType;
        };

        template <class U> struct ReferenceTraits
        {
            enum { result = false };
            typedef U ReferredType;
        };

        template <class U> struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U ReferredType;
        };

        template <class U> struct PToMTraits
        {
            enum { result = false };
        };

        template <class U, class V>
        struct PToMTraits<U V::*>
        {
            enum { result = true };
        };

        template <class U> struct UnConst
        {
            typedef U Result;
            enum { isConst = 0 };
        };

        template <class U> struct UnConst<const U>
        {
            typedef U Result;
            enum { isConst = 1 };
        };

        template <class U> struct UnConst<const U*>
        {
            typedef U* Result;
            enum { isConst = 1 };
        };

        template <class U> struct UnVolatile
        {
            typedef U Result;
            enum { isVolatile = 0 };
        };

        template <class U> struct UnVolatile<volatile U>
        {
            typedef U Result;
            enum { isVolatile = 1 };
        };

    public:
        enum { isPointer = PointerTraits<T>::result };
        typedef typename PointerTraits<T>::PointeeType PointeeType;

        enum { isReference = ReferenceTraits<T>::result };
        typedef typename ReferenceTraits<T>::ReferredType ReferredType;

        enum { isMemberPointer = PToMTraits<T>::result };

        enum { isStdUnsignedInt =
               KTypeListIndexOf<Private::StdUnsignedInts, T>::value >= 0 };
        enum { isStdSignedInt =
               KTypeListIndexOf<Private::StdSignedInts, T>::value >= 0 };
        enum { isStdIntegral = isStdUnsignedInt || isStdSignedInt ||
               KTypeListIndexOf<Private::StdOtherInts, T>::value >= 0 };
        enum { isStdFloat = KTypeListIndexOf<Private::StdFloats, T>::value >= 0 };
        enum { isStdArith = isStdIntegral || isStdFloat };
        enum { isStdFundamental = isStdArith || isStdFloat ||
               Conversion<T, void>::sameType };

        enum { isUnsignedInt = isStdUnsignedInt || IsCustomUnsignedInt<T>::value };
        enum { isSignedInt = isStdSignedInt || IsCustomSignedInt<T>::value };
        enum { isIntegral = isStdIntegral || isUnsignedInt || isSignedInt };
        enum { isFloat = isStdFloat || IsCustomFloat<T>::value };
        enum { isArith = isIntegral || isFloat };
        enum { isFundamental = isStdFundamental || isArith || isFloat };

        typedef typename Select<isStdArith || isPointer || isMemberPointer,
            T, typename Private::AddReference<T>::Result>::Result ParameterType;

        enum { isConst = UnConst<T>::isConst };
        typedef typename UnConst<T>::Result NonConstType;
        enum { isVolatile = UnVolatile<T>::isVolatile };
        typedef typename UnVolatile<T>::Result NonVolatileType;
        typedef typename UnVolatile<typename UnConst<T>::Result>::Result
            UnqualifiedType;
    };
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// September 16, 2002: ParameterType fixed, as TypeTraits<void> made
//     ParameterType give error about reference to void. T.S.
////////////////////////////////////////////////////////////////////////////////

#endif // KTYPETRAITS_H
