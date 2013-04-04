// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.2 (2012/07/07)

#include "DistancePointEllipseEllipsoid.h"

WM5_CONSOLE_APPLICATION(DistancePointEllipseEllipsoid);

//----------------------------------------------------------------------------
// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1 with e0 >= e1.  The query point is
// (y0,y1) with y0 >= 0 and y1 >= 0.  The function returns the distance from
// the query point to the ellipse.  It also computes the ellipse point (x0,x1)
// in the first quadrant that is closest to (y0,y1).
//----------------------------------------------------------------------------
template <typename Real>
Real DistancePointEllipseSpecial (const Real e[2], const Real y[2], Real x[2])
{
    Real distance = (Real)0;
    if (y[1] > (Real)0)
    {
        if (y[0] > (Real)0)
        {
            // Bisect to compute the root of F(t) for t >= -e1*e1.
            Real esqr[2] = { e[0]*e[0], e[1]*e[1] };
            Real ey[2] = { e[0]*y[0], e[1]*y[1] };
            Real t0 = -esqr[1] + ey[1];
            Real t1 = -esqr[1] + sqrt(ey[0]*ey[0] + ey[1]*ey[1]);
            Real t = t0;
            const int imax = 2*std::numeric_limits<Real>::max_exponent;
            for (int i = 0; i < imax; ++i)
            {
                t = ((Real)0.5)*(t0 + t1);
                if (t == t0 || t == t1)
                {
                    break;
                }

                Real r[2] = { ey[0]/(t + esqr[0]), ey[1]/(t + esqr[1]) };
                Real f = r[0]*r[0] + r[1]*r[1] - (Real)1;
                if (f > (Real)0)
                {
                    t0 = t;
                }
                else if (f < (Real)0)
                {
                    t1 = t;
                }
                else
                {
                    break;
                }
            }

            x[0] = esqr[0]*y[0]/(t + esqr[0]);
            x[1] = esqr[1]*y[1]/(t + esqr[1]);
            Real d[2] = { x[0] - y[0], x[1] - y[1] };
            distance = sqrt(d[0]*d[0] + d[1]*d[1]);
        }
        else  // y0 == 0
        {
            x[0] = (Real)0;
            x[1] = e[1];
            distance = fabs(y[1] - e[1]);
        }
    }
    else  // y1 == 0
    {
        Real denom0 = e[0]*e[0] - e[1]*e[1];
        Real e0y0 = e[0]*y[0];
        if (e0y0 < denom0)
        {
            // y0 is inside the subinterval.
            Real x0de0 = e0y0/denom0;
            Real x0de0sqr = x0de0*x0de0;
            x[0] = e[0]*x0de0;
            x[1] = e[1]*sqrt(fabs((Real)1 - x0de0sqr));
            Real d0 = x[0] - y[0];
            distance = sqrt(d0*d0 + x[1]*x[1]);
        }
        else
        {
            // y0 is outside the subinterval.  The closest ellipse point has
            // x1 == 0 and is on the domain-boundary interval (x0/e0)^2 = 1.
            x[0] = e[0];
            x[1] = (Real)0;
            distance = fabs(y[0] - e[0]);
        }
    }
    return distance;
}
//----------------------------------------------------------------------------
// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1.  The query point is (y0,y1).
// The function returns the distance from the query point to the ellipse.
// It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
//----------------------------------------------------------------------------
template <typename Real>
Real DistancePointEllipse (const Real e[2], const Real y[2], Real x[2])
{
    // Determine reflections for y to the first quadrant.
    bool reflect[2];
    int i, j;
    for (i = 0; i < 2; ++i)
    {
        reflect[i] = (y[i] < (Real)0);
    }

    // Determine the axis order for decreasing extents.
    int permute[2];
    if (e[0] < e[1])
    {
        permute[0] = 1;  permute[1] = 0;
    }
    else
    {
        permute[0] = 0;  permute[1] = 1;
    }

    int invpermute[2];
    for (i = 0; i < 2; ++i)
    {
        invpermute[permute[i]] = i;
    }

    Real locE[2], locY[2];
    for (i = 0; i < 2; ++i)
    {
        j = permute[i];
        locE[i] = e[j];
        locY[i] = y[j];
        if (reflect[j])
        {
            locY[i] = -locY[i];
        }
    }

    Real locX[2];
    Real distance = DistancePointEllipseSpecial(locE, locY, locX);

    // Restore the axis order and reflections.
    for (i = 0; i < 2; ++i)
    {
        j = invpermute[i];
        if (reflect[j])
        {
            locX[j] = -locX[j];
        }
        x[i] = locX[j];
    }

    return distance;
}
//----------------------------------------------------------------------------
// The ellipsoid is (x0/e0)^2 + (x1/e1)^2 + (x2/e2)^2 = 1 with e0 >= e1 >= e2.
// The query point is (y0,y1,y2) with y0 >= 0, y1 >= 0, and y2 >= 0.  The
// function returns the distance from the query point to the ellipsoid.  It
// also computes the ellipsoid point (x0,x1,x2) in the first octant that is
// closest to (y0,y1,y2).
//----------------------------------------------------------------------------
template <typename Real>
Real DistancePointEllipsoidSpecial (const Real e[3], const Real y[3],
    Real x[3])
{
    Real distance;
    if (y[2] > (Real)0)
    {
        if (y[1] > (Real)0)
        {
            if (y[0] > (Real)0)
            {
                // Bisect to compute the root of F(t) for t >= -e2*e2.
                Real esqr[3] = { e[0]*e[0], e[1]*e[1], e[2]*e[2] };
                Real ey[3] = { e[0]*y[0], e[1]*y[1], e[2]*y[2] };
                Real t0 = -esqr[2] + ey[2];
                Real t1 = -esqr[2] + sqrt(ey[0]*ey[0] + ey[1]*ey[1] +
                    ey[2]*ey[2]);
                Real t = t0;
                const int imax = 2*std::numeric_limits<Real>::max_exponent;
                for (int i = 0; i < imax; ++i)
                {
                    t = ((Real)0.5)*(t0 + t1);
                    if (t == t0 || t == t1)
                    {
                        break;
                    }

                    Real r[3] = { ey[0]/(t + esqr[0]), ey[1]/(t + esqr[1]),
                        ey[2]/(t + esqr[2]) };
                    Real f = r[0]*r[0] + r[1]*r[1] + r[2]*r[2] - (Real)1;
                    if (f > (Real)0)
                    {
                        t0 = t;
                    }
                    else if (f < (Real)0)
                    {
                        t1 = t;
                    }
                    else
                    {
                        break;
                    }
                }

                x[0] = esqr[0]*y[0]/(t + esqr[0]);
                x[1] = esqr[1]*y[1]/(t + esqr[1]);
                x[2] = esqr[2]*y[2]/(t + esqr[2]);
                Real d[3] = { x[0] - y[0], x[1] - y[1], x[2] - y[2] };
                distance = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
            }
            else  // y0 == 0
            {
                x[0] = (Real)0;
                Real etmp[2] = { e[1], e[2] };
                Real ytmp[2] = { y[1], y[2] };
                Real xtmp[2];
                distance = DistancePointEllipseSpecial<Real>(etmp, ytmp, xtmp);
                x[1] = xtmp[0];
                x[2] = xtmp[1];
            }
        }
        else  // y1 == 0
        {
            x[1] = (Real)0;
            if (y[0] > (Real)0)
            {
                Real etmp[2] = { e[0], e[2] };
                Real ytmp[2] = { y[0], y[2] };
                Real xtmp[2];
                distance = DistancePointEllipseSpecial<Real>(etmp, ytmp, xtmp);
                x[0] = xtmp[0];
                x[2] = xtmp[1];
            }
            else  // y0 == 0
            {
                x[0] = (Real)0;
                x[2] = e[2];
                distance = fabs(y[2] - e[2]);
            }
        }
    }
    else  // y2 == 0
    {
        Real denom[2] = { e[0]*e[0] - e[2]*e[2], e[1]*e[1] - e[2]*e[2] };
        Real ey[2] = { e[0]*y[0], e[1]*y[1] };
        if (ey[0] < denom[0] && ey[1] < denom[1])
        {
            // (y0,y1) is inside the axis-aligned bounding rectangle of the
            // subellipse.  This intermediate test is designed to guard
            // against the division by zero when e0 == e2 or e1 == e2.
            Real xde[2] = { ey[0]/denom[0], ey[1]/denom[1] };
            Real xdesqr[2] = { xde[0]*xde[0], xde[1]*xde[1] };
            Real discr = (Real)1 - xdesqr[0] - xdesqr[1];
            if (discr > (Real)0)
            {
                // (y0,y1) is inside the subellipse.  The closest ellipsoid
                // point has x2 > 0.
                x[0] = e[0]*xde[0];
                x[1] = e[1]*xde[1];
                x[2] = e[2]*sqrt(discr);
                Real d[2] = { x[0] - y[0], x[1] - y[1] };
                distance = sqrt(d[0]*d[0] + d[1]*d[1] + x[2]*x[2]);
            }
            else
            {
                // (y0,y1) is outside the subellipse.  The closest ellipsoid
                // point has x2 == 0 and is on the domain-boundary ellipse
                // (x0/e0)^2 + (x1/e1)^2 = 1.
                x[2] = (Real)0;
                distance = DistancePointEllipseSpecial<Real>(e, y, x);
            }
        }
        else
        {
            // (y0,y1) is outside the subellipse.  The closest ellipsoid
            // point has x2 == 0 and is on the domain-boundary ellipse
            // (x0/e0)^2 + (x1/e1)^2 = 1.
            x[2] = (Real)0;
            distance = DistancePointEllipseSpecial<Real>(e, y, x);
        }
    }
    return distance;
}
//----------------------------------------------------------------------------
// The ellipsoid is (x0/e0)^2 + (x1/e1)^2 + (x2/e2)^2 = 1.  The query point is
// (y0,y1,y2).  The function returns the distance from the query point to the
// ellipsoid.   It also computes the ellipsoid point (x0,x1,x2) that is
// closest to (y0,y1,y2).
//----------------------------------------------------------------------------
template <typename Real>
Real DistancePointEllipsoid (const Real e[3], const Real y[3], Real x[3])
{
    // Determine reflections for y to the first octant.
    bool reflect[3];
    int i, j;
    for (i = 0; i < 3; ++i)
    {
        reflect[i] = (y[i] < (Real)0);
    }

    // Determine the axis order for decreasing extents.
    int permute[3];
    if (e[0] < e[1])
    {
        if (e[2] < e[0])
        {
            permute[0] = 1;  permute[1] = 0;  permute[2] = 2;
        }
        else if (e[2] < e[1])
        {
            permute[0] = 1;  permute[1] = 2;  permute[2] = 0;
        }
        else
        {
            permute[0] = 2;  permute[1] = 1;  permute[2] = 0;
        }
    }
    else
    {
        if (e[2] < e[1])
        {
            permute[0] = 0;  permute[1] = 1;  permute[2] = 2;
        }
        else if (e[2] < e[0])
        {
            permute[0] = 0;  permute[1] = 2;  permute[2] = 1;
        }
        else
        {
            permute[0] = 2;  permute[1] = 0;  permute[2] = 1;
        }
    }

    int invpermute[3];
    for (i = 0; i < 3; ++i)
    {
        invpermute[permute[i]] = i;
    }

    Real locE[3], locY[3];
    for (i = 0; i < 3; ++i)
    {
        j = permute[i];
        locE[i] = e[j];
        locY[i] = y[j];
        if (reflect[j])
        {
            locY[i] = -locY[i];
        }
    }

    Real locX[3];
    Real distance = DistancePointEllipsoidSpecial(locE, locY, locX);

    // Restore the axis order and reflections.
    for (i = 0; i < 3; ++i)
    {
        j = invpermute[i];
        if (reflect[j])
        {
            locX[j] = -locX[j];
        }
        x[i] = locX[j];
    }

    return distance;
}
//----------------------------------------------------------------------------
// Bisect for the root of
//   F(t) = sum_{j=0}{m-1} (e[i[j]]*y[i[j]]/(t + e[i[j]]*e[i[j]])
// for t >= -e[i[m-1]]*e[i[m-1]].  The incoming e[] and y[] values are those
// for which the query point components are positive.
//----------------------------------------------------------------------------
template <int N, typename Real>
Real DPHSBisector (int numComponents, const Real e[N], const Real y[N],
    Real x[N])
{
    Real esqr[N], ey[N], argument = (Real)0;
    int i;
    for (i = 0; i < numComponents; ++i)
    {
        esqr[i] = e[i]*e[i];
        ey[i] = e[i]*y[i];
        argument += ey[i]*ey[i];
    }

    Real t0 = -esqr[numComponents-1] + ey[numComponents-1];
    Real t1 = -esqr[numComponents-1] + sqrt(argument);
    Real t = t0;
    const int jmax = 2*std::numeric_limits<Real>::max_exponent;
    for (int j = 0; j < jmax; ++j)
    {
        t = ((Real)0.5)*(t0 + t1);
        if (t == t0 || t == t1)
        {
            break;
        }

        Real f = (Real)-1;
        for (i = 0; i < numComponents; ++i)
        {
            Real r = ey[i]/(t + esqr[i]);
            f += r*r;
        }
        if (f > (Real)0)
        {
            t0 = t;
        }
        else if (f < (Real)0)
        {
            t1 = t;
        }
        else
        {
            break;
        }
    }

    Real distance = (Real)0;
    for (i = 0; i < numComponents; ++i)
    {
        x[i] = esqr[i]*y[i]/(t + esqr[i]);
        Real diff = x[i] - y[i];
        distance += diff*diff;
    }
    distance = sqrt(distance);
    return distance;
}
//----------------------------------------------------------------------------
// The hyperellipsoid is sum_{i=0}^{N-1}(x[i]/e[i])^2 = 1 with e[i+1] >= e[i]
// for all i.  The query point is (y[0],...,y[N-1]) with y[i] >= 0 for all i.
// The function returns the distance from the query point to the
// hyperellipsoid.  It also computes the hyperellipsoid point
// (x[0],...,x[N-1]) closest to (y[0],...,y[N-1]), with x[i] >= 0 for all i.
//----------------------------------------------------------------------------
template <int N, typename Real>
Real DistancePointHyperellipsoidSpecial (const Real e[N], const Real y[N],
    Real x[N])
{
    Real distance = (Real)0;

    Real ePos[N], yPos[N], xPos[N];
    int numPos = 0;
    int i;
    for (i = 0; i < N; ++i)
    {
        if (y[i] > (Real)0)
        {
            ePos[numPos] = e[i];
            yPos[numPos] = y[i];
            ++numPos;
        }
        else
        {
            x[i] = (Real)0;
        }
    }

    if (y[N-1] > (Real)0)
    {
        distance = DPHSBisector<N,Real>(numPos, ePos, yPos, xPos);
    }
    else  // y[N-1] = 0
    {
        Real eNm1Sqr = e[N-1]*e[N-1];
        Real denom[N-1], ey[N-1];
        for (i = 0; i < numPos; ++i)
        {
            denom[i] = ePos[i]*ePos[i] - eNm1Sqr;
            ey[i] = ePos[i]*yPos[i];
        }

        bool inSubHyperbox = true;
        for (i = 0; i < numPos; ++i)
        {
            if (ey[i] >= denom[i])
            {
                inSubHyperbox = false;
                break;
            }
        }

        bool inSubHyperellipsoid = false;
        if (inSubHyperbox)
        {
            // yPos[] is inside the axis-aligned bounding box of the
            // subhyperellipsoid.  This intermediate test is designed to guard
            // against the division by zero when ePos[i] == e[N-1] for some i.
            Real xde[N-1], discr = (Real)1;
            for (i = 0; i < numPos; ++i)
            {
                xde[i] = ey[i]/denom[i];
                discr -= xde[i]*xde[i];
            }
            if (discr > (Real)0)
            {
                // yPos[] is inside the subhyperellipsoid.  The closest
                // hyperellipsoid point has x[N-1] > 0.
                distance = (Real)0;
                for (i = 0; i < numPos; ++i)
                {
                    xPos[i] = ePos[i]*xde[i];
                    Real diff = xPos[i] - yPos[i];
                    distance += diff*diff;
                }
                x[N-1] = e[N-1]*sqrt(discr);
                distance += x[N-1]*x[N-1];
                distance = sqrt(distance);
                inSubHyperellipsoid = true;
            }
        }

        if (!inSubHyperellipsoid)
        {
            // yPos[] is outside the subhyperellipsoid.  The closest
            // hyperellipsoid point has x[N-1] == 0 and is on the
            // domain-boundary hyperellipsoid.
            x[N-1] = (Real)0;
            distance = DPHSBisector<N,Real>(numPos, ePos, yPos, xPos);
        }
    }

    // Fill in those x[] values that were not zeroed out initially.
    for (i = 0, numPos = 0; i < N; ++i)
    {
        if (y[i] > (Real)0)
        {
            x[i] = xPos[numPos];
            ++numPos;
        }
    }

    return distance;
}
//----------------------------------------------------------------------------
// The hyperellipsoid is sum_{i=0}^{N-1}(x[i]/e[i])^2 = 1.  The query point is
// (y[0],...,y[N-1]).  The function returns the distance from the query point
// to the hyperellipsoid.   It also computes the hyperellipsoid point
// (x[0],...,x[N-1]) that is closest to (y[0],...,y[N-1]).
//----------------------------------------------------------------------------
template <int N, typename Real>
Real DistancePointHyperellipsoid (const Real e[N], const Real y[N], Real x[N])
{
    // Determine reflections for y to the first octant.
    bool reflect[N];
    int i, j;
    for (i = 0; i < N; ++i)
    {
        reflect[i] = (y[i] < (Real)0);
    }

    // Determine the axis order for decreasing extents.
    std::vector<std::pair<Real,int> > permute(N);
    for (i = 0; i < N; ++i)
    {
        permute[i].first = -e[i];
        permute[i].second = i;
    }
    std::sort(permute.begin(), permute.end());

    int invpermute[N];
    for (i = 0; i < N; ++i)
    {
        invpermute[permute[i].second] = i;
    }

    Real locE[N], locY[N];
    for (i = 0; i < N; ++i)
    {
        j = permute[i].second;
        locE[i] = e[j];
        locY[i] = y[j];
        if (reflect[j])
        {
            locY[i] = -locY[i];
        }
    }

    Real locX[N];
    Real distance =
        DistancePointHyperellipsoidSpecial<N,Real>(locE, locY, locX);

    // Restore the axis order and reflections.
    for (i = 0; i < N; ++i)
    {
        j = invpermute[i];
        if (reflect[i])
        {
            locX[j] = -locX[j];
        }
        x[i] = locX[j];
    }

    return distance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
DistancePointEllipseEllipsoid::DistancePointEllipseEllipsoid ()
    :
    ConsoleApplication("SampleMathematics/DistancePointEllipseEllipsoid")
{
}
//----------------------------------------------------------------------------
DistancePointEllipseEllipsoid::~DistancePointEllipseEllipsoid ()
{
}
//----------------------------------------------------------------------------
void DistancePointEllipseEllipsoid::GenerateDistance2 (
    ImageDouble2D& distance)
{
    const int bound0 = distance.GetBound(0);
    const int bound1 = distance.GetBound(1);
    const double e[2] =
    {
        (double)bound0/4,
        (double)bound1/4
    };

    double y[2], x[2];
    for (int i1 = -bound1/2, j1 = 0; i1 < bound1/2; ++i1, ++j1)
    {
        y[1] = 4.0*e[1]*i1/bound1;
        for (int i0 = -bound0/2, j0 = 0; i0 < bound0/2; ++i0, ++j0)
        {
            y[0] = 4.0*e[0]*i0/bound0;
            distance(j0, j1) = DistancePointEllipse(e, y, x);
        }
    }
}
//----------------------------------------------------------------------------
void DistancePointEllipseEllipsoid::GenerateDistance3 (
    ImageDouble3D& distance)
{
    const int bound0 = distance.GetBound(0);
    const int bound1 = distance.GetBound(1);
    const int bound2 = distance.GetBound(2);
    const double e[3] =
    {
        (double)bound0/4,
        (double)bound1/4,
        (double)bound2/4
    };

    double y[3], x[3];
    for (int i2 = -bound2/2, j2 = 0; i2 < bound2/2; ++i2, ++j2)
    {
        y[2] = 4.0*e[2]*i2/bound2;
        for (int i1 = -bound1/2, j1 = 0; i1 < bound1/2; ++i1, ++j1)
        {
            y[1] = 4.0*e[1]*i1/bound1;
            for (int i0 = -bound0/2, j0 = 0; i0 < bound0/2; ++i0, ++j0)
            {
                y[0] = 4.0*e[0]*i0/bound0;
                distance(j0, j1, j2) = DistancePointEllipsoid(e, y, x);
            }
        }
    }
}
//----------------------------------------------------------------------------
void DistancePointEllipseEllipsoid::HyperGenerateDistance2 (
    ImageDouble2D& distance)
{
    const int bound0 = distance.GetBound(0);
    const int bound1 = distance.GetBound(1);
    const double e[2] =
    {
        (double)bound0/4,
        (double)bound1/4
    };

    double y[2], x[2];

    for (int i1 = -bound1/2, j1 = 0; i1 < bound1/2; ++i1, ++j1)
    {
        y[1] = 4.0*e[1]*i1/bound1;
        for (int i0 = -bound0/2, j0 = 0; i0 < bound0/2; ++i0, ++j0)
        {
            y[0] = 4.0*e[0]*i0/bound0;
            distance(j0, j1) =
                DistancePointHyperellipsoid<2,double>(e, y, x);
        }
    }
}
//----------------------------------------------------------------------------
void DistancePointEllipseEllipsoid::HyperGenerateDistance3 (
    ImageDouble3D& distance)
{
    const int bound0 = distance.GetBound(0);
    const int bound1 = distance.GetBound(1);
    const int bound2 = distance.GetBound(2);
    const double e[3] =
    {
        (double)bound0/4,
        (double)bound1/4,
        (double)bound2/4
    };

    double y[3], x[3];

    for (int i2 = -bound2/2, j2 = 0; i2 < bound2/2; ++i2, ++j2)
    {
        y[2] = 4.0*e[2]*i2/bound2;
        for (int i1 = -bound1/2, j1 = 0; i1 < bound1/2; ++i1, ++j1)
        {
            y[1] = 4.0*e[1]*i1/bound1;
            for (int i0 = -bound0/2, j0 = 0; i0 < bound0/2; ++i0, ++j0)
            {
                y[0] = 4.0*e[0]*i0/bound0;
                distance(j0, j1, j2) =
                    DistancePointHyperellipsoid<3,double>(e, y, x);
            }
        }
    }
}
//----------------------------------------------------------------------------
int DistancePointEllipseEllipsoid::Main (int, char**)
{
    std::string output = ThePath + "output.txt";
    FILE* outFile = fopen(output.c_str(), "wt");

    char filename[256];
    double maxError;
    int i, j;

    // The specialized code for 2D is compared with the general code for a
    // hyperellipsoid with N = 2.
    const int numTests2D = 3;
    const int xBound2D[numTests2D] = { 512, 256, 256 };
    const int yBound2D[numTests2D] = { 256, 512, 256 };
    for (i = 0; i < numTests2D; ++i)
    {
        ImageDouble2D distance(xBound2D[i], yBound2D[i]);
        GenerateDistance2(distance);
        sprintf(filename, "distance_%d_%d.im", xBound2D[i], yBound2D[i]);
        distance.Save(filename);

        ImageDouble2D hdistance(xBound2D[i], yBound2D[i]);
        HyperGenerateDistance2(hdistance);
        sprintf(filename, "hdistance_%d_%d.im", xBound2D[i], yBound2D[i]);
        hdistance.Save(filename);

        maxError = 0.0;
        for (j = 0; j < distance.GetQuantity(); ++j)
        {
            double error = fabs(distance[j] - hdistance[j]);
            if (error > maxError)
            {
                maxError = error;
            }
        }

        fprintf(outFile, "2D: %3d %3d   %lg\n", xBound2D[i], yBound2D[i],
            maxError);
    }
    fprintf(outFile, "\n");

    // The specialized code for 3D is compared with the general code for a
    // hyperellipsoid with N = 3.
    const int numTests3D = 5;
    const int xBound3D[numTests3D] = { 128, 128, 128,  64,  32 };
    const int yBound3D[numTests3D] = {  64, 128,  64,  64,  64 };
    const int zBound3D[numTests3D] = {  32,  32,  64,  64, 128 };
    for (i = 0; i < numTests3D; ++i)
    {
        ImageDouble3D distance(xBound3D[i], yBound3D[i], zBound3D[i]);
        GenerateDistance3(distance);
        sprintf(filename, "distance_%d_%d_%d.im", xBound3D[i], yBound3D[i],
            zBound3D[i]);
        distance.Save(filename);

        ImageDouble3D hdistance(xBound3D[i], yBound3D[i], zBound3D[i]);
        HyperGenerateDistance3(hdistance);
        sprintf(filename, "hdistance_%d_%d_%d.im", xBound3D[i], yBound3D[i],
            zBound3D[i]);
        hdistance.Save(filename);

        maxError = 0.0;
        for (j = 0; j < distance.GetQuantity(); ++j)
        {
            double error = fabs(distance[j] - hdistance[j]);
            if (error > maxError)
            {
                maxError = error;
            }
        }

        fprintf(outFile, "3D: %3d %3d %3d   %lg\n", xBound3D[i], yBound3D[i],
            zBound3D[i], maxError);
    }

    fclose(outFile);
    return 0;
}
//----------------------------------------------------------------------------
