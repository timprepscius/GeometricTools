// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename T>
UnorderedSet<T>::UnorderedSet (int maxNumElements, int grow)
{
    if (maxNumElements == 0)
    {
        maxNumElements = DEFAULT_GROW;
    }

    if (grow == 0)
    {
        grow = DEFAULT_GROW;
    }

    mNumElements = 0;
    mMaxNumElements = maxNumElements;
    mGrow = grow;
    mElements = new1<T>(mMaxNumElements);
}
//----------------------------------------------------------------------------
template <typename T>
UnorderedSet<T>::UnorderedSet (const UnorderedSet& uset)
{
    mElements = 0;
    *this = uset;
}
//----------------------------------------------------------------------------
template <typename T>
UnorderedSet<T>::~UnorderedSet ()
{
    delete1(mElements);
}
//----------------------------------------------------------------------------
template <typename T>
void UnorderedSet<T>::Clear ()
{
    mNumElements = 0;
}
//----------------------------------------------------------------------------
template <typename T>
void UnorderedSet<T>::Reset (int maxNumElements, int grow)
{
    if (maxNumElements == 0)
    {
        maxNumElements = DEFAULT_GROW;
    }

    if (grow == 0)
    {
        grow = DEFAULT_GROW;
    }

    mNumElements = 0;
    mMaxNumElements = maxNumElements;
    mGrow = grow;
    delete1(mElements);
    mElements = new1<T>(mMaxNumElements);
}
//----------------------------------------------------------------------------
template <typename T>
UnorderedSet<T>& UnorderedSet<T>::operator= (const UnorderedSet& uset)
{
    mNumElements = uset.mNumElements;
    mMaxNumElements = uset.mMaxNumElements;
    mGrow = uset.mGrow;
    delete1(mElements);
    mElements = new1<T>(mMaxNumElements);
    for (int i = 0; i < mMaxNumElements; ++i)
    {
        mElements[i] = uset.mElements[i];
    }

    return *this;
}
//----------------------------------------------------------------------------
template <typename T>
int UnorderedSet<T>::GetMaxNumElements () const
{
    return mMaxNumElements;
}
//----------------------------------------------------------------------------
template <typename T>
int UnorderedSet<T>::GetGrow () const
{
    return mGrow;
}
//----------------------------------------------------------------------------
template <typename T>
int UnorderedSet<T>::GetNumElements () const
{
    return mNumElements;
}
//----------------------------------------------------------------------------
template <typename T>
const T& UnorderedSet<T>::Get (int i) const
{
    return mElements[i];
}
//----------------------------------------------------------------------------
template <typename T>
T& UnorderedSet<T>::operator[] (int i)
{
    return mElements[i];
}
//----------------------------------------------------------------------------
template <typename T>
bool UnorderedSet<T>::Exists (const T& element) const
{
    for (int i = 0; i < mNumElements; ++i)
    {
        if (element == mElements[i])
        {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename T>
bool UnorderedSet<T>::Insert (const T& element)
{
    int i;
    for (i = 0; i < mNumElements; ++i)
    {
        if (element == mElements[i])
        {
            return false;
        }
    }

    GrowArray();

    mElements[mNumElements++] = element;
    return true;
}
//----------------------------------------------------------------------------
template <typename T>
int UnorderedSet<T>::Append (const T& element)
{
    GrowArray();

    int location = mNumElements++;
    mElements[location] = element;
    return location;
}
//----------------------------------------------------------------------------
template <typename T>
bool UnorderedSet<T>::Remove (const T& element, int* oldIndex, int* newIndex)
{
    for (int i = 0; i < mNumElements; i++)
    {
        if (element == mElements[i])
        {
            RemoveElement(i, oldIndex, newIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename T>
bool UnorderedSet<T>::RemoveAt (int i, int* oldIndex, int* newIndex)
{
    if (0 <= i && i < mNumElements)
    {
        RemoveElement(i, oldIndex, newIndex);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename T>
void UnorderedSet<T>::Compactify ()
{
    if (mNumElements > 0)
    {
        T* newElements = new1<T>(mNumElements);
        for (int i = 0; i < mNumElements; ++i)
        {
            newElements[i] = mElements[i];
        }
        delete1(mElements);
        mMaxNumElements = mNumElements;
        mElements = newElements;
    }
    else
    {
        mNumElements = 0;
        mMaxNumElements = DEFAULT_GROW;
        mGrow = DEFAULT_GROW;
        delete1(mElements);
        mElements = new1<T>(mMaxNumElements);
    }
}
//----------------------------------------------------------------------------
template <typename T>
void UnorderedSet<T>::GrowArray ()
{
    if (mNumElements == mMaxNumElements)
    {
        int newMaxNumElements = mMaxNumElements + mGrow;
        T* newElements = new1<T>(newMaxNumElements);
        for (int i = 0; i < mMaxNumElements; ++i)
        {
            newElements[i] = mElements[i];
        }
        delete1(mElements);
        mMaxNumElements = newMaxNumElements;
        mElements = newElements;
    }
}
//----------------------------------------------------------------------------
template <typename T>
void UnorderedSet<T>::RemoveElement (int i, int* oldIndex, int* newIndex)
{
    --mNumElements;
    if (oldIndex)
    {
        *oldIndex = mNumElements;
    }

    if (i != mNumElements)
    {
        mElements[i] = mElements[mNumElements];
        if (newIndex)
        {
            *newIndex = i;
        }
    }
    else
    {
        if (newIndex)
        {
            *newIndex = -1;
        }
    }
}
//----------------------------------------------------------------------------
