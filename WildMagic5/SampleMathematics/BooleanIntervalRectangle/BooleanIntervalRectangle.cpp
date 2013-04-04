// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "BooleanIntervalRectangle.h"
#include "RectangleSet.h"

WM5_CONSOLE_APPLICATION(BooleanIntervalRectangle);

//----------------------------------------------------------------------------
BooleanIntervalRectangle::BooleanIntervalRectangle ()
    :
    ConsoleApplication("SampleMathematics/BooleanIntervalRectangle")
{
}
//----------------------------------------------------------------------------
BooleanIntervalRectangle::~BooleanIntervalRectangle ()
{
}
//----------------------------------------------------------------------------
int BooleanIntervalRectangle::Main (int, char**)
{
    IntervalSet<float> S1, S2;
    S1.Insert(1.0f, 3.0f);
    S1.Insert(4.0f, 9.0f);
    S1.Insert(10.0f, 12.0f);
    S1.Insert(16.0f, 17.0f);
    S2.Insert(0.0f, 2.0f);
    S2.Insert(6.0f, 7.0f);
    S2.Insert(8.0f, 11.0f);
    S2.Insert(13.0f, 14.0f);
    S2.Insert(15.0f, 18.0f);
    IntervalSet<float> S1uS2, S1iS2, S1dS2, S1xS2;
    IntervalSet<float>::Union(S1, S2, S1uS2);
    IntervalSet<float>::Intersection(S1, S2, S1iS2);
    IntervalSet<float>::Difference(S1, S2, S1dS2);
    IntervalSet<float>::ExclusiveOr(S1, S2, S1xS2);

    RectangleSet<float> R1, R2;
    R1.Insert(0.0f, 2.0f, 0.0f, 2.0f);
    R2.Insert(1.0f, 3.0f, 1.0f, 3.0f);
    RectangleSet<float> R1uR2, R1iR2, R1dR2, R1xR2;
    RectangleSet<float>::Union(R1,R2, R1uR2);
    RectangleSet<float>::Intersection(R1, R2, R1iR2);
    RectangleSet<float>::Difference(R1, R2, R1dR2);
    RectangleSet<float>::ExclusiveOr(R1, R2, R1xR2);
    return 0;
}
//----------------------------------------------------------------------------
