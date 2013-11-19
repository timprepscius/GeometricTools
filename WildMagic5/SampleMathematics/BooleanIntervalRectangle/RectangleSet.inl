// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Scalar>
RectangleSet<Scalar>::RectangleSet ()
    :
    mNumRectangles(0)
{
}
//----------------------------------------------------------------------------
template <typename Scalar>
RectangleSet<Scalar>::RectangleSet (Scalar xmin, Scalar xmax, Scalar ymin,
    Scalar ymax)
{
    if (xmin < xmax && ymin < ymax)
    {
        mNumRectangles = 1;
        mStrips.push_back(Strip(ymin,ymax,
            new0 IntervalSet<Scalar>(xmin, xmax)));
    }
    else
    {
        mNumRectangles = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
RectangleSet<Scalar>::~RectangleSet ()
{
}
//----------------------------------------------------------------------------
template <typename Scalar>
int RectangleSet<Scalar>::GetNumRectangles () const
{
    return mNumRectangles;
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool RectangleSet<Scalar>::GetRectangle (int i, Scalar& xmin, Scalar& xmax,
    Scalar& ymin, Scalar& ymax) const
{
    const int numStrips = (int)mStrips.size();
    for (int j = 0, totalQuantity = 0; j < numStrips; ++j)
    {
        const IntervalSet<Scalar>& intervals = *mStrips[j].Intervals;
        int xQuantity = intervals.GetNumIntervals();
        int nextTotalQuantity = totalQuantity + xQuantity;
        if (i < nextTotalQuantity)
        {
            i -= totalQuantity;
            intervals.GetInterval(i, xmin, xmax);
            return true;
        }
        totalQuantity = nextTotalQuantity;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::Clear ()
{
    mNumRectangles = 0;
    mStrips.clear();
}
//----------------------------------------------------------------------------
template <typename Scalar>
int RectangleSet<Scalar>::GetNumStrips() const
{
    return (int)mStrips.size();
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool RectangleSet<Scalar>::GetStrip (int i, Scalar& ymin, Scalar& ymax,
    const IntervalSet<Scalar>* xIntervals) const
{
    if (0 <= i && i < GetNumStrips())
    {
        const Strip& strip = mStrips[i];
        ymin = strip.Min;
        ymax = strip.Max;
        if (xIntervals)
        {
            xIntervals = strip.Intervals;
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool RectangleSet<Scalar>::Insert (Scalar xmin, Scalar xmax, Scalar ymin,
    Scalar ymax)
{
    if (xmin < xmax && ymin < ymax)
    {
        RectangleSet input(xmin, xmax, ymin, ymax), output;
        Union(*this, input, output);
        *this = output;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool RectangleSet<Scalar>::Remove (Scalar xmin, Scalar xmax, Scalar ymin,
    Scalar ymax)
{
    if (xmin < xmax && ymin < ymax)
    {
        RectangleSet input(xmin, xmax, ymin, ymax), output;
        Difference(*this, input, output);
        *this = output;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::Union (const RectangleSet& input0,
    const RectangleSet& input1, RectangleSet& output)
{
    Operate(&IntervalSet<Scalar>::Union, input0, input1, output);
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::Intersection (const RectangleSet& input0,
    const RectangleSet& input1, RectangleSet& output)
{
    Operate(&IntervalSet<Scalar>::Intersection, input0, input1, output);
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::Difference (const RectangleSet& input0,
    const RectangleSet& input1, RectangleSet& output)
{
    Operate(&IntervalSet<Scalar>::Difference, input0, input1, output);
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::ExclusiveOr (const RectangleSet& input0,
    const RectangleSet& input1, RectangleSet& output)
{
    Operate(&IntervalSet<Scalar>::ExclusiveOr, input0, input1, output);
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::ComputeRectangleQuantity ()
{
    const int numStrips = (int)mStrips.size();
    mNumRectangles = 0;
    for (int i = 0; i < numStrips; ++i)
    {
        mNumRectangles += mStrips[i].Intervals->GetNumIntervals();
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
void RectangleSet<Scalar>::Operate (
    typename IntervalSet<Scalar>::Operator opFunction,
    const RectangleSet& input0, const RectangleSet& input1,
    RectangleSet& output)
{
    output.Clear();

    bool unionExclusiveOr = (opFunction == &IntervalSet<Scalar>::Union ||
        opFunction == &IntervalSet<Scalar>::ExclusiveOr);

    bool unionExclusiveOrDifference = (unionExclusiveOr ||
        opFunction == &IntervalSet<Scalar>::Difference);

    const int numStrips0 = input0.GetNumStrips();
    const int numStrips1 = input1.GetNumStrips();
    int i0 = 0, i1 = 0;
    bool getOriginal0 = true, getOriginal1 = true;
    Scalar min0 = (Scalar)0;
    Scalar max0 = (Scalar)0;
    Scalar min1 = (Scalar)0;
    Scalar max1 = (Scalar)0;
    IntervalSet<Scalar>* result;

    while (i0 < numStrips0 && i1 < numStrips1)
    {
        IntervalSet<Scalar>& intr0 = *input0.mStrips[i0].Intervals;
        if (getOriginal0)
        {
            min0 = input0.mStrips[i0].Min;
            max0 = input0.mStrips[i0].Max;
        }

        IntervalSet<Scalar>& intr1 = *input1.mStrips[i1].Intervals;
        if (getOriginal1)
        {
            min1 = input1.mStrips[i1].Min;
            max1 = input1.mStrips[i1].Max;
        }

        // Case 1.
        if (max1 <= min0)
        {
            // operator(empty,strip1)
            if (unionExclusiveOr)
            {
                output.mStrips.push_back(Strip(min1, max1,
                    new0 IntervalSet<Scalar>(intr1)));
            }

            ++i1;
            getOriginal0 = false;
            getOriginal1 = true;
            continue;  // using next min1/max1
        }

        // Case 11.
        if (min1 >= max0)
        {
            // operator(strip0,empty)
            if (unionExclusiveOrDifference)
            {
                output.mStrips.push_back(Strip(min0, max0,
                    new0 IntervalSet<Scalar>(intr0)));
            }

            ++i0;
            getOriginal0 = true;
            getOriginal1 = false;
            continue;  // using next min0/max0
        }

        // Reduce cases 2, 3, 4 to cases 5, 6, 7.
        if (min1 < min0)
        {
            // operator(empty,[min1,min0))
            if (unionExclusiveOr)
            {
                output.mStrips.push_back(Strip(min1, min0,
                    new0 IntervalSet<Scalar>(intr1)));
            }

            min1 = min0;
            getOriginal1 = false;
        }

        // Reduce cases 8, 9, 10 to cases 5, 6, 7.
        if (min1 > min0)
        {
            // operator([min0,min1),empty)
            if (unionExclusiveOrDifference)
            {
                output.mStrips.push_back(Strip(min0, min1,
                    new0 IntervalSet<Scalar>(intr0)));
            }

            min0 = min1;
            getOriginal0 = false;
        }

        // Case 5.
        if (max1 < max0)
        {
            // operator(strip0,[min1,max1))
            result = new0 IntervalSet<Scalar>;
            opFunction(intr0, intr1, *result);
            output.mStrips.push_back(Strip(min1, max1, result));

            min0 = max1;
            ++i1;
            getOriginal0 = false;
            getOriginal1 = true;
            continue;  // using next min1/max1
        }

        // Case 6.
        if (max1 == max0)
        {
            // operator(strip0,[min1,max1))
            result = new0 IntervalSet<Scalar>;
            opFunction(intr0, intr1, *result);
            output.mStrips.push_back(Strip(min1, max1, result));

            ++i0;
            ++i1;
            getOriginal0 = true;
            getOriginal1 = true;
            continue;  // using next min0/max0 and min1/max1
        }

        // Case 7.
        if (max1 > max0)
        {
            // operator(strip0,[min1,max0))
            result = new0 IntervalSet<Scalar>;
            opFunction(intr0, intr1, *result);
            output.mStrips.push_back(Strip(min1, max0, result));

            min1 = max0;
            ++i0;
            getOriginal0 = true;
            getOriginal1 = false;
            // continue;  using current min1/max1
        }
    }

    if (unionExclusiveOrDifference)
    {
        while (i0 < numStrips0)
        {
            if (getOriginal0)
            {
                min0 = input0.mStrips[i0].Min;
                max0 = input0.mStrips[i0].Max;
            }
            else
            {
                getOriginal0 = true;
            }

            // operator(strip0,empty)
            output.mStrips.push_back(Strip(min0, max0,
                new0 IntervalSet<Scalar>(*input0.mStrips[i0].Intervals)));

            ++i0;
        }
    }

    if (unionExclusiveOr)
    {
        while (i1 < numStrips1)
        {
            if (getOriginal1)
            {
                min1 = input1.mStrips[i1].Min;
                max1 = input1.mStrips[i1].Max;
            }
            else
            {
                getOriginal1 = true;
            }

            // operator(empty,strip1)
            output.mStrips.push_back(Strip(min1, max1,
                new0 IntervalSet<Scalar>(*input1.mStrips[i1].Intervals)));

            ++i1;
        }
    }

    output.ComputeRectangleQuantity();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// RectangleSet::Strip
//----------------------------------------------------------------------------
template <typename Scalar>
RectangleSet<Scalar>::Strip::Strip (Scalar ymin, Scalar ymax,
    IntervalSet<Scalar>*intervals)
    :
    Min(ymin),
    Max(ymax),
    Intervals(intervals)
{
}
//----------------------------------------------------------------------------
template <typename Scalar>
RectangleSet<Scalar>::Strip::~Strip ()
{
}
//----------------------------------------------------------------------------
