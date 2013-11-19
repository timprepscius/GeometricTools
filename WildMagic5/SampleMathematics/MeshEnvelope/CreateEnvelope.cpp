// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "CreateEnvelope.h"
#include "Endpoint.h"
#include "SegmentGraph.h"
#include "Wm5Memory.h"

const RScalar CreateEnvelope::ZERO(0);
const RScalar CreateEnvelope::ONE(1);

//----------------------------------------------------------------------------
CreateEnvelope::CreateEnvelope (int numVertices, const Vector2f* vertices,
    int numIndices, const int* indices, int& numEnvelopeVertices,
    Vector2f*& envelopeVertices)
{
    // The graph of vertices and edgeMaps to be used for constructing the
    // obstacle envelope.
    SegmentGraph* graph = new0 SegmentGraph();

    // Convert the vertices to rational points to allow exact arithmetic,
    // thereby avoiding problems with numerical round-off errors.
    RPoint2* ratVertices = new1<RPoint2>(numVertices);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        ratVertices[i].X() = RScalar(vertices[i].X());
        ratVertices[i].Y() = RScalar(vertices[i].Y());
    }

    // Insert the 2D mesh edgeMaps into the graph.
    const int* currentIndex = indices;
    int numTriangles = numIndices/3;
    for (i = 0; i < numTriangles; ++i)
    {
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;
        graph->InsertEdge(ratVertices[v0], ratVertices[v1]);
        graph->InsertEdge(ratVertices[v1], ratVertices[v2]);
        graph->InsertEdge(ratVertices[v2], ratVertices[v0]);
    }
    delete1(ratVertices);

    // Represent each edge as a map of points ordered by rational parameter
    // values, each point P(t) = End0 + t*(End1-End0), where End0 and End1
    // are the rational endpoints of the edge and t is the rational
    // parameter for the edge point P(t).
    std::set<SegmentGraph::Edge>& edgeSet = graph->GetEdges();
    int numEdges = (int)edgeSet.size();
    EdgeMap** edgeMaps = new1<EdgeMap*>(numEdges);
    std::set<SegmentGraph::Edge>::iterator esIter = edgeSet.begin();
    for (i = 0; i < numEdges; ++i, ++esIter)
    {
        SegmentGraph::Edge edge = *esIter;
        EdgeMap* edgeMap = new0 EdgeMap();
        (*edgeMap)[0] = edge.GetVertex(0)->Position;
        (*edgeMap)[1] = edge.GetVertex(1)->Position;
        edgeMaps[i] = edgeMap;
    }

    UpdateAllEdges(numEdges, edgeMaps);

    // Recreate the graph, now using the segmented edgeMaps from the original
    // graph.
    delete0(graph);
    graph = new0 SegmentGraph();
    for (i = 0; i < numEdges; ++i)
    {
        // Each graph edge is a pair of consecutive edge points.
        EdgeMap* edgeMap = edgeMaps[i];

        // Get first point.
        EdgeMap::iterator iter = edgeMap->begin();
        EdgeMap::iterator end = edgeMap->end();
        RPoint2* point0 = &iter->second;

        // Get remaining points.
        for (++iter; iter != end; ++iter)
        {
            RPoint2* point1 = &iter->second;
            graph->InsertEdge(*point0, *point1);
            point0 = point1;
        }

        delete0(edgeMap);
    }
    delete1(edgeMaps);

    std::vector<RPoint2> envelope;
    graph->ExtractEnvelope(envelope);

    // Convert the vertices back to floating-point values and return to the
    // caller.
    numEnvelopeVertices = (int)envelope.size();
    envelopeVertices = new1<Vector2f>(numEnvelopeVertices);
    for (i = 0; i < numEnvelopeVertices; ++i)
    {
        const RPoint2& point = envelope[i];
        point.X().ConvertTo(envelopeVertices[i].X());
        point.Y().ConvertTo(envelopeVertices[i].Y());
    }

    delete0(graph);
}
//----------------------------------------------------------------------------
void CreateEnvelope::InsertParameter (EdgeMap& edgeMap, const RScalar& t)
{
    if (ZERO < t && t < ONE)
    {
        const RPoint2& point0 = edgeMap[ZERO];
        const RPoint2& point1 = edgeMap[ONE];
        edgeMap[t] = point0 + (point1 - point0)*t;
    }
}
//----------------------------------------------------------------------------
void CreateEnvelope::UpdateEdges (EdgeMap& edgeMap0, EdgeMap& edgeMap1)
{
    const RPoint2& U0 = edgeMap0[0];
    const RPoint2& U1 = edgeMap0[1];
    const RPoint2& V0 = edgeMap1[0];
    const RPoint2& V1 = edgeMap1[1];

    // The segments have a common point when U0+s*(U1-U0) = V0+t*(V1-V0) for
    // some values s and t with 0 <= s <= 1 and 0 <= t <= 1.  Rearranging
    // the terms and applying some algebra,
    //   s*(U1-U0) - t*(V1-V0) = V0-U0
    //   s*DotPerp(U1-U0,V1-V0) = DotPerp(V0-U0,V1-V0)
    //   t*DotPerp(U1-U0,V1-V0) = DotPerp(V0-U0,U1-U0)
    // where I used the identities DotPerp(U,V) = -DotPerp(V,U) and
    // DotPerp(W,W) = 0.  Using the notation in the other PfEdgesIntersect
    // comments, we have s*C = B and t*C = A.  As long as C is not zero, the
    // parameters are s = B/C and t = A/C.  These correspond to the
    // intersection of two lines.   The segments intersect at these values
    // as long as 0 <= A/C <= 1 and 0 <= B/C <= 1

    RPoint2 U1mU0 = U1 - U0;
    RPoint2 V1mV0 = V1 - V0;
    RPoint2 V0mU0 = V0 - U0;
    RScalar A = V0mU0.DotPerp(U1mU0);
    RScalar B = V0mU0.DotPerp(V1mV0);
    RScalar C = U1mU0.DotPerp(V1mV0);

    if (A*(A - C) <= ZERO && B*(B - C) <= ZERO)
    {
        if (C != ZERO)
        {
            RScalar invC = ONE/C;
            RScalar tU = B*invC;
            RScalar tV = A*invC;
            InsertParameter(edgeMap0, tU);
            InsertParameter(edgeMap1, tV);
            return;
        }

        // C = 0, so the two inequalities above force A = 0 and B = 0, in
        // which case the segments are collinear.
        RPoint2 V1mU0 = V1 - U0;
        RScalar numer0 = V0mU0.Dot(U1mU0);
        RScalar numer1 = V1mU0.Dot(U1mU0);
        RScalar invDenom = ONE/U1mU0.Dot(U1mU0);
        RScalar tmin, tmax;

        C = U1mU0.Dot(V1mV0);
        bool bCPositive = (C > ZERO);
        if (bCPositive)
        {
            // U1-U0 and V1-V0 in same direction.
            tmin = numer0*invDenom;
            tmax = numer1*invDenom;
        }
        else
        {
            // U1-U0 and V1-V0 in opposite directions.
            tmax = numer0*invDenom;
            tmin = numer1*invDenom;
        }

        // Segment <U0,U1> maps to parameters [0,1] and segment <V0,V1> maps
        // to parameters [tmin,tmax].  Edge updates only occur if the
        // intervals overlap at more than a single point.
        if (ZERO < tmax && ONE > tmin)
        {
            RPoint2 U1mV0;
            RScalar denom, s;

            if (ZERO < tmin)
            {
                if (ONE > tmax)
                {
                    // Intersection is [tmin,tmax], <V0,V1> in <U0,U1>.
                    InsertParameter(edgeMap0, tmin);
                    InsertParameter(edgeMap0, tmax);
                }
                else
                {
                    // Intersection is [tmin,1], U1 in <V0,V1>.
                    InsertParameter(edgeMap0,tmin);
                    U1mV0 = U1 - V0;
                    numer1 = U1mV0.Dot(V1mV0);
                    denom = V1mV0.Dot(V1mV0);
                    s = numer1/denom;
                    InsertParameter(edgeMap1, s);
                }
            }
            else
            {
                if (ONE > tmax)
                {
                    // Intersection is [0,tmax], U0 in <V0,V1>.
                    InsertParameter(edgeMap0,tmax);
                    numer0 = -V0mU0.Dot(V1mV0);
                    denom = V1mV0.Dot(V1mV0);
                    s = numer0/denom;
                    InsertParameter(edgeMap1, s);
                }
                else
                {
                    // Intersection is [0,1], <U0,U1> in <V0,V1>.
                    numer0 = -V0mU0.Dot(V1mV0);
                    invDenom = ONE/V1mV0.Dot(V1mV0);
                    s = numer0*invDenom;
                    InsertParameter(edgeMap1, s);
                    U1mV0 = U1 - V0;
                    numer1 = U1mV0.Dot(V1mV0);
                    s = numer1*invDenom;
                    InsertParameter(edgeMap1, s);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void CreateEnvelope::UpdateAllEdges (int numEdges, EdgeMap** edgeMaps)
{
    // Construct the axis-aligned bounding boxes of the edgeMaps.
    RPoint2* rmin = new1<RPoint2>(numEdges);
    RPoint2* rmax = new1<RPoint2>(numEdges);
    int i;
    for (i = 0; i < numEdges; ++i)
    {
        EdgeMap& edgeMap = *edgeMaps[i];

        RPoint2& end0 = edgeMap[ZERO];
        RPoint2& end1 = edgeMap[ONE];

        if (end0.X() <= end1.X())
        {
            rmin[i].X() = end0.X();
            rmax[i].X() = end1.X();
        }
        else
        {
            rmin[i].X() = end1.X();
            rmax[i].X() = end0.X();
        }

        if (end0.Y() <= end1.Y())
        {
            rmin[i].Y() = end0.Y();
            rmax[i].Y() = end1.Y();
        }
        else
        {
            rmin[i].Y() = end1.Y();
            rmax[i].Y() = end0.Y();
        }
    }

    // Store the x-extremes for the AABBs in a data structure to be sorted.
    // The "Type" field indicates whether the x-value is the minimum (Type
    // is 0) or maximum (Type is 1).  The "Index" field stores the edge
    // index for use as a lookup in the overlap tests.
    int numEndpoints = 2*numEdges;
    std::vector<Endpoint> xEndpoints(numEndpoints);
    int j;
    for (i = 0, j = 0; i < numEdges; ++i)
    {
        xEndpoints[j].Type = 0;
        xEndpoints[j].Value = rmin[i].X();
        xEndpoints[j].Index = i;
        ++j;

        xEndpoints[j].Type = 1;
        xEndpoints[j].Value = rmax[i].X();
        xEndpoints[j].Index = i;
        ++j;
    }

    // Sort the x-values.
    std::sort(xEndpoints.begin(), xEndpoints.end());

    // The active set of rectangles (stored by index in array).
    std::set<int> active;

    // The set of overlapping rectangles (stored by index pairs in array).
    std::set<std::pair<int,int> > overlap;

    // Sweep through the endpoints to determine overlapping x-intervals.
    for (i = 0; i < numEndpoints; ++i)
    {
        Endpoint& end = xEndpoints[i];
        int index = end.Index;
        if (end.Type == 0)  // an interval 'begin' value
        {
            // The current AABB overlaps in the x-direction with all the
            // active intervals.  Now check for y-overlap.
            std::set<int>::iterator iter = active.begin();
            for (/**/; iter != active.end(); ++iter)
            {
                // Rectangles iAIndex and index overlap in the x-dimension.
                // Test for overlap in the y-dimension.
                int activeIndex = *iter;
                if (rmax[activeIndex].Y() >= rmin[activeIndex].Y()
                &&  rmin[activeIndex].Y() <= rmax[activeIndex].Y())
                {
                    // If the edgeMaps share an endpoint, there is no need to
                    // test later for overlap.
                    EdgeMap& edgeMap0 = *edgeMaps[index];
                    EdgeMap& edgeMap1 = *edgeMaps[activeIndex];
                    RPoint2& E0P0 = edgeMap0[ZERO];
                    RPoint2& E0P1 = edgeMap0[ONE];
                    RPoint2& E1P0 = edgeMap1[ZERO];
                    RPoint2& E1P1 = edgeMap1[ONE];
                    if (E0P0 == E1P0 || E0P0 == E1P1
                    ||  E0P1 == E1P0 || E0P1 == E1P1)
                    {
                        continue;
                    }

                    overlap.insert(std::make_pair(activeIndex, index));
                }
            }
            active.insert(index);
        }
        else  // an interval 'end' value
        {
            active.erase(index);
        }
    }

    // Search for edge-edge intersections by comparing only those edgeMaps whose
    // AABBs overlap.
    std::set<std::pair<int,int> >::const_iterator iter = overlap.begin();
    std::set<std::pair<int,int> >::const_iterator end = overlap.end();
    for (/**/; iter != end; ++iter)
    {
        int i0 = iter->first;
        int i1 = iter->second;
        EdgeMap& edgeMap0 = *edgeMaps[i0];
        EdgeMap& edgeMap1 = *edgeMaps[i1];
        UpdateEdges(edgeMap0, edgeMap1);
    }

    delete1(rmax);
    delete1(rmin);
}
//----------------------------------------------------------------------------
