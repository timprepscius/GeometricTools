// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef INTERVALSET_H
#define INTERVALSET_H

#include <vector>

// Disjoint sets of half-open intervals of the form [min,max) with min < max.
template <typename Scalar>
class IntervalSet
{
public:
    // Construction and destruction.  The non-default constructor requires
    // that xmin < xmax.
    IntervalSet ();
    IntervalSet (const IntervalSet& intervalSet);
    IntervalSet (Scalar xmin, Scalar xmax);
    ~IntervalSet ();

    // The number of intervals in the set.
    int GetNumIntervals () const;

    // The i-th interval is [xmin,xmax).  The values xmin and xmax are valid
    // only when 0 <= i < GetNumIntervals().
    bool GetInterval (int i, Scalar& xmin, Scalar& xmax) const;

    // Make this set empty.
    void Clear ();

    // Insert [xmin,xmax) into the set.  This is a Boolean 'union' operation.
    // The operation is successful only when xmin < xmax.
    bool Insert (Scalar xmin, Scalar xmax);

    // Remove [xmin,xmax) from the set.  This is a Boolean 'difference'
    // operation.  The operation is successful only when xmin < xmax.
    bool Remove (Scalar xmin, Scalar xmax);

    // Boolean operations of sets.  If you want to know the indices of the
    // input intervals that contributed to an output interval, pass an
    // array in the last component to store the pairs of indices.  It is
    // possible that a pair has a -1 component.  In this case, only the
    // nonnegative component contributed.
    static void Union (const IntervalSet& input0,
        const IntervalSet& input1, IntervalSet& output);

    static void Intersection (const IntervalSet& input0,
        const IntervalSet& input1, IntervalSet& output);

    static void Difference (const IntervalSet& input0,
        const IntervalSet& input1, IntervalSet& output);

    static void ExclusiveOr (const IntervalSet& input0,
        const IntervalSet& input1, IntervalSet& output);

    // For use by RectangleSet<Scalar>.
    typedef void (*Operator)(const IntervalSet&, const IntervalSet&,
        IntervalSet&);

private:
    // The array of endpoints has an even number of elements.  The i-th
    // interval is [mEndPoints[2*i],mEndPoints[2*i+1]).
    std::vector<Scalar> mEndpoints;
};

#include "IntervalSet.inl"

#endif
