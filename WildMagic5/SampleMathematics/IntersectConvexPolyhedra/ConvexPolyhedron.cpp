// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ConvexPolyhedron.h"
#include "ConvexClipper.h"

//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>::ConvexPolyhedron ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>::ConvexPolyhedron (const V3Array& points,
    const IArray& indices)
{
    Create(points, indices);
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>::ConvexPolyhedron (const V3Array& points,
    const IArray& indices, const PArray& planes)
{
    Create(points, indices, planes);
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>::ConvexPolyhedron (const ConvexPolyhedron& polyhedron)
    :
    MTMesh(polyhedron),
    mPoints(polyhedron.mPoints),
    mPlanes(polyhedron.mPlanes)
{
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::Create (const V3Array& points,
    const IArray& indices)
{
    assertion(points.size() >= 4 && indices.size() >= 4,
        "Polyhedron must be at least a tetrahedron.\n");

    const int numVertices = (int)points.size();
    const int numTriangles = (int)indices.size()/3;
    const int numEdges = numVertices + numTriangles - 2;
    Reset(numVertices, numEdges, numTriangles);
    mPoints = points;

    // Copy polyhedron points into vertex array.  Compute centroid for use in
    // making sure the triangles are counterclockwise oriented when viewed
    // from the outside.
    ComputeCentroid();

    // Get polyhedron edge and triangle information.
    const int* currentIndex = &indices[0];
    for (int t = 0; t < numTriangles; ++t)
    {
        // Get vertex indices for triangle.
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;

        // Make sure triangle is counterclockwise.
        Vector3<Real>& vertex0 = mPoints[v0];
        Vector3<Real>& vertex1 = mPoints[v1];
        Vector3<Real>& vertex2 = mPoints[v2];

        Vector3<Real> diff = mCentroid - vertex0;
        Vector3<Real> edge1 = vertex1 - vertex0;
        Vector3<Real> edge2 = vertex2 - vertex0;
        Vector3<Real> normal = edge1.Cross(edge2);
        Real length = normal.Length();
        if (length > Math<Real>::ZERO_TOLERANCE)
        {
            normal /= length;
        }
        else
        {
            // The triangle is degenerate, use a "normal" that points towards
            // the centroid.
            normal = diff;
            normal.Normalize();
        }

        Real distance = normal.Dot(diff);
        if (distance < (Real)0)
        {
            // The triangle is counterclockwise.
            Insert(v0, v1, v2);
        }
        else
        {
            // The triangle is clockwise.
            Insert(v0, v2, v1);
        }
    }

    UpdatePlanes();
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::Create (const V3Array& points,
    const IArray& indices, const PArray& planes)
{
    assertion(points.size() >= 4 && indices.size() >= 4,
        "Polyhedron must be at least a tetrahedron.\n");

    const int numVertices = (int)points.size();
    const int numTriangles = (int)indices.size()/3;
    const int numEdges = numVertices + numTriangles - 2;
    Reset(numVertices, numEdges, numTriangles);
    mPoints = points;
    mPlanes = planes;

    // Copy polyhedron points into vertex array.  Compute centroid for use in
    // making sure the triangles are counterclockwise oriented when viewed
    // from the outside.
    ComputeCentroid();

    // Get polyhedron edge and triangle information.
    const int* currentIndex = &indices[0];
    for (int t = 0; t < numTriangles; ++t)
    {
        // Get vertex indices for triangle.
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;

        Real distance = mPlanes[t].DistanceTo(mCentroid);
        if (distance > (Real)0)
        {
            // The triangle is counterclockwise.
            Insert(v0, v1, v2);
        }
        else
        {
            // The triangle is clockwise.
            Insert(v0, v2, v1);
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>& ConvexPolyhedron<Real>::operator= (
    const ConvexPolyhedron& polyhedron)
{
    MTMesh::operator=(polyhedron);
    mPoints = polyhedron.mPoints;
    mPlanes = polyhedron.mPlanes;
    return *this;
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolyhedron<Real>::V3Array&
ConvexPolyhedron<Real>::GetPoints () const
{
    return mPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& ConvexPolyhedron<Real>::GetPoint (int i) const
{
    return mPoints[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const typename ConvexPolyhedron<Real>::PArray&
ConvexPolyhedron<Real>::GetPlanes () const
{
    return mPlanes;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& ConvexPolyhedron<Real>::GetPlane (int i) const
{
    return mPlanes[i];
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexPolyhedron<Real>::AddPoint (const Vector3<Real>& point)
{
    int numPoints = (int)mPoints.size();
    mPoints.push_back(point);
    return numPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
typename ConvexPolyhedron<Real>::V3Array& ConvexPolyhedron<Real>::Points ()
{
    return mPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real>& ConvexPolyhedron<Real>::Point (int i)
{
    return mPoints[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::UpdatePlanes ()
{
    // The planes are constructed to have *inner-pointing* normals.  This
    // supports the Wild Magic software clipping code that was based on a
    // view frustum having inner-pointing normals.
    ComputeCentroid();

    const int numTriangles = mTriangles.GetNumElements();
    mPlanes.resize(numTriangles);
    for (int t = 0; t < numTriangles; ++t)
    {
        MTTriangle& triangle = mTriangles[t];
        int v0 = GetVLabel(triangle.Vertex(0));
        int v1 = GetVLabel(triangle.Vertex(1));
        int v2 = GetVLabel(triangle.Vertex(2));
        Vector3<Real>& vertex0 = mPoints[v0];
        Vector3<Real>& vertex1 = mPoints[v1];
        Vector3<Real>& vertex2 = mPoints[v2];

        Vector3<Real> diff = mCentroid - vertex0;
        Vector3<Real> edge1 = vertex1 - vertex0;
        Vector3<Real> edge2 = vertex2 - vertex0;
        Vector3<Real> normal = edge2.Cross(edge1);
        Real length = normal.Length();
        if (length > Math<Real>::ZERO_TOLERANCE)
        {
            normal /= length;
            Real dot = normal.Dot(diff);
            if (dot < (Real)0)
            {
                normal = -normal;
            }
        }
        else
        {
            // The triangle is degenerate, use a "normal" that points towards
            // the centroid.
            normal = diff;
            normal.Normalize();
        }

        // The plane has inner-pointing normal.
        mPlanes[t] = Plane3<Real>(normal, normal.Dot(vertex0));
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::ValidateHalfSpaceProperty (Real epsilon) const
{
    Real maxDistance = -Math<Real>::MAX_REAL;
    Real minDistance = Math<Real>::MAX_REAL;
    const int numTriangles = mTriangles.GetNumElements();
    for (int t = 0; t < numTriangles; ++t)
    {
        const Plane3<Real>& plane = mPlanes[t];
        for (int i = 0; i < (int)mPoints.size(); ++i)
        {
            Real distance = plane.DistanceTo(mPoints[i]);
            if (distance < minDistance)
            {
                minDistance = distance;
            }
            if (distance > maxDistance)
            {
                maxDistance = distance;
            }
            if (distance < epsilon)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::ComputeCentroid ()
{
    mCentroid = Vector3<Real>::ZERO;
    const int numPoints = (int)mPoints.size();
    for (int i = 0; i < numPoints; ++i)
    {
        mCentroid += mPoints[i];
    }
    mCentroid /= (Real)numPoints;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& ConvexPolyhedron<Real>::GetCentroid () const
{
    return mCentroid;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::Clip (const Plane3<Real>& plane,
    ConvexPolyhedron& intersection) const
{
    ConvexClipper<Real> clipper(*this);
    int side = clipper.Clip(plane);

    if (side > 0)
    {
        intersection = *this;
        return true;
    }

    if (side < 0)
    {
        return false;
    }

    clipper.Convert(intersection);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::FindIntersection (
    const ConvexPolyhedron& polyhedron, ConvexPolyhedron& intersection) const
{
    ConvexClipper<Real> clipper(*this);

    const PArray& planes = polyhedron.GetPlanes();
    const int numPlanes = (int)planes.size();
    for (int i = 0; i < numPlanes; ++i)
    {
        if (clipper.Clip(planes[i]) < 0)
        {
            return false;
        }
    }

    clipper.Convert(intersection);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::FindAllIntersections (int numPolyhedra,
    ConvexPolyhedron* polyhedra, int& combos,
    ConvexPolyhedron**& intersections)
{
    // Only 2^16 possible combinations for intersections are currently
    // supported.  If you need more, then GetHighBit(int) must be modified
    // to handle more than 16-bit inputs.
    if (numPolyhedra <= 0 || numPolyhedra > 16)
    {
        combos = 0;
        intersections = 0;
        return;
    }

    combos = (1 << numPolyhedra);
    bool* needsTesting = new1<bool>(combos);
    intersections = new1<ConvexPolyhedron*>(combos);
    int i, j;
    for (i = 0; i < combos; ++i)
    {
        needsTesting[i] = true;
        intersections[i] = 0;
    }

    // Trivial cases, zero or one polyhedron--already the intersection.
    needsTesting[0] = false;
    for (i = 0; i < numPolyhedra; ++i)
    {
        j = (1 << i);
        needsTesting[j] = false;
        intersections[j] = new0 ConvexPolyhedron(polyhedra[i]);
    }
    
    for (i = 3; i < combos; ++i)
    {
        if (needsTesting[i])
        {
            // In binary, i = b[m]...b[0] where b[m] is not zero (the
            // high-order bit.  Also, i1 = b[m-1]...b[0] is not zero.  If it
            // were, we would have ruled out the combination by the j-loop
            // below.  Therefore, i0 = b[m]0...0 and i1 correspond to already
            // existing polyhedra.  The intersection finding just needs to
            // look at the intersection of the two polyhedra.
            int i0 = GetHighBit(i);
            int i1 = i & ~i0;
            intersections[i] = FindSolidIntersection(*intersections[i0],
                *intersections[i1]);
            if (!intersections[i])
            {
                // No intersection for this combination.  No need to test
                // other combinations that include this one.
                for (j = 0; j < combos; ++j)
                {
                    if ((i & j) == i)
                    {
                        needsTesting[j] = false;
                    }
                }
            }
#ifdef _DEBUG
            else  // Test whether we have a well-formed convex polyhedron.
            {
                Vector3<Real> centroid = intersections[i]->GetCentroid();
                bool contains = intersections[i]->ContainsPoint(centroid);
                assertion(contains, "Polyhedron is not well formed.\n");
            }
#endif
        }
    }

    delete1(needsTesting);
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConvexPolyhedron<Real>::GetSurfaceArea () const
{
    Real surfaceArea = (Real)0;

    const int numTriangles = mTriangles.GetNumElements();
    for (int t = 0; t < numTriangles; ++t)
    {
        const MTTriangle& triangle = mTriangles.Get(t);
        int v0 = GetVLabel(triangle.GetVertex(0));
        int v1 = GetVLabel(triangle.GetVertex(1));
        int v2 = GetVLabel(triangle.GetVertex(2));
        const Vector3<Real>& vertex0 = mPoints[v0];
        const Vector3<Real>& vertex1 = mPoints[v1];
        const Vector3<Real>& vertex2 = mPoints[v2];
        const Vector3<Real>& normal = mPlanes[t].Normal;

        surfaceArea += GetTriangleArea(normal, vertex0, vertex1, vertex2);
    }

    return surfaceArea;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConvexPolyhedron<Real>::GetVolume () const
{
    Real volume = (Real)0;

    const int numTriangles = mTriangles.GetNumElements();
    for (int t = 0; t < numTriangles; ++t)
    {
        const MTTriangle& triangle = mTriangles.Get(t);
        int v0 = GetVLabel(triangle.GetVertex(0));
        int v1 = GetVLabel(triangle.GetVertex(1));
        int v2 = GetVLabel(triangle.GetVertex(2));
        const Vector3<Real>& vertex0 = mPoints[v0];
        const Vector3<Real>& vertex1 = mPoints[v1];
        const Vector3<Real>& vertex2 = mPoints[v2];
        volume += vertex0.Dot(vertex1.Cross(vertex2));
    }

    volume /= (Real)6;
    return volume;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::ContainsPoint (const Vector3<Real>& point) const
{
    const int numTriangles = mTriangles.GetNumElements();
    for (int t = 0; t < numTriangles; ++t)
    {
        Real distance = mPlanes[t].DistanceTo(point);
        if (distance < (Real)0)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::ComputeTerminator (const Vector3<Real>& eye,
    V3Array& terminator)
{
    // Temporary storage for signed distances from eye to triangles.
    const int numTriangles = mTriangles.GetNumElements();
    std::vector<Real> distances(numTriangles);
    int i, j;
    for (i = 0; i < numTriangles; ++i)
    {
        distances[i] = Math<Real>::MAX_REAL;
    }

    // Start a search for a front-facing triangle that has an adjacent
    // back-facing triangle or for a back-facing triangle that has an
    // adjacent front-facing triangle.
    int tCurrent = 0;
    MTTriangle* triangle = &mTriangles[tCurrent];
    Real triDistance = GetDistance(eye, tCurrent, distances);
    int eFirst = -1;
    for (i = 0; i < numTriangles; ++i)
    {
        // Check adjacent neighbors for edge of terminator.  Such an edge
        // occurs if the signed distance changes sign.
        int minIndex = -1;
        Real minAbsDistance = Math<Real>::MAX_REAL;
        Real adjDistance[3];
        for (j = 0; j < 3; ++j)
        {
            adjDistance[j] = GetDistance(eye, triangle->Adjacent(j),
                distances);
            if (IsNegativeProduct(triDistance, adjDistance[j]))
            {
                eFirst = triangle->Edge(j);
                break;
            }

            Real absDistance = Math<Real>::FAbs(adjDistance[j]);
            if (absDistance < minAbsDistance)
            {
                minAbsDistance = absDistance;
                minIndex = j;
            }
        }
        if (j < 3)
        {
            break;
        }

        // First edge not found during this iteration.  Move to adjacent
        // triangle whose distance is smallest of all adjacent triangles.
        tCurrent = triangle->Adjacent(minIndex);
        triangle = &mTriangles[tCurrent];
        triDistance = adjDistance[minIndex];
    }
    assertion(i < numTriangles, "Unexpected condition.\n");

    MTEdge& edgeFirst = mEdges[eFirst];
    terminator.push_back(mPoints[GetVLabel(edgeFirst.Vertex(0))]);
    terminator.push_back(mPoints[GetVLabel(edgeFirst.Vertex(1))]);

    // Walk along the terminator.
    int vFirst = edgeFirst.Vertex(0);
    int v = edgeFirst.Vertex(1);
    int e = eFirst;
    const int numEdges = mEdges.GetNumElements();
    for (i = 0; i < numEdges; ++i)
    {
        // Search all edges sharing the vertex for another terminator edge.
        const int jmax = mVertices[v].GetNumEdges();
        for (j = 0; j < jmax; ++j)
        {
            int eNext = mVertices[v].GetEdge(j);
            if (eNext == e)
            {
                continue;
            }

            Real distance0 = GetDistance(eye, mEdges[eNext].GetTriangle(0),
                distances);
            Real distance1 = GetDistance(eye, mEdges[eNext].GetTriangle(1),
                distances);
            if (IsNegativeProduct(distance0, distance1))
            {
                if (mEdges[eNext].GetVertex(0) == v)
                {
                    v = mEdges[eNext].GetVertex(1);
                    terminator.push_back(mPoints[GetVLabel(v)]);
                    if (v == vFirst)
                    {
                        return;
                    }
                }
                else
                {
                    v = mEdges[eNext].GetVertex(0);
                    terminator.push_back(mPoints[GetVLabel(v)]);
                    if (v == vFirst)
                    {
                        return;
                    }
                }

                e = eNext;
                break;
            }
        }
        assertion(j < jmax, "Unexpected condition.\n");
    }
    assertion(i < numEdges, "Unexpected condition.\n");
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::ComputeSilhouette (const Vector3<Real>& eye,
    const Plane3<Real>& plane, const Vector3<Real>& U,
    const Vector3<Real>& V, V2Array& silhouette)
{
    V3Array terminator;
    ComputeTerminator(eye, terminator);
    return ComputeSilhouette(terminator, eye, plane, U, V, silhouette);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::ComputeSilhouette (V3Array& terminator,
    const Vector3<Real>& eye, const Plane3<Real>& plane,
    const Vector3<Real>& U, const Vector3<Real>& V, V2Array& silhouette)
{
    Real eyeDistance = plane.DistanceTo(eye);
    assertion(eyeDistance > (Real)0,
        "The eye must be outside the polyhedron.\n");

    // The closest planar point to E is K = E - distance*N.
    Vector3<Real> closest = eye - eyeDistance*plane.Normal;

    // Project the polyhedron points onto the plane.
    const int numVertices = (int)terminator.size();
    for (int i = 0; i < numVertices; ++i)
    {
        Vector3<Real>& point = terminator[i];

        Real vertexDistance = plane.DistanceTo(point);
        if (vertexDistance >= eyeDistance)
        {
            // Cannot project the vertex onto the plane.
            return false;
        }

        // Compute projected point Q.
        Real ratio = eyeDistance/(eyeDistance - vertexDistance);
        Vector3<Real> projected = eye + ratio*(point - eye);

        // Compute (x,y) so that Q = K + x*U + y*V + z*N.
        Vector3<Real> diff = projected - closest;
        silhouette.push_back(Vector2<Real>(U.Dot(diff), V.Dot(diff)));
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::CreateEggShape (const Vector3<Real>& center,
    Real x0, Real x1, Real y0, Real y1, Real z0, Real z1, int maxSteps,
    ConvexPolyhedron& egg)
{
    assertion(x0 > (Real)0 && x1 > (Real)0, "Invalid input.\n");
    assertion(y0 > (Real)0 && y1 > (Real)0, "Invalid input.\n");
    assertion(z0 > (Real)0 && z1 > (Real)0, "Invalid input.\n");
    assertion(maxSteps >= 0, "Invalid input.\n");

    // Start with an octahedron whose 6 vertices are (-x0,0,0), (x1,0,0),
    // (0,-y0,0), (0,y1,0), (0,0,-z0), (0,0,z1).  The center point will be
    // added later.
    V3Array points(6);
    points[0] = Vector3<Real>(-x0, (Real)0, (Real)0);
    points[1] = Vector3<Real>( x1, (Real)0, (Real)0);
    points[2] = Vector3<Real>((Real)0, -y0, (Real)0);
    points[3] = Vector3<Real>((Real)0,  y1, (Real)0);
    points[4] = Vector3<Real>((Real)0, (Real)0, -z0);
    points[5] = Vector3<Real>((Real)0, (Real)0,  z1);

    IArray indices(24);
    indices[ 0] = 1;  indices[ 1] = 3;  indices[ 2] = 5;
    indices[ 3] = 3;  indices[ 4] = 0;  indices[ 5] = 5;
    indices[ 6] = 0;  indices[ 7] = 2;  indices[ 8] = 5;
    indices[ 9] = 2;  indices[10] = 1;  indices[11] = 5;
    indices[12] = 3;  indices[13] = 1;  indices[14] = 4;
    indices[15] = 0;  indices[16] = 3;  indices[17] = 4;
    indices[18] = 2;  indices[19] = 0;  indices[20] = 4;
    indices[21] = 1;  indices[22] = 2;  indices[23] = 4;

    egg.InitialELabel() = 0;
    egg.Create(points, indices);

    // Subdivide the triangles.  The midpoints of the edges are computed.
    // The triangle is replaced by four subtriangles using the original 3
    // vertices and the 3 new edge midpoints.
    int i;
    for (int step = 1; step <= maxSteps; ++step)
    {
        int numVertices = egg.GetNumVertices();
        int numEdges = egg.GetNumEdges();
        int numTriangles = egg.GetNumTriangles();

        // Compute lifted edge midpoints.
        for (i = 0; i < numEdges; ++i)
        {
            // Get an edge.
            const MTEdge& edge = egg.GetEdge(i);
            int v0 = egg.GetVLabel(edge.GetVertex(0));
            int v1 = egg.GetVLabel(edge.GetVertex(1));

            // Compute lifted centroid to points.
            Vector3<Real> lifted = egg.Point(v0) + egg.Point(v1);
            Real xr = (lifted.X() > (Real)0 ? lifted.X()/x1 : lifted.X()/x0);
            Real yr = (lifted.Y() > (Real)0 ? lifted.Y()/y1 : lifted.Y()/y0);
            Real zr = (lifted.Z() > (Real)0 ? lifted.Z()/z1 : lifted.Z()/z0);
            lifted *= Math<Real>::InvSqrt(xr*xr + yr*yr + zr*zr);

            // Add the point to the array.  Store the point index in the edge
            // label for support in adding new triangles.
            egg.ELabel(i) = numVertices++;
            egg.AddPoint(lifted);
        }

        // Add the new triangles and remove the old triangle.  The removal
        // in slot i will cause the last added triangle to be moved to that
        // slot.  This side effect will not interfere with the iteration
        // and removal of the triangles.
        for (i = 0; i < numTriangles; ++i)
        {
            const MTTriangle& triangle = egg.GetTriangle(i);
            int v0  = egg.GetVLabel(triangle.GetVertex(0));
            int v1  = egg.GetVLabel(triangle.GetVertex(1));
            int v2  = egg.GetVLabel(triangle.GetVertex(2));
            int v01 = egg.GetELabel(triangle.GetEdge(0));
            int v12 = egg.GetELabel(triangle.GetEdge(1));
            int v20 = egg.GetELabel(triangle.GetEdge(2));
            egg.Insert(v0,  v01, v20);
            egg.Insert(v01, v1,  v12);
            egg.Insert(v20, v12, v2);
            egg.Insert(v01, v12, v20);
            egg.Remove(v0,  v1,  v2);
        }
    }

    // Add the center.
    const int numEggPoints = (int)egg.mPoints.size();
    for (i = 0; i < numEggPoints; ++i)
    {
        egg.mPoints[i] += center;
    }

    egg.UpdatePlanes();
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexPolyhedron<Real>::Print (std::ofstream& outFile) const
{
    MTMesh::Print(outFile);

    int i;
    char message[512];

    outFile << "points:" << std::endl;
    const int numPoints = (int)mPoints.size();
    for (i = 0; i < numPoints; ++i)
    {
        const Vector3<Real>& vertex = mPoints[i];
        sprintf(message, "point<%d> = (%+8.4f, %+8.4f, %+8.4f)", i,
            vertex.X(), vertex.Y(), vertex.Z());
        outFile << message << std::endl;
    }
    outFile << std::endl;

    outFile << "planes:" << std::endl;
    const int numPlanes = (int)mPlanes.size();
    for (i = 0; i < numPlanes; ++i)
    {
        const Plane3<Real>& plane = mPlanes[i];
        sprintf(message, "plane<%d> = (%+8.6f, %+8.6f, %+8.6f; %+8.4f)", i,
            plane.Normal.X(), plane.Normal.Y(), plane.Normal.Z(),
            plane.Constant);
        outFile << message << std::endl;
    }
    outFile << std::endl;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::Print (const char* filename) const
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        return false;
    }

    Print(outFile);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexPolyhedron<Real>* ConvexPolyhedron<Real>::FindSolidIntersection (
    const ConvexPolyhedron& polyhedron0, const ConvexPolyhedron& polyhedron1)
{
    ConvexPolyhedron* intersection = new0 ConvexPolyhedron();
    if (polyhedron0.FindIntersection(polyhedron1, *intersection))
    {
        return intersection;
    }
    
    // As surfaces, the polyhedra do not intersect.  However, as solids,
    // one polyhedron might be fully contained in the other.
    if (polyhedron0.ContainsPoint(polyhedron1.GetCentroid()))
    {
        *intersection = polyhedron1;
        return intersection;
    }
        
    if (polyhedron1.ContainsPoint(polyhedron0.GetCentroid()))
    {
        *intersection = polyhedron0;
        return intersection;
    }

    delete0(intersection);
    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexPolyhedron<Real>::GetHighBit (int i)
{
    // assert: i in [1,2^16].  That is, (i>0) && (0xFFFF0000&i)==0.
    // This is a binary search for the high-order bit of i.
    if ((i & 0xFF00) != 0)
    {
        if ((i & 0xF000) != 0)
        {
            if ((i & 0xC000) != 0)
            {
                if ((i & 0x8000) != 0)
                {
                    return 0x8000;
                }
                else // (i & 0x4000) != 0
                {
                    return 0x4000;
                }
            }
            else  // (i & 0x3000) != 0
            {
                if ((i & 0x2000) != 0)
                {
                    return 0x2000;
                }
                else  // (i & 0x1000) != 0
                {
                    return 0x1000;
                }
            }
        }
        else  // (i & 0x0F00) != 0
        {
            if ((i & 0x0C00) != 0)
            {
                if ((i & 0x0800) != 0)
                {
                    return 0x0800;
                }
                else  // (i & 0x0400) != 0
                {
                    return 0x0400;
                }
            }
            else  // (i & 0x0300) != 0
            {
                if ((i & 0x0200) != 0)
                {
                    return 0x0200;
                }
                else  // (i & 0x0100) != 0
                {
                    return 0x0100;
                }
            }
        }
    }
    else  // (i & 0x00FF)
    {
        if ((i & 0x00F0) != 0)
        {
            if ((i & 0x00C0) != 0)
            {
                if ((i & 0x0080) != 0)
                {
                    return 0x0080;
                }
                else  // (i & 0x0040) != 0
                {
                    return 0x0040;
                }
            }
            else  // (i & 0x0030) != 0
            {
                if ((i & 0x0020) != 0)
                {
                    return 0x0020;
                }
                else  // (i & 0x0010) != 0
                {
                    return 0x0010;
                }
            }
        }
        else  // (i & 0x000F) != 0
        {
            if ((i & 0x000C) != 0)
            {
                if ((i & 0x0008) != 0)
                {
                    return 0x0008;
                }
                else  // (i & 0x0004) != 0
                {
                    return 0x0004;
                }
            }
            else  // (i & 0x0003) != 0
            {
                if ((i & 0x0002) != 0)
                {
                    return 0x0002;
                }
                else  // (i & 0x0001) != 0
                {
                    return 0x0001;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConvexPolyhedron<Real>::GetTriangleArea (const Vector3<Real>& normal,
    const Vector3<Real>& vertex0, const Vector3<Real>& vertex1,
    const Vector3<Real>& vertex2) const
{
    // Compute maximum absolute component of normal vector.
    int maxIndex = 0;
    Real maxAbsValue = Math<Real>::FAbs(normal.X());

    Real absValue = Math<Real>::FAbs(normal.Y());
    if (absValue > maxAbsValue)
    {
        maxIndex = 1;
        maxAbsValue = absValue;
    }

    absValue = Math<Real>::FAbs(normal.Z());
    if (absValue > maxAbsValue)
    {
        maxIndex = 2;
        maxAbsValue = absValue;
    }

    // Trap degenerate triangles.
    if (maxAbsValue == (Real)0)
    {
        return (Real)0;
    }

    // Compute area of projected triangle.
    Real d0, d1, d2, area;
    if (maxIndex == 0)
    {
        d0 = vertex1.Z() - vertex2.Z();
        d1 = vertex2.Z() - vertex0.Z();
        d2 = vertex0.Z() - vertex1.Z();
        area = Math<Real>::FAbs(vertex0.Y()*d0 + vertex1.Y()*d1 +
            vertex2.Y()*d2);
    }
    else if (maxIndex == 1)
    {
        d0 = vertex1.X() - vertex2.X();
        d1 = vertex2.X() - vertex0.X();
        d2 = vertex0.X() - vertex1.X();
        area = Math<Real>::FAbs(vertex0.Z()*d0 + vertex1.Z()*d1 +
            vertex2.Z()*d2);
    }
    else
    {
        d0 = vertex1.Y() - vertex2.Y();
        d1 = vertex2.Y() - vertex0.Y();
        d2 = vertex0.Y() - vertex1.Y();
        area = Math<Real>::FAbs(vertex0.X()*d0 + vertex1.X()*d1 +
            vertex2.X()*d2);
    }

    area *= ((Real)0.5)/maxAbsValue;
    return area;
}
//----------------------------------------------------------------------------
template <typename Real>
Real ConvexPolyhedron<Real>::GetDistance (const Vector3<Real>& eye, int t,
    std::vector<Real>& distances) const
{
    // Signed distance from eye to plane of triangle.  When distance is
    // positive, triangle is visible from eye (front-facing).  When distance
    // is negative, triangle is not visible from eye (back-facing).  When
    // distance is zero, triangle is visible "on-edge" from eye.

    if (distances[t] == Math<Real>::MAX_REAL)
    {
        distances[t] = -mPlanes[t].DistanceTo(eye);
        if (Math<Real>::FAbs(distances[t]) < Math<Real>::ZERO_TOLERANCE)
        {
            distances[t] = (Real)0;
        }
    }

    return distances[t];
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexPolyhedron<Real>::IsNegativeProduct (Real distance0,
    Real distance1)
{
    return (distance0 != (Real)0 ? (distance0*distance1 <= (Real)0) :
        (distance1 != (Real)0));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template class ConvexPolyhedron<float>;
template class ConvexPolyhedron<double>;
//----------------------------------------------------------------------------
