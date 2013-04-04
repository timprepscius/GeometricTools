// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/08)

#include "ConvexClipper.h"
#include "ConvexPolyhedron.h"

//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::ConvexClipper (const ConvexPolyhedron<Real>& polyhedron,
    Real epsilon)
    :
    mEpsilon(epsilon)
{
    const std::vector<Vector3<Real> >& points = polyhedron.GetPoints();
    int numVertices = polyhedron.GetNumVertices();
    mVertices.resize(numVertices);
    for (int v = 0; v < numVertices; ++v)
    {
        mVertices[v].Point = points[v];
    }

    int numEdges = polyhedron.GetNumEdges();
    mEdges.resize(numEdges);
    for (int e = 0; e < numEdges; ++e)
    {
        const MTEdge& edge = polyhedron.GetEdge(e);
        for (int i = 0; i < 2; ++i)
        {
            mEdges[e].Vertex[i] = polyhedron.GetVLabel(edge.GetVertex(i));
            mEdges[e].Face[i] = edge.GetTriangle(i);
        }
    }

    int numTriangles = polyhedron.GetNumTriangles();
    mFaces.resize(numTriangles);
    for (int t = 0; t < numTriangles; ++t)
    {
        mFaces[t].Plane = polyhedron.GetPlane(t);
        const MTTriangle& triangle = polyhedron.GetTriangle(t);
        for (int i = 0; i < 3; ++i)
        {
            mFaces[t].Edges.insert(triangle.GetEdge(i));
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
int ConvexClipper<Real>::Clip (const Plane3<Real>& plane)
{
    // Compute signed distances from vertices to plane.
    int numPositive = 0, numNegative = 0, numZero = 0;
    const int numVertices = (int)mVertices.size();
    for (int v = 0; v < numVertices; ++v)
    {
        Vertex& vertex = mVertices[v];
        if (vertex.Visible)
        {
            vertex.Distance = plane.DistanceTo(vertex.Point);
            if (vertex.Distance > mEpsilon)
            {
                ++numPositive;
            }
            else if (vertex.Distance < -mEpsilon)
            {
                ++numNegative;
                vertex.Visible = false;
            }
            else
            {
                // The point is on the plane (within floating point
                // tolerance).
                ++numZero;
                vertex.Distance = (Real)0;
            }
        }
    }

    if (numPositive == 0)
    {
        // Mesh is in negative half-space, fully clipped.
        return -1;
    }

    if (numNegative == 0)
    {
        // Mesh is in positive half-space, fully visible.
        return +1;
    }

    // Clip the visible edges.
    const int numEdges = (int)mEdges.size();
    for (int e = 0; e < numEdges; ++e)
    {
        Edge& edge = mEdges[e];
        if (edge.Visible)
        {
            int v0 = edge.Vertex[0];
            int v1 = edge.Vertex[1];
            int f0 = edge.Face[0];
            int f1 = edge.Face[1];
            Face& face0 = mFaces[f0];
            Face& face1 = mFaces[f1];
            Real d0 = mVertices[v0].Distance;
            Real d1 = mVertices[v1].Distance;

            if (d0 <= (Real)0 && d1 <= (Real)0)
            {
                // The edge is culled.  If the edge is exactly on the clip
                // plane, it is possible that a visible triangle shares it.
                // The edge will be re-added during the face loop.
                face0.Edges.erase(e);
                if (face0.Edges.empty())
                {
                    face0.Visible = false;
                }

                face1.Edges.erase(e);
                if (face1.Edges.empty())
                {
                    face1.Visible = false;
                }

                edge.Visible = false;
                continue;
            }

            if (d0 >= (Real)0 && d1 >= (Real)0)
            {
                // Face retains the edge.
                continue;
            }

            // The edge is split by the plane.  Compute the point of
            // intersection.  If the old edge is <V0,V1> and I is the
            // intersection point, the new edge is <V0,I> when d0 > 0 or
            // <I,V1> when d1 > 0.
            int vNew = (int)mVertices.size();
            mVertices.push_back(Vertex());
            Vertex& vertexNew = mVertices[vNew];

            Vector3<Real>& point0 = mVertices[v0].Point;
            Vector3<Real>& point1 = mVertices[v1].Point;
            vertexNew.Point = point0 + (d0/(d0 - d1))*(point1 - point0);

            if (d0 > (Real)0)
            {
                edge.Vertex[1] = vNew;
            }
            else
            {
                edge.Vertex[0] = vNew;
            }
        }
    }

    // The mesh straddles the plane.  A new convex polygonal face will be
    // generated.  Add it now and insert edges when they are visited.
    int fNew = (int)mFaces.size();
    mFaces.push_back(Face());
    Face& faceNew = mFaces[fNew];
    faceNew.Plane = plane;

    // Process the faces.
    for (int f = 0; f < fNew; ++f)
    {
        Face& face = mFaces[f];
        if (face.Visible)
        {
            // Determine if the face is on the negative side, the positive
            // side, or split by the clipping plane.  The Occurs members
            // are set to zero to help find the end points of the polyline
            // that results from clipping a face.
            assertion(face.Edges.size() >= 2, "Unexpected condition.\n");
            std::set<int>::iterator iter = face.Edges.begin();
            std::set<int>::iterator end = face.Edges.end();
            while (iter != end)
            {
                int e = *iter++;
                Edge& edge = mEdges[e];
                assertion(edge.Visible, "Unexpected condition.\n");
                mVertices[edge.Vertex[0]].Occurs = 0;
                mVertices[edge.Vertex[1]].Occurs = 0;
            }

            int vStart, vFinal;
            if (GetOpenPolyline(face, vStart, vFinal))
            {
                // Polyline is open, close it up.
                int eNew = (int)mEdges.size();
                mEdges.push_back(Edge());
                Edge& edgeNew = mEdges[eNew];

                edgeNew.Vertex[0] = vStart;
                edgeNew.Vertex[1] = vFinal;
                edgeNew.Face[0] = f;
                edgeNew.Face[1] = fNew;

                // Add new edge to polygons.
                face.Edges.insert(eNew);
                faceNew.Edges.insert(eNew);
            }
        }
    }

    // Process 'faceNew' to make sure it is a simple polygon (theoretically
    // convex, but numerically may be slightly not convex).  Floating-point
    // round-off errors can cause the new face from the last loop to be
    // needle-like with a collapse of two edges into a single edge.  This
    // block guarantees the invariant "face always a simple polygon".
    Postprocess(fNew, faceNew);
    if (faceNew.Edges.size() < 3)
    {
        // Face is completely degenerate, remove it from mesh.
        mFaces.pop_back();
    }

    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexClipper<Real>::Convert (ConvexPolyhedron<Real>& polyhedron)
{
    // Get visible vertices.
    int numVertices = (int)mVertices.size();
    std::vector<Vector3<Real> > points;
    int* vMap = new1<int>(numVertices);
    memset(vMap, 0xFF, numVertices*sizeof(int));
    for (int v = 0; v < numVertices; ++v)
    {
        const Vertex& vertex = mVertices[v];
        if (vertex.Visible)
        {
            vMap[v] = (int)points.size();
            points.push_back(vertex.Point);
        }
    }

    std::vector<int> indices;
    std::vector<Plane3<Real> > planes;
    GetTriangles(indices, planes);

    // Reorder the indices.
    for (int c = 0; c < (int)indices.size(); ++c)
    {
        int oldC = indices[c];
        assertion(0 <= oldC && oldC < numVertices, "Index out of range.\n");
        int newC = vMap[oldC];
        assertion(0 <= newC && newC < (int)points.size(),
            "Index out of range.\n");
        indices[c] = newC;
    }

    delete1(vMap);

    polyhedron.Create(points, indices, planes);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexClipper<Real>::Print (const char* filename) const
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        return false;
    }

    const int numVertices = (int)mVertices.size();
    const int numEdges = (int)mEdges.size();
    const int numFaces = (int)mFaces.size();
    const int vDigits = (int)(ceil(log10((double)numVertices)));
    const int eDigits = (int)(ceil(log10((double)numEdges)));
    const int fDigits = (int)(ceil(log10((double)numFaces)));
    char message[1024];

    outFile << numVertices << " vertices" << std::endl;
    for (int v = 0; v < numVertices; ++v)
    {
        const Vertex& vertex = mVertices[v];
        sprintf(message, "v<%*d> %c: (%f,%f,%f)",
            vDigits, v, (vertex.Visible ? 'T' : 'F'), vertex.Point.X(),
            vertex.Point.Y(), vertex.Point.Z());
        outFile << message << std::endl;
    }
    outFile << std::endl;

    outFile << numEdges << " edges" << std::endl;
    for (int e = 0; e < numEdges; ++e)
    {
        const Edge& edge = mEdges[e];
        sprintf(message, "e<%*d> %c: v[%*d,%*d], t[%*d,%*d]",
            eDigits, e, (edge.Visible ? 'T' : 'F'), vDigits, edge.Vertex[0],
            vDigits, edge.Vertex[1], fDigits, edge.Face[0], fDigits,
            edge.Face[1]);
        outFile << message << std::endl;
    }
    outFile << std::endl;

    outFile << numFaces << " faces" << std::endl;
    for (int f = 0; f < numFaces; ++f)
    {
        const Face& face = mFaces[f];
        sprintf(message, "t<%*d> %d: e = ",
            fDigits, f, (face.Visible ? 'T' : 'F'));
        outFile << message;

        std::set<int>::const_iterator iter = face.Edges.begin();
        std::set<int>::const_iterator end = face.Edges.end();
        while (iter != end)
        {
            outFile << *iter << ' ';
            iter++;
        }
        outFile << std::endl;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexClipper<Real>::Postprocess (int fNew, Face& faceNew)
{
    const int numEdges = (int)faceNew.Edges.size();
    std::vector<EdgePlus> edges(numEdges);
    std::set<int>::iterator iter = faceNew.Edges.begin();
    std::set<int>::iterator end = faceNew.Edges.end();
    int i = 0;
    while (iter != end)
    {
        int e = *iter++;
        edges[i++] = EdgePlus(e, mEdges[e]);
    }
    std::sort(edges.begin(), edges.end());

    // Process duplicate edges.
    for (int i0 = 0, i1 = 1; i1 < numEdges; i0 = i1++)
    {
        if (edges[i0] == edges[i1])
        {
            // Found two equivalent edges (same vertex end points).
#ifdef _DEBUG
            int i2 = i1 + 1;
            if (i2 < numEdges)
            {
                // Make sure an edge occurs at most twice.  If not, then
                // algorithm needs to be modified to handle it.
                assertion(edges[i1] != edges[i2], "Unexpected condition.\n");
            }
#endif
            // Edge E0 has vertices V0, V1 and faces F0, NF.  Edge E1 has
            // vertices V0, V1 and faces F1, NF.
            int e0 = edges[i0].E;
            int e1 = edges[i1].E;
            Edge& edge0 = mEdges[e0];
            Edge& edge1 = mEdges[e1];

            // Remove E0 and E1 from faceNew.
            faceNew.Edges.erase(e0);
            faceNew.Edges.erase(e1);

            // Remove faceNew from E0.
            if (edge0.Face[0] == fNew)
            {
                edge0.Face[0] = edge0.Face[1];
            }
            else
            {
                assertion(edge0.Face[1] == fNew, "Unexpected condition.\n");
            }
            edge0.Face[1] = -1;

            // Remove faceNew from E1.
            if (edge1.Face[0] == fNew)
            {
                edge1.Face[0] = edge1.Face[1];
            }
            else
            {
                assertion(edge1.Face[1] == fNew, "Unexpected condition.\n");
            }
            edge1.Face[1] = -1;

            // E2 is being booted from the system.  Update the face F1 that
            // shares it.  Update E1 to share F1.
            int f1 = edge1.Face[0];
            Face& face1 = mFaces[f1];
            face1.Edges.erase(e1);
            face1.Edges.insert(e0);
            edge0.Face[1] = f1;
            edge1.Visible = false;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexClipper<Real>::GetOpenPolyline (Face& face, int& vStart,
    int& vFinal)
{
    // Count the number of occurrences of each vertex in the polyline.
    bool okay = true;
    std::set<int>::iterator iter = face.Edges.begin();
    std::set<int>::iterator end = face.Edges.end();
    while (iter != end)
    {
        int e = *iter++;
        Edge& edge = mEdges[e];

        int v0 = edge.Vertex[0];
        ++mVertices[v0].Occurs;
        if (mVertices[v0].Occurs > 2)
        {
            okay = false;
        }

        int v1 = edge.Vertex[1];
        ++mVertices[v1].Occurs;
        if (mVertices[v1].Occurs > 2)
        {
            okay = false;
        }
    }

    if (!okay)
    {
#ifdef _DEBUG
        // If you reach this block, there is a good chance that floating
        // point round-off error had caused this face to be needle-like and
        // what was theoretically a narrow V-shaped portion (a vertex shared
        // by two edges forming a small angle) has collapsed into a single
        // line segment.
        //
        // NOTE.  Once I added Postprocess, I have not gotten to this block.
        std::ofstream outFile("error.txt");
        iter = face.Edges.begin();
        end = face.Edges.end();
        while (iter != end)
        {
            int e = *iter++;
            Edge& edge = mEdges[e];
            outFile << "e<" << e << "> = <" << edge.Vertex[0] << ","
                  << edge.Vertex[1] << "|" << edge.Face[0] << ","
                  << edge.Face[1] << "> ";
            if (edge.Visible)
            {
                outFile << "T" << std::endl;
            }
            else
            {
                outFile << "F" << std::endl;
            }
        }
        outFile.close();

        assertion(false, "Probable numerical round-off errors.\n");
#else
        return false;
#endif
    }

    // Determine whether the polyline is open.
    iter = face.Edges.begin();
    end = face.Edges.end();
    vStart = -1;
    vFinal = -1;
    while (iter != end)
    {
        int e = *iter++;
        Edge& edge = mEdges[e];

        int v0 = edge.Vertex[0];
        if (mVertices[v0].Occurs == 1)
        {
            if (vStart == -1)
            {
                vStart = v0;
            }
            else if (vFinal == -1)
            {
                vFinal = v0;
            }
            else
            {
                // If you reach this assert, there is a good chance that the
                // polyhedron is not convex.  To check this, use the function
                // ValidateHalfSpaceProperty() on your polyhedron right after
                // you construct it.
                assertion(false, "Polyhedron might not be convex.\n");
            }
        }

        int v1 = edge.Vertex[1];
        if (mVertices[v1].Occurs == 1)
        {
            if (vStart == -1)
            {
                vStart = v1;
            }
            else if (vFinal == -1)
            {
                vFinal = v1;
            }
            else
            {
                // If you reach this assert, there is a good chance that the
                // polyhedron is not convex.  To check this, use the function
                // ValidateHalfSpaceProperty() on your polyhedron right after
                // you construct it.
                assertion(false, "Polyhedron might not be convex.\n");
            }
        }
    }

    assertion((vStart == -1 && vFinal == -1)
        || (vStart != -1 && vFinal != -1), "Unexpected condition.\n");

    return vStart != -1;
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexClipper<Real>::OrderVertices (Face& face,
    std::vector<int>& vOrdered)
{
    // Copy edge indices into contiguous memory.
    const int numEdges = (int)face.Edges.size();
    std::vector<int> eOrdered(numEdges);
    std::set<int>::iterator iter = face.Edges.begin();
    std::set<int>::iterator end = face.Edges.end();
    int i = 0;
    while (iter != end)
    {
        eOrdered[i++] = *iter++;
    }

    // Bubble sort (yes, it is...).
    for (int i0 = 0, i1 = 1, choice = 1; i1 < numEdges - 1; i0 = i1++)
    {
        Edge& edgeCurr = mEdges[eOrdered[i0]];
        int j, curr = edgeCurr.Vertex[choice];
        for (j = i1; j < numEdges; ++j)
        {
            Edge& edgeTemp = mEdges[eOrdered[j]];
            int save;
            if (edgeTemp.Vertex[0] == curr)
            {
                save = eOrdered[i1];
                eOrdered[i1] = eOrdered[j];
                eOrdered[j] = save;
                choice = 1;
                break;
            }
            if (edgeTemp.Vertex[1] == curr)
            {
                save = eOrdered[i1];
                eOrdered[i1] = eOrdered[j];
                eOrdered[j] = save;
                choice = 0;
                break;
            }
        }
        assertion(j < numEdges, "Unexpected condition.\n");
    }

    vOrdered[0] = mEdges[eOrdered[0]].Vertex[0];
    vOrdered[1] = mEdges[eOrdered[0]].Vertex[1];
    for (i = 1; i < numEdges; ++i)
    {
        Edge& edge = mEdges[eOrdered[i]];
        if (edge.Vertex[0] == vOrdered[i])
        {
            vOrdered[i + 1] = edge.Vertex[1];
        }
        else
        {
            vOrdered[i + 1] = edge.Vertex[0];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void ConvexClipper<Real>::GetTriangles (std::vector<int>& indices,
    std::vector<Plane3<Real> >& planes)
{
    const int numFaces = (int)mFaces.size();
    for (int f = 0; f < numFaces; ++f)
    {
        Face& face = mFaces[f];
        if (face.Visible)
        {
            const int numEdges = (int)face.Edges.size();
            assertion(numEdges >= 3, "Unexpected condition.\n");
            std::vector<int> vOrdered(numEdges + 1);
            OrderVertices(face, vOrdered);

            int v0 = vOrdered[0];
            int v2 = vOrdered[numEdges - 1];
            int v1 = vOrdered[(numEdges - 1) >> 1];
            Vector3<Real> diff1 = mVertices[v1].Point - mVertices[v0].Point;
            Vector3<Real> diff2 = mVertices[v2].Point - mVertices[v0].Point;
            Real sgnVolume = face.Plane.Normal.Dot(diff1.Cross(diff2));
            if (sgnVolume > (Real)0)
            {
                // Clockwise, need to swap.
                for (int i = 1; i + 1 < numEdges; ++i)
                {
                    indices.push_back(v0);
                    indices.push_back(vOrdered[i + 1]);
                    indices.push_back(vOrdered[i]);
                    planes.push_back(face.Plane);
                }
            }
            else
            {
                // Counterclockwise.
                for (int i = 1; i + 1 < numEdges; ++i)
                {
                    indices.push_back(v0);
                    indices.push_back(vOrdered[i]);
                    indices.push_back(vOrdered[i + 1]);
                    planes.push_back(face.Plane);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexClipper::Vertex
//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::Vertex::Vertex ()
    :
    Distance((Real)0),
    Visible(true)
{
    // Point and Occurs are uninitialized.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexClipper::Edge
//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::Edge::Edge ()
    :
    Visible(true)
{
    // Vertex[] and Face[] are uninitialized.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexClipper::Face
//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::Face::Face ()
    :
    Visible(true)
{
    // Plane is uninitialized, Edges is empty.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ConvexClipper::EdgePlus
//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::EdgePlus::EdgePlus ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
ConvexClipper<Real>::EdgePlus::EdgePlus (int e, const Edge& edge)
    :
    E(e)
{
    F0 = edge.Face[0];
    F1 = edge.Face[1];

    if (edge.Vertex[0] < edge.Vertex[1])
    {
        V0 = edge.Vertex[0];
        V1 = edge.Vertex[1];
    }
    else
    {
        V0 = edge.Vertex[1];
        V1 = edge.Vertex[0];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexClipper<Real>::EdgePlus::operator< (const EdgePlus& edge) const
{
    if (V1 < edge.V1)
    {
        return true;
    }

    if (V1 == edge.V1)
    {
        return V0 < edge.V0;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexClipper<Real>::EdgePlus::operator== (const EdgePlus& edge) const
{
    return V0 == edge.V0 && V1 == edge.V1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool ConvexClipper<Real>::EdgePlus::operator!= (const EdgePlus& edge) const
{
    return V0 != edge.V0 || V1 != edge.V1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template class ConvexClipper<float>;
template class ConvexClipper<double>;
//----------------------------------------------------------------------------
