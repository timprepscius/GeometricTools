// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BspTree2.h"
#include "BspPolygon2.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
BspTree2::BspTree2 (BspPolygon2& polygon, const EArray& edges)
{
    assertion(edges.size() > 0, "Invalid input.\n");

    // Construct splitting line from first edge.
    Vector2d end0 = polygon.mVArray[edges[0].I0];
    Vector2d end1 = polygon.mVArray[edges[0].I1];

    // Add edge to coincident list.
    mCoincident.push_back(edges[0]);

    // Split remaining edges.
    EArray posArray, negArray;
    int imax = (int)edges.size();
    for (int i = 1; i < imax; ++i)
    {
        int v0 = edges[i].I0;
        int v1 = edges[i].I1;
        Vector2d vertex0 = polygon.mVArray[v0];
        Vector2d vertex1 = polygon.mVArray[v1];

        Vector2d intr;
        int vmid;

        switch (Classify(end0, end1, vertex0, vertex1, intr))
        {
            case TRANSVERSE_POSITIVE:
                // modify edge <V0,V1> to <V0,I> and add new edge <I,V1>
                vmid = polygon.InsertVertex(intr);
                polygon.SplitEdge(v0, v1, vmid);
                posArray.push_back(Edge2(vmid, v1));
                negArray.push_back(Edge2(v0, vmid));
                break;
            case TRANSVERSE_NEGATIVE:
                // modify edge <V0,V1> to <V0,I> and add new edge <I,V1>
                vmid = polygon.InsertVertex(intr);
                polygon.SplitEdge(v0, v1, vmid);
                posArray.push_back(Edge2(v0, vmid));
                negArray.push_back(Edge2(vmid, v1));
                break;
            case ALL_POSITIVE:
                posArray.push_back(edges[i]);
                break;
            case ALL_NEGATIVE:
                negArray.push_back(edges[i]);
                break;
            default:  // COINCIDENT
                mCoincident.push_back(edges[i]);
                break;
        }
    }

    if (posArray.size() > 0)
    {
        mPosChild = new0 BspTree2(polygon, posArray);
    }
    else
    {
        mPosChild = 0;
    }

    if (negArray.size() > 0)
    {
        mNegChild = new0 BspTree2(polygon, negArray);
    }
    else
    {
        mNegChild = 0;
    }
}
//----------------------------------------------------------------------------
BspTree2::~BspTree2 ()
{
    delete0(mPosChild);
    delete0(mNegChild);
}
//----------------------------------------------------------------------------
BspTree2* BspTree2::GetCopy () const
{
    BspTree2* tree = new0 BspTree2();

    tree->mCoincident = mCoincident;

    if (mPosChild)
    {
        tree->mPosChild = mPosChild->GetCopy();
    }
    else
    {
        tree->mPosChild = 0;
    }

    if (mNegChild)
    {
        tree->mNegChild = mNegChild->GetCopy();
    }
    else
    {
        tree->mNegChild = 0;
    }

    return tree;
}
//----------------------------------------------------------------------------
void BspTree2::Negate ()
{
    // Reverse coincident edge directions.
    const int numEdges = (int)mCoincident.size();
    for (int i = 0; i < numEdges; ++i)
    {
        Edge2& edge = mCoincident[i];
        int save = edge.I0;
        edge.I0 = edge.I1;
        edge.I1 = save;
    }

    // Swap positive and negative subtrees.
    BspTree2* save = mPosChild;
    mPosChild = mNegChild;
    mNegChild = save;

    if (mPosChild)
    {
        mPosChild->Negate();
    }

    if (mNegChild)
    {
        mNegChild->Negate();
    }
}
//----------------------------------------------------------------------------
int BspTree2::Classify (const Vector2d& end0, const Vector2d& end1,
    const Vector2d& v0, const Vector2d& v1, Vector2d& intr) const
{
    // For numerical round-off error handling.
    const double epsilon0 = 0.00001;
    const double epsilon1 = 0.99999;

    Vector2d dir = end1 - end0;
    Vector2d nor = dir.Perp();
    Vector2d diff0 = v0 - end0;
    Vector2d diff1 = v1 - end0;

    double d0 = nor.Dot(diff0);
    double d1 = nor.Dot(diff1);

    if (d0*d1 < 0.0)
    {
        // Edge <V0,V1> transversely crosses line.  Compute point of
        // intersection I = V0 + t*(V1 - V0).
        double t = d0/(d0 - d1);
        if (t > epsilon0)
        {
            if (t < epsilon1)
            {
                intr = v0 + t*(v1 - v0);
                if (d1 > 0.0)
                {
                    return TRANSVERSE_POSITIVE;
                }
                else
                {
                    return TRANSVERSE_NEGATIVE;
                }
            }
            else
            {
                // T is effectively 1 (numerical round-off issue), so
                // set d1 = 0 and go on to other cases.
                d1 = 0.0;
            }
        }
        else
        {
            // T is effectively 0 (numerical round-off issue), so
            // set d0 = 0 and go on to other cases.
            d0 = 0.0;
        }
    }

    if (d0 > 0.0 || d1 > 0.0)
    {
        // edge on positive side of line
        return ALL_POSITIVE;
    }

    if (d0 < 0.0 || d1 < 0.0)
    {
        // edge on negative side of line
        return ALL_NEGATIVE;
    }

    return COINCIDENT;
}
//----------------------------------------------------------------------------
void BspTree2::GetPosPartition (const BspPolygon2& polygon,
    const Vector2d& v0, const Vector2d& v1, BspPolygon2& pos,
    BspPolygon2& neg, BspPolygon2& coSame, BspPolygon2& coDiff) const
{
    if (mPosChild)
    {
        mPosChild->GetPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
    }
    else
    {
        int i0 = pos.InsertVertex(v0);
        int i1 = pos.InsertVertex(v1);
        pos.InsertEdge(Edge2(i0, i1));
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetNegPartition (const BspPolygon2& polygon,
    const Vector2d& v0, const Vector2d& v1, BspPolygon2& pos,
    BspPolygon2& neg, BspPolygon2& coSame, BspPolygon2& coDiff) const
{
    if (mNegChild)
    {
        mNegChild->GetPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
    }
    else
    {
        int i0 = neg.InsertVertex(v0);
        int i1 = neg.InsertVertex(v1);
        neg.InsertEdge(Edge2(i0, i1));
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetCoPartition (const BspPolygon2& polygon, const Vector2d& v0,
    const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
    BspPolygon2& coSame, BspPolygon2& coDiff) const
{
    const double epsilon = 0.00001;

    // Segment the line containing V0 and V1 by the coincident intervals that
    // intersect <V0,V1>.
    Vector2d dir = v1 - v0;
    double tmax = dir.Dot(dir);

    Vector2d end0, end1;
    double t0, t1;
    bool sameDir;

    std::list<Interval> intervalList;
    std::list<Interval>::iterator iter;

    const int numEdges = (int)mCoincident.size();
    for (int i = 0; i < numEdges; ++i)
    {
        end0 = polygon.mVArray[mCoincident[i].I0];
        end1 = polygon.mVArray[mCoincident[i].I1];

        t0 = dir.Dot(end0 - v0);
        if (Mathd::FAbs(t0) <= epsilon)
        {
            t0 = 0.0;
        }
        else if (Mathd::FAbs(t0 - tmax) <= epsilon)
        {
            t0 = tmax;
        }

        t1 = dir.Dot(end1 - v0);
        if (Mathd::FAbs(t1) <= epsilon)
        {
            t1 = 0.0;
        }
        else if (Mathd::FAbs(t1 - tmax) <= epsilon)
        {
            t1 = tmax;
        }

        sameDir = (t1 > t0);
        if (!sameDir)
        {
            double save = t0;
            t0 = t1;
            t1 = save;
        }

        if (t1 > 0.0 && t0 < tmax)
        {
            if (intervalList.empty())
            {
                intervalList.push_front(Interval(t0, t1, sameDir, true));
            }
            else
            {
                iter = intervalList.begin();
                for (/**/; iter != intervalList.end(); ++iter)
                {
                    if (Mathd::FAbs(t1 - iter->T0) <= epsilon)
                    {
                        t1 = iter->T0;
                    }

                    if (t1 <= iter->T0)
                    {
                        // [t0,t1] is on the left of [I.t0,I.t1]
                        intervalList.insert(iter,
                            Interval(t0, t1, sameDir, true));
                        break;
                    }

                    // Theoretically, the intervals are disjoint or intersect
                    // only at an end point.  The assert makes sure that
                    // [t0,t1] is to the right of [I.t0,I.t1].
                    if (Mathd::FAbs(t0 - iter->T1) <= epsilon)
                    {
                        t0 = iter->T1;
                    }

                    assertion(t0 >= iter->T1, "Invalid ordering.\n");

                    std::list<Interval>::iterator last = intervalList.end();
                    --last;
                    if (iter == last)
                    {
                        intervalList.push_back(Interval(t0, t1, sameDir,
                            true));
                        break;
                    }
                }
            }
        }
    }

    if (intervalList.empty())
    {
        GetPosPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
        GetNegPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
        return;
    }

    // Insert outside intervals between the touching intervals.  It is
    // possible that two touching intervals are adjacent, so this is not just
    // a simple alternation of touching and outside intervals.
    Interval& front = intervalList.front();
    if (front.T0 > 0.0)
    {
        intervalList.push_front(Interval(0.0, front.T0, front.SameDir,
            false));
    }
    else
    {
        front.T0 = 0.0;
    }

    Interval& back = intervalList.back();
    if (back.T1 < tmax)
    {
        intervalList.push_back(Interval(back.T1, tmax, back.SameDir, false));
    }
    else
    {
        back.T1 = tmax;
    }

    std::list<Interval>::iterator iter0 = intervalList.begin();
    std::list<Interval>::iterator iter1 = intervalList.begin();
    for (++iter1; iter1 != intervalList.end(); ++iter0, ++iter1)
    {
        t0 = iter0->T1;
        t1 = iter1->T0;
        if (t1 - t0 > epsilon)
        {
            iter0 = intervalList.insert(iter1, Interval(t0, t1, true, false));
        }
    }

    // Process the segmentation.
    double invTMax = 1.0/tmax;
    t0 = intervalList.front().T0*invTMax;
    end1 = v0 + (intervalList.front().T0*invTMax)*dir;
    iter = intervalList.begin();
    for (/**/; iter != intervalList.end(); ++iter)
    {
        end0 = end1;
        t1 = iter->T1*invTMax;
        end1 = v0 + (iter->T1*invTMax)*dir;

        if (iter->Touching)
        {
            Edge2 edge;
            if (iter->SameDir)
            {
                edge.I0 = coSame.InsertVertex(end0);
                edge.I1 = coSame.InsertVertex(end1);
                if (edge.I0 != edge.I1)
                {
                    coSame.InsertEdge(edge);
                }
            }
            else
            {
                edge.I0 = coDiff.InsertVertex(end1);
                edge.I1 = coDiff.InsertVertex(end0);
                if (edge.I0 != edge.I1)
                {
                    coDiff.InsertEdge(edge);
                }
            }
        }
        else
        {
            GetPosPartition(polygon, end0, end1, pos, neg, coSame, coDiff);
            GetNegPartition(polygon, end0, end1, pos, neg, coSame, coDiff);
        }
    }
}
//----------------------------------------------------------------------------
void BspTree2::GetPartition (const BspPolygon2& polygon, const Vector2d& v0,
    const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
    BspPolygon2& coSame, BspPolygon2& coDiff) const
{
    // Construct splitting line from first coincident edge.
    Vector2d end0 = polygon.mVArray[mCoincident[0].I0];
    Vector2d end1 = polygon.mVArray[mCoincident[0].I1];

    Vector2d intr;

    switch (Classify(end0, end1, v0, v1, intr))
    {
    case TRANSVERSE_POSITIVE:
        GetPosPartition(polygon, intr, v1, pos, neg, coSame, coDiff);
        GetNegPartition(polygon, v0, intr, pos, neg, coSame, coDiff);
        break;
    case TRANSVERSE_NEGATIVE:
        GetPosPartition(polygon, v0, intr, pos, neg, coSame, coDiff);
        GetNegPartition(polygon, intr, v1, pos, neg, coSame, coDiff);
        break;
    case ALL_POSITIVE:
        GetPosPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
        break;
    case ALL_NEGATIVE:
        GetNegPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
        break;
    default:  // COINCIDENT
        GetCoPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
        break;
    }
}
//----------------------------------------------------------------------------
int BspTree2::Classify (const Vector2d& end0, const Vector2d& end1,
    const Vector2d& vertex) const
{
    // For numerical round-off error handling.
    const double epsilon = 0.00001;

    Vector2d dir = end1 - end0;
    Vector2d nor = dir.Perp();
    Vector2d diff = vertex - end0;
    double c = nor.Dot(diff);

    if (c > epsilon)
    {
        return ALL_POSITIVE;
    }

    if (c < -epsilon)
    {
        return ALL_NEGATIVE;
    }

    return COINCIDENT;
}
//----------------------------------------------------------------------------
int BspTree2::CoPointLocation (const BspPolygon2& polygon,
    const Vector2d& vertex) const
{
    // For numerical round-off error handling.
    const double epsilon = 0.00001;

    const int numEdges = (int)mCoincident.size();
    for (int i = 0; i < numEdges; ++i)
    {
        Vector2d end0 = polygon.mVArray[mCoincident[i].I0];
        Vector2d end1 = polygon.mVArray[mCoincident[i].I1];
        Vector2d dir = end1 - end0;
        Vector2d diff = vertex - end0;
        double tmax = dir.Dot(dir);
        double t = dir.Dot(diff);

        if (-epsilon <= t && t <= tmax + epsilon)
        {
            return 0;
        }
    }

    // Does not matter which subtree you use.
    if (mPosChild)
    {
        return mPosChild->PointLocation(polygon, vertex);
    }

    if (mNegChild)
    {
        return mNegChild->PointLocation(polygon, vertex);
    }

    return 0;
}
//----------------------------------------------------------------------------
int BspTree2::PointLocation (const BspPolygon2& polygon,
    const Vector2d& vertex) const
{
    // Construct splitting line from first coincident edge.
    Vector2d end0 = polygon.mVArray[mCoincident[0].I0];
    Vector2d end1 = polygon.mVArray[mCoincident[0].I1];

    switch (Classify(end0, end1, vertex))
    {
    case ALL_POSITIVE:
        if (mPosChild)
        {
            return mPosChild->PointLocation(polygon, vertex);
        }
        else
        {
            return 1;
        }
    case ALL_NEGATIVE:
        if (mNegChild)
        {
            return mNegChild->PointLocation(polygon, vertex);
        }
        else
        {
            return -1;
        }
    default:  // COINCIDENT
        return CoPointLocation(polygon, vertex);
    }
}
//----------------------------------------------------------------------------
void BspTree2::Indent (std::ofstream& outFile, int numSpaces)
{
    for (int i = 0; i < numSpaces; ++i)
    {
        outFile << ' ';
    }
}
//----------------------------------------------------------------------------
void BspTree2::Print (std::ofstream& outFile, int level, char type)
{
    const int numEdges = (int)mCoincident.size();
    for (int i = 0; i < numEdges; ++i)
    {
        Indent(outFile, 4*level);
        outFile << type << " <" << mCoincident[i].I0 << ',' <<
            mCoincident[i].I1 << ">" << std::endl;
    }

    if (mPosChild)
    {
        mPosChild->Print(outFile, level + 1, 'p');
    }

    if (mNegChild)
    {
        mNegChild->Print(outFile, level + 1, 'n');
    }
}
//----------------------------------------------------------------------------
