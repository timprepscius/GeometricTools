// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2012/07/06)

#ifndef CLIMB3D_H
#define CLIMB3D_H

#include "LinearMergeTree.h"
#include "OctBox.h"
#include "MergeBox.h"
#include "VETable.h"
#include "Wm5Vector3.h"
using namespace Wm5;

class Climb3D
{
public:
    // Construction and destruction.  The array is assumed to contain 2^N+1 by
    // 2^N+1 by 2^N+1 elements where N >= 0.  The organization is
    // lexicographic order for (x,y,z).  The class assumes responsibility for
    // the input array and will delete it.
    Climb3D (int N, float* data, bool fixBoundary = false);
    ~Climb3D ();

    void ExtractContour (float level, int depth, int& numVertices,
        Vector3f*& positions, int& numTriangles, TriangleKey*& triangles);

    void MakeUnique (int& numVertices, Vector3f*& positions,
        int& numTriangles, TriangleKey*& triangles);

    void OrientTriangles (const Vector3f* positions, int numTriangles,
        TriangleKey* triangles, bool sameDirection);

    static Vector3f* ComputeNormals (int numVertices,
        const Vector3f* positions, int numTriangles,
        const TriangleKey* triangles);

    // Support for debugging.
    void PrintBoxes (const char* filename);

private:
    // Support for merging monoboxes.
    void Merge (int depth);
    bool Merge (int v, int LX, int LY, int LZ, int x0, int y0, int z0,
        int stride, int depth);

    bool DoXMerge (MergeBox& box0, MergeBox& box1, int LX, int y0, int z0);
    bool DoYMerge (MergeBox& box0, MergeBox& box1, int x0, int LY, int z0);
    bool DoZMerge (MergeBox& box0, MergeBox& box1, int x0, int y0, int LZ);
    void AddBox (int x0, int y0, int z0, int dx, int dy, int dz, int LX,
        int LY, int LZ);

    // Support for tessellating monoboxes.
    void Tessellate (std::vector<Vector3f>& positions,
        std::vector<TriangleKey>& triangles);

    float GetXInterp (int x, int y, int z) const;
    float GetYInterp (int x, int y, int z) const;
    float GetZInterp (int x, int y, int z) const;

    void GetVertices (const OctBox& box, unsigned int& type, VETable& table);

    // Edge extraction for single boxes (1x1x1).
    void GetXMinEdgesS (const OctBox& box, unsigned int type, VETable& table);
    void GetXMaxEdgesS (const OctBox& box, unsigned int type, VETable& table);
    void GetYMinEdgesS (const OctBox& box, unsigned int type, VETable& table);
    void GetYMaxEdgesS (const OctBox& box, unsigned int type, VETable& table);
    void GetZMinEdgesS (const OctBox& box, unsigned int type, VETable& table);
    void GetZMaxEdgesS (const OctBox& box, unsigned int type, VETable& table);

    // Edge extraction for merged boxes.
    class Sort0
    {
    public:
        bool operator() (const Vector3f& arg0, const Vector3f& arg1) const;
    };

    class Sort1
    {
    public:
        bool operator() (const Vector3f& arg0, const Vector3f& arg1) const;
    };

    class Sort2
    {
    public:
        bool operator() (const Vector3f& arg0, const Vector3f& arg1) const;
    };

    void GetZMinEdgesM (const OctBox& box, unsigned int type, VETable& table);
    void GetZMaxEdgesM (const OctBox& box, unsigned int type, VETable& table);
    void GetYMinEdgesM (const OctBox& box, unsigned int type, VETable& table);
    void GetYMaxEdgesM (const OctBox& box, unsigned int type, VETable& table);
    void GetXMinEdgesM (const OctBox& box, unsigned int type, VETable& table);
    void GetXMaxEdgesM (const OctBox& box, unsigned int type, VETable& table);

    // Support for normal vector calculations.
    Vector3f GetGradient (const Vector3f& position);

    enum
    {
        EI_XMIN_YMIN =  0,
        EI_XMIN_YMAX =  1,
        EI_XMAX_YMIN =  2,
        EI_XMAX_YMAX =  3,
        EI_XMIN_ZMIN =  4,
        EI_XMIN_ZMAX =  5,
        EI_XMAX_ZMIN =  6,
        EI_XMAX_ZMAX =  7,
        EI_YMIN_ZMIN =  8,
        EI_YMIN_ZMAX =  9,
        EI_YMAX_ZMIN = 10,
        EI_YMAX_ZMAX = 11,
        FI_XMIN      = 12,
        FI_XMAX      = 13,
        FI_YMIN      = 14,
        FI_YMAX      = 15,
        FI_ZMIN      = 16,
        FI_ZMAX      = 17,
        I_QUANTITY = 18,

        EB_XMIN_YMIN = 1 << EI_XMIN_YMIN,
        EB_XMIN_YMAX = 1 << EI_XMIN_YMAX,
        EB_XMAX_YMIN = 1 << EI_XMAX_YMIN,
        EB_XMAX_YMAX = 1 << EI_XMAX_YMAX,
        EB_XMIN_ZMIN = 1 << EI_XMIN_ZMIN,
        EB_XMIN_ZMAX = 1 << EI_XMIN_ZMAX,
        EB_XMAX_ZMIN = 1 << EI_XMAX_ZMIN,
        EB_XMAX_ZMAX = 1 << EI_XMAX_ZMAX,
        EB_YMIN_ZMIN = 1 << EI_YMIN_ZMIN,
        EB_YMIN_ZMAX = 1 << EI_YMIN_ZMAX,
        EB_YMAX_ZMIN = 1 << EI_YMAX_ZMIN,
        EB_YMAX_ZMAX = 1 << EI_YMAX_ZMAX,
        FB_XMIN      = 1 << FI_XMIN,
        FB_XMAX      = 1 << FI_XMAX,
        FB_YMIN      = 1 << FI_YMIN,
        FB_YMAX      = 1 << FI_YMAX,
        FB_ZMIN      = 1 << FI_ZMIN,
        FB_ZMAX      = 1 << FI_ZMAX
    };

    // image data
    int mN, mTwoPowerN, mSize, mSizeSqr;
    float* mData;
    float mLevel;

    bool mFixBoundary;

    // linear merging
    LinearMergeTree*** mXMerge;
    LinearMergeTree*** mYMerge;
    LinearMergeTree*** mZMerge;

    // monoboxes
    std::vector<OctBox> mBoxes;
};

#endif
