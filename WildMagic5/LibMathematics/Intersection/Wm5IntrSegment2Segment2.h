// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2012/11/03)

#ifndef WM5INTRSEGMENT2SEGMENT2_H
#define WM5INTRSEGMENT2SEGMENT2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Intersector.h"
#include "Wm5Segment2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IntrSegment2Segment2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Segment2 (const Segment2<Real>& segment0,
        const Segment2<Real>& segment1);

    // Object access.
    const Segment2<Real>& GetSegment0 () const;
    const Segment2<Real>& GetSegment1 () const;

    // Static intersection query.
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  If the segment dos not intersect, GetQuantity()
    // returns 0.  If the segments intersect in a single point, GetQuantity()
    // returns 1, in which case GetPoint() returns the point of intersection
    // and Intersector::GetIntersectionType() returns IT_POINT.  If the
    // segments are collinear and intersect in a segment, GetQuantity()
    // returns INT_MAX and Intersector::GetIntersectionType() returns
    // IT_SEGMENT.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint () const;

    // The intersection testing uses the center-extent form for line segments.
    // If you start with endpoints (Vector2<Real>) and create Segment2<Real>
    // objects, the conversion to center-extent form can contain small
    // numerical round-off errors.  Testing for the intersection of two
    // segments that share an endpoint might lead to a failure due to the
    // round-off errors.  To allow for this, you may specify a small positive
    // threshold that slightly enlarges the intervals for the segments.  The
    // default value is zero.
    void SetIntervalThreshold (Real intervalThreshold);
    Real GetIntervalThreshold () const;

    // The computation for determining whether the segments are parallel
    // might contain small floating-point round-off errors.  The algorithm
    // reduces to testing whether a dot product is zero or nonzero.  You may
    // specify a small positive threshold to adjust the test (dot == 0) to
    // (fabs(dot) > 0).  The default threshold is zero.
    void SetDotThreshold (Real dotThreshold);
    Real GetDotThreshold () const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::mIntersectionType;

    // Determine the relationship between the lines that contain the segments.
    int Classify (Real* s, Vector2<Real>* diff, Vector2<Real>* diffN);

    // The objects to intersect.
    const Segment2<Real>* mSegment0;
    const Segment2<Real>* mSegment1;

    // Information about the intersection set.
    int mQuantity;
    Vector2<Real> mPoint;

    // See the comments before {Set,Get}IntervalThreshold.
    Real mIntervalThreshold;

    // See the comments before {Set,Get}DotThreshold.
    Real mDotThreshold;
};

typedef IntrSegment2Segment2<float> IntrSegment2Segment2f;
typedef IntrSegment2Segment2<double> IntrSegment2Segment2d;

}

#endif
