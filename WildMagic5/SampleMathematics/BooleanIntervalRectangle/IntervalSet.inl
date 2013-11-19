// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Scalar>
IntervalSet<Scalar>::IntervalSet ()
{
}
//----------------------------------------------------------------------------
template <typename Scalar>
IntervalSet<Scalar>::IntervalSet (const IntervalSet& intervalSet)
{
    mEndpoints = intervalSet.mEndpoints;
}
//----------------------------------------------------------------------------
template <typename Scalar>
IntervalSet<Scalar>::IntervalSet (Scalar xmin, Scalar xmax)
    :
    mEndpoints(2)
{
    if (xmin < xmax)
    {
        mEndpoints[0] = xmin;
        mEndpoints[1] = xmax;
    }
    else
    {
        mEndpoints.clear();
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
IntervalSet<Scalar>::~IntervalSet ()
{
}
//----------------------------------------------------------------------------
template <typename Scalar>
int IntervalSet<Scalar>::GetNumIntervals () const
{
    return (int)mEndpoints.size()/2;
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool IntervalSet<Scalar>::GetInterval (int i, Scalar& xmin, Scalar& xmax)
    const
{
    int index = 2*i;
    if (0 <= index && index < (int)mEndpoints.size())
    {
        xmin = mEndpoints[index];
        xmax = mEndpoints[++index];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
void IntervalSet<Scalar>::Clear ()
{
    mEndpoints.clear();
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool IntervalSet<Scalar>::Insert (Scalar xmin, Scalar xmax)
{
    if (xmin < xmax)
    {
        IntervalSet input(xmin, xmax), output;
        Union(*this, input, output);
        *this = output;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
bool IntervalSet<Scalar>::Remove (Scalar xmin, Scalar xmax)
{
    if (xmin < xmax)
    {
        IntervalSet input(xmin, xmax), output;
        Difference(*this, input, output);
        *this = output;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Scalar>
void IntervalSet<Scalar>::Union (const IntervalSet& input0,
    const IntervalSet& input1, IntervalSet& output)
{
    output.mEndpoints.clear();

    const int numEndpoints0 = (const int)input0.mEndpoints.size();
    const int numEndpoints1 = (const int)input1.mEndpoints.size();
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 0;
    while (i0 < numEndpoints0 && i1 < numEndpoints1)
    {
        Scalar value0 = input0.mEndpoints[i0];
        Scalar value1 = input1.mEndpoints[i1];

        if (value0 < value1)
        {
            if (parity0 == 0)
            {
                parity0 = 1;
                if (parity1 == 0)
                {
                    output.mEndpoints.push_back(value0);
                }
            }
            else
            {
                if (parity1 == 0)
                {
                    output.mEndpoints.push_back(value0);
                }
                parity0 = 0;
            }
            ++i0;
        }
        else if (value1 < value0)
        {
            if (parity1 == 0)
            {
                parity1 = 1;
                if (parity0 == 0)
                {
                    output.mEndpoints.push_back(value1);
                }
            }
            else
            {
                if (parity0 == 0)
                {
                    output.mEndpoints.push_back(value1);
                }
                parity1 = 0;
            }
            ++i1;
        }
        else  // value0 == value1
        {
            if (parity0 == parity1)
            {
                output.mEndpoints.push_back(value0);
            }
            parity0 ^= 1;
            parity1 ^= 1;
            ++i0;
            ++i1;
        }
    }

    while (i0 < numEndpoints0)
    {
        output.mEndpoints.push_back(input0.mEndpoints[i0]);
        ++i0;
    }

    while (i1 < numEndpoints1)
    {
        output.mEndpoints.push_back(input1.mEndpoints[i1]);
        ++i1;
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
void IntervalSet<Scalar>::Intersection (const IntervalSet& input0,
    const IntervalSet& input1, IntervalSet& output)
{
    output.mEndpoints.clear();

    const int numEndpoints0 = (const int)input0.mEndpoints.size();
    const int numEndpoints1 = (const int)input1.mEndpoints.size();
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 0;
    while (i0 < numEndpoints0 && i1 < numEndpoints1)
    {
        Scalar value0 = input0.mEndpoints[i0];
        Scalar value1 = input1.mEndpoints[i1];

        if (value0 < value1)
        {
            if (parity0 == 0)
            {
                parity0 = 1;
                if (parity1 == 1)
                {
                    output.mEndpoints.push_back(value0);
                }
            }
            else
            {
                if (parity1 == 1)
                {
                    output.mEndpoints.push_back(value0);
                }
                parity0 = 0;
            }
            ++i0;
        }
        else if (value1 < value0)
        {
            if (parity1 == 0)
            {
                parity1 = 1;
                if (parity0 == 1)
                {
                    output.mEndpoints.push_back(value1);
                }
            }
            else
            {
                if (parity0 == 1)
                {
                    output.mEndpoints.push_back(value1);
                }
                parity1 = 0;
            }
            ++i1;
        }
        else  // value0 == value1
        {
            if (parity0 == parity1)
            {
                output.mEndpoints.push_back(value0);
            }
            parity0 ^= 1;
            parity1 ^= 1;
            ++i0;
            ++i1;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
void IntervalSet<Scalar>::Difference (const IntervalSet& input0,
    const IntervalSet& input1, IntervalSet& output)
{
    output.mEndpoints.clear();

    const int numEndpoints0 = (const int)input0.mEndpoints.size();
    const int numEndpoints1 = (const int)input1.mEndpoints.size();
    int i0 = 0, i1 = 0, parity0 = 0, parity1 = 1;
    while (i0 < numEndpoints0 && i1 < numEndpoints1)
    {
        Scalar value0 = input0.mEndpoints[i0];
        Scalar value1 = input1.mEndpoints[i1];

        if (value0 < value1)
        {
            if (parity0 == 0)
            {
                parity0 = 1;
                if (parity1 == 1)
                {
                    output.mEndpoints.push_back(value0);
                }
            }
            else
            {
                if (parity1 == 1)
                {
                    output.mEndpoints.push_back(value0);
                }
                parity0 = 0;
            }
            ++i0;
        }
        else if (value1 < value0)
        {
            if (parity1 == 0)
            {
                parity1 = 1;
                if (parity0 == 1)
                {
                    output.mEndpoints.push_back(value1);
                }
            }
            else
            {
                if (parity0 == 1)
                {
                    output.mEndpoints.push_back(value1);
                }
                parity1 = 0;
            }
            ++i1;
        }
        else  // value0 == value1
        {
            if (parity0 == parity1)
            {
                output.mEndpoints.push_back(value0);
            }
            parity0 ^= 1;
            parity1 ^= 1;
            ++i0;
            ++i1;
        }
    }

    while (i0 < numEndpoints0)
    {
        output.mEndpoints.push_back(input0.mEndpoints[i0]);
        ++i0;
    }
}
//----------------------------------------------------------------------------
template <typename Scalar>
void IntervalSet<Scalar>::ExclusiveOr (const IntervalSet& input0,
    const IntervalSet& input1, IntervalSet& output)
{
    output.mEndpoints.clear();

    const int numEndpoints0 = (const int)input0.mEndpoints.size();
    const int numEndpoints1 = (const int)input1.mEndpoints.size();
    int i0 = 0, i1 = 0;
    while (i0 < numEndpoints0 && i1 < numEndpoints1)
    {
        Scalar value0 = input0.mEndpoints[i0];
        Scalar value1 = input1.mEndpoints[i1];

        if (value0 < value1)
        {
            output.mEndpoints.push_back(value0);
            ++i0;
        }
        else if (value1 < value0)
        {
            output.mEndpoints.push_back(value1);
            ++i1;
        }
        else  // value0 == value1
        {
            ++i0;
            ++i1;
        }
    }

    while (i0 < numEndpoints0)
    {
        output.mEndpoints.push_back(input0.mEndpoints[i0]);
        ++i0;
    }

    while (i1 < numEndpoints1)
    {
        output.mEndpoints.push_back(input1.mEndpoints[i1]);
        ++i1;
    }
}
//----------------------------------------------------------------------------
