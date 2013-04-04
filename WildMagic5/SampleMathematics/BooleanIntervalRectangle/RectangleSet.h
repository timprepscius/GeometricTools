// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef RECTANGLESET_H
#define RECTANGLESET_H

#include "IntervalSet.h"
#include "Wm5SmartPointer.h"

// Disjoint sets of half-open rectangles of the form [xmin,xmax)x[ymin,ymax)
// with xmin < xmax and ymin < ymax.
template <typename Scalar>
class RectangleSet
{
public:
    // Construction and destruction.  The non-default constructor requires
    // that xmin < xmax and ymin < ymax.
    RectangleSet ();
    RectangleSet (Scalar xmin, Scalar xmax, Scalar ymin, Scalar ymax);
    ~RectangleSet ();

    // The rectangle set consists of y-strips of interval sets.
    class Strip
    {
    public:
        // Construction and destruction.
        Strip (Scalar ymin = (Scalar)0, Scalar ymax = (Scalar)0,
            IntervalSet<Scalar>*intervals = 0);

        ~Strip ();

        // Member access.
        Scalar Min, Max;
        Pointer0<IntervalSet<Scalar> > Intervals;
    };

    // The number of rectangles in the set.
    int GetNumRectangles () const;

    // The i-th rectangle is [xmin,xmax)x[ymin,ymax).  The values xmin, xmax,
    // ymin, and ymax are valid only when 0 <= i < GetNumRectangles().
    bool GetRectangle (int i, Scalar& xmin, Scalar& xmax, Scalar& ymin,
        Scalar& ymax) const;

    // Make this set empty.
    void Clear ();

    // The number of y-strips in the set.
    int GetNumStrips () const;

    // The i-th strip.  The returned values are valid only when
    // 0 <= i < GetStripQuantity().
    bool GetStrip (int i, Scalar& ymin, Scalar& ymax,
        const IntervalSet<Scalar>* xIntervals = 0) const;

    // Insert [xmin,xmax)x[ymin,ymax) into the set.  This is a Boolean
    // union operation.  The operation is successful only when xmin < xmax
    // and ymin < ymax.
    bool Insert (Scalar xmin, Scalar xmax, Scalar ymin, Scalar ymax);

    // Remove [xmin,xmax)x[ymin,ymax) from the set.  This is a Boolean
    // difference operation.  The operation is successful only when
    // xmin < xmax and ymin < ymax.
    bool Remove (Scalar xmin, Scalar xmax, Scalar ymin, Scalar ymax);

    // Boolean operations of sets.
    static void Union (const RectangleSet& input0,
        const RectangleSet& input1, RectangleSet& output);

    static void Intersection (const RectangleSet& input0,
        const RectangleSet& input1, RectangleSet& output);

    static void Difference (const RectangleSet& input0,
        const RectangleSet& input1, RectangleSet& output);

    static void ExclusiveOr (const RectangleSet& input0,
        const RectangleSet& input1, RectangleSet& output);

private:
    static void Operate (typename IntervalSet<Scalar>::Operator opFunction,
        const RectangleSet& input0, const RectangleSet& input1,
        RectangleSet& output);

    void ComputeRectangleQuantity ();

    // The number of rectangles in the set.
    int mNumRectangles;

    // The y-strips of the set, each containing an x-interval set.
    std::vector<Strip> mStrips;
};

#include "RectangleSet.inl"

#endif
