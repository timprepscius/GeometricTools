// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2012/07/06)

#include "Climb2D.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
Climb2D::Climb2D (int N, int* data)
    :
    mN(N),
    mTwoPowerN(1 << N),
    mSize(mTwoPowerN + 1),
    mData(data)
{
    mXMerge = new1<LinearMergeTree*>(mSize);
    mYMerge = new1<LinearMergeTree*>(mSize);
    for (int i = 0; i < mSize; ++i)
    {
        mXMerge[i] = new0 LinearMergeTree(mN);
        mYMerge[i] = new0 LinearMergeTree(mN);
    }

    mXYMerge = new0 AreaMergeTree(mN, mXMerge, mYMerge);
}
//----------------------------------------------------------------------------
Climb2D::~Climb2D ()
{
    delete1(mData);

    for (int i = 0; i < mSize; ++i)
    {
        delete0(mXMerge[i]);
        delete0(mYMerge[i]);
    }
    delete1(mXMerge);
    delete1(mYMerge);

    delete0(mXYMerge);
}
//----------------------------------------------------------------------------
void Climb2D::ExtractContour (float level, int depth, int& numVertices,
    Vector2f*& vertices, int& numEdges, Edge2*& edges)
{
    std::vector<Rectangle2> rectangles;
    std::vector<Vector2f> localVertices;
    std::vector<Edge2> localEdges;

    SetLevel(level, depth);
    GetRectangles(rectangles);
    const int numRectangles = (int)rectangles.size();
    for (int i = 0; i < numRectangles; ++i)
    {
        Rectangle2& rectangle = rectangles[i];
        if (rectangle.mType > 0)
        {
            GetComponents(level, rectangle, localVertices, localEdges);
        }
    }

    numVertices = (int)localVertices.size();
    vertices = new1<Vector2f>(numVertices);
    memcpy(vertices, &localVertices[0], numVertices*sizeof(Vector2f));

    numEdges = (int)localEdges.size();
    edges = new1<Edge2>(numEdges);
    memcpy(edges, &localEdges[0], numEdges*sizeof(Edge2));
}
//----------------------------------------------------------------------------
void Climb2D::MakeUnique (int& numVertices, Vector2f*& vertices,
    int& numEdges, Edge2*& edges)
{
    if (numVertices == 0)
    {
        return;
    }

    // Use a hash table to generate unique storage.
    typedef std::map<Vector2f,int> VMap;
    typedef VMap::iterator VIterator;
    VMap vMap;
    for (int v = 0, nextVertex = 0; v < numVertices; ++v)
    {
        std::pair<VIterator,bool> result = vMap.insert(
            std::make_pair(vertices[v], nextVertex));
        if (result.second == true)
        {
            ++nextVertex;
        }
    }

    // Use a hash table to generate unique storage.
    typedef std::map<Edge2,int> EMap;
    typedef EMap::iterator EIterator;
    EMap* eMap = 0;
    int e;
    VIterator viter;

    if (numEdges > 0)
    {
        eMap = new0 EMap();
        int nextEdge = 0;
        for (e = 0; e < numEdges; ++e)
        {
            // Replace old vertex indices by new ones.
            viter = vMap.find(vertices[edges[e].first]);
            assertion(viter != vMap.end(), "Unexpected condition.\n");
            edges[e].first = viter->second;
            viter = vMap.find(vertices[edges[e].second]);
            assertion(viter != vMap.end(), "Unexpected condition.\n");
            edges[e].second = viter->second;

            // Keep only unique edges.
            std::pair<EIterator,bool> result = eMap->insert(
                std::make_pair(edges[e], nextEdge));
            if (result.second == true)
            {
                ++nextEdge;
            }
        }
    }

    // Pack vertices into an array.
    delete1(vertices);
    if (numVertices > 0)
    {
        numVertices = (int)vMap.size();
        vertices = new1<Vector2f>(numVertices);
        for (viter = vMap.begin(); viter != vMap.end(); ++viter)
        {
            vertices[viter->second] = viter->first;
        }
    }
    else
    {
        vertices = 0;
    }

    // Pack edges into an array.
    delete1(edges);
    if (numEdges > 0)
    {
        numEdges = (int)eMap->size();
        edges = new1<Edge2>(numEdges);
        EIterator eiter;
        for (eiter = eMap->begin(); eiter != eMap->end(); ++eiter)
        {
            edges[eiter->second] = eiter->first;
        }
        delete0(eMap);
    }
    else
    {
        edges = 0;
    }
}
//----------------------------------------------------------------------------
void Climb2D::SetLevel (float level, int depth)
{
    int offset, stride;

    for (int y = 0; y < mSize; ++y)
    {
        offset = mSize*y;
        stride = 1;
        mXMerge[y]->SetLevel(level, mData, offset, stride);
    }

    for (int x = 0; x < mSize; ++x)
    {
        offset = x;
        stride = mSize;
        mYMerge[x]->SetLevel(level, mData, offset, stride);
    }

    mXYMerge->ConstructMono(0, 0, 0, 0, 0, mTwoPowerN, depth);
}
//----------------------------------------------------------------------------
void Climb2D::GetRectangles (std::vector<Rectangle2>& rectangles)
{
    mXYMerge->GetRectangles(0, 0, 0, 0, 0, mTwoPowerN, rectangles);
}
//----------------------------------------------------------------------------
float Climb2D::GetInterp (float level, int base, int index, int increment)
{
    float f0 = (float)mData[index];
    index += increment;
    float f1 = (float)mData[index];
    assertion((f0 - level)*(f1 - level) < 0.0f, "Unexpected condition.\n");
    return (float)base + (level - f0)/(f1 - f0);
}
//----------------------------------------------------------------------------
void Climb2D::AddVertex (std::vector<Vector2f>& vertices, float x, float y)
{
    vertices.push_back(Vector2f(x, y));
}
//----------------------------------------------------------------------------
void Climb2D::AddEdge (std::vector<Vector2f>& vertices,
    std::vector<Edge2>& edges, float x0, float y0, float x1, float y1)
{
    int v0 = (int)vertices.size(), v1 = v0 + 1;
    edges.push_back(Edge2(v0, v1));
    vertices.push_back(Vector2f(x0, y0));
    vertices.push_back(Vector2f(x1, y1));
}
//----------------------------------------------------------------------------
void Climb2D::GetComponents (float level, const Rectangle2& rectangle,
    std::vector<Vector2f>& vertices, std::vector<Edge2>& edges)
{
    int x, y;
    float x0, y0, x1, y1;

    switch (rectangle.mType)
    {
    case  3:  // two vertices, on xmin and xmax
        assertion(rectangle.mYofXMin != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin;
        y = rectangle.mYofXMin;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mYofXMax != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin + rectangle.mXStride;
        y = rectangle.mYofXMax;
        x1 = (float)x;
        y1 = GetInterp(level, y, x + mSize*y, mSize);

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case  5:  // two vertices, on xmin and ymin
        assertion(rectangle.mYofXMin != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin;
        y = rectangle.mYofXMin;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mXofYMin != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMin;
        y = rectangle.mYOrigin;
        x1 = GetInterp(level, x, x + mSize*y, 1);
        y1 = (float)y;

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case  6:  // two vertices, on xmax and ymin
        assertion(rectangle.mYofXMax != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin + rectangle.mXStride;
        y = rectangle.mYofXMax;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mXofYMin != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMin;
        y = rectangle.mYOrigin;
        x1 = GetInterp(level, x, x + mSize*y, 1);
        y1 = (float)y;

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case  9:  // two vertices, on xmin and ymax
        assertion(rectangle.mYofXMin != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin;
        y = rectangle.mYofXMin;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mXofYMax != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMax;
        y = rectangle.mYOrigin + rectangle.mYStride;
        x1 = GetInterp(level, x, x + mSize*y, 1);
        y1 = (float)y;

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case 10:  // two vertices, on xmax and ymax
        assertion(rectangle.mYofXMax != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin + rectangle.mXStride;
        y = rectangle.mYofXMax;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mXofYMax != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMax;
        y = rectangle.mYOrigin + rectangle.mYStride;
        x1 = GetInterp(level, x, x + mSize*y, 1);
        y1 = (float)y;

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case 12:  // two vertices, on ymin and ymax
        assertion(rectangle.mXofYMin != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMin;
        y = rectangle.mYOrigin;
        x0 = GetInterp(level, x, x + mSize*y, 1);
        y0 = (float)y;

        assertion(rectangle.mXofYMax != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMax;
        y = rectangle.mYOrigin + rectangle.mYStride;
        x1 = GetInterp(level, x, x + mSize*y, 1);
        y1 = (float)y;

        AddEdge(vertices, edges, x0, y0, x1, y1);
        break;
    case 15:  // four vertices, one per edge, need to disambiguate
    {
        assertion(rectangle.mXStride == 1 && rectangle.mYStride == 1,
            "Unexpected condition.\n");

        assertion(rectangle.mYofXMin != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin;
        y = rectangle.mYofXMin;
        x0 = (float)x;
        y0 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mYofXMax != -1, "Unexpected condition.\n");
        x = rectangle.mXOrigin + rectangle.mXStride;
        y = rectangle.mYofXMax;
        x1 = (float)x;
        y1 = GetInterp(level, y, x + mSize*y, mSize);

        assertion(rectangle.mXofYMin != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMin;
        y = rectangle.mYOrigin;
        float fx2 = GetInterp(level,x,x+mSize*y,1);
        float fy2 = (float)y;

        assertion(rectangle.mXofYMax != -1, "Unexpected condition.\n");
        x = rectangle.mXofYMax;
        y = rectangle.mYOrigin + rectangle.mYStride;
        float fx3 = GetInterp(level, x, x + mSize*y, 1);
        float fy3 = (float)y;

        int index = rectangle.mXOrigin + mSize*rectangle.mYOrigin;
        int i00 = mData[index];
        ++index;
        int i10 = mData[index];
        index += mSize;
        int i11 = mData[index];
        --index;
        int i01 = mData[index];

        int det = i00*i11 - i01*i10;
        if (det > 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P2> and <P1,P3>.
            AddEdge(vertices, edges, x0, y0, fx2, fy2);
            AddEdge(vertices, edges, x1, y1, fx3, fy3);
        }
        else if (det < 0)
        {
            // Disjoint hyperbolic segments, pair <P0,P3> and <P1,P2>.
            AddEdge(vertices, edges, x0, y0, fx3, fy3);
            AddEdge(vertices, edges, x1, y1, fx2, fy2);
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            float fx4 = fx2, fy4 = y0;
            AddEdge(vertices, edges, x0, y0, fx4, fy4);
            AddEdge(vertices, edges, x1, y1, fx4, fy4);
            AddEdge(vertices, edges, fx2, fy2, fx4, fy4);
            AddEdge(vertices, edges, fx3, fy3, fx4, fy4);
        }
        break;
    }
    default:
        assertion(false, "Unexpected condition.\n");
    }
}
//----------------------------------------------------------------------------
void Climb2D::PrintRectangles (const char* filename,
    std::vector<Rectangle2>& rectangles)
{
    std::ofstream outFile(filename);
    const int numRectangles = (int)rectangles.size();
    for (int i = 0; i < numRectangles; ++i)
    {
        Rectangle2& rectangle = rectangles[i];
        outFile << "rectangle " << i << std::endl;
        outFile << "  x origin = " << rectangle.mXOrigin << std::endl;
        outFile << "  y origin = " << rectangle.mYOrigin << std::endl;
        outFile << "  x stride = " << rectangle.mXStride << std::endl;
        outFile << "  y stride = " << rectangle.mYStride << std::endl;
        outFile << "  flag = " << (unsigned int)rectangle.mType << std::endl;
        outFile << "  y of xmin = " << rectangle.mYofXMin << std::endl;
        outFile << "  y of xmax = " << rectangle.mYofXMax << std::endl;
        outFile << "  x of ymin = " << rectangle.mXofYMin << std::endl;
        outFile << "  x of ymax = " << rectangle.mXofYMax << std::endl;
        outFile << std::endl;
    }
}
//----------------------------------------------------------------------------

