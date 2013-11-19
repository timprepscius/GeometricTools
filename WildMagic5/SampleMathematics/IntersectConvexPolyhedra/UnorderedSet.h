// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef UNORDEREDSET_H
#define UNORDEREDSET_H

#include "Wm5Memory.h"

// An unordered set of objects stored in contiguous memory.  The type T must
// have the following member functions:
//   T::T();
//   T::~T();
//   T& operator= (const T&);
//   bool operator== (const T&) const;

template <typename T>
class UnorderedSet
{
public:
    UnorderedSet (int maxNumElements = 0, int grow = 0);
    UnorderedSet (const UnorderedSet& uset);
    ~UnorderedSet ();

    void Reset (int maxNumElements = 0, int grow = 0);
    void Clear ();
    UnorderedSet& operator= (const UnorderedSet& uset);

    int GetMaxNumElements () const;
    int GetGrow () const;

    int GetNumElements () const;
    const T& Get (int i) const;
    T& operator[] (int i);

    bool Exists (const T& element) const;
    bool Insert (const T& element);
    int Append (const T& element);
    bool Remove (const T& element, int* oldIndex = 0, int* newIndex = 0);
    bool RemoveAt (int i, int* oldIndex = 0, int* newIndex = 0);
    void Compactify ();

    enum { DEFAULT_GROW = 8 };

protected:
    void GrowArray ();
    void RemoveElement (int i, int* oldIndex, int* newIndex);

    int mMaxNumElements, mGrow, mNumElements;
    T* mElements;
};

#include "UnorderedSet.inl"

#endif
