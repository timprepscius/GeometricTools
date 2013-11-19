// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef VETABLE_H
#define VETABLE_H

#include "Wm5Vector3.h"
#include "Wm5TriangleKey.h"
using namespace Wm5;

typedef std::pair<int,int> Edge3;

class VETable
{
public:
    // Construction and destruction.
    VETable ();
    ~VETable ();

    // Member access.
    bool IsValidVertex (int i) const;
    int GetNumVertices () const;
    const Vector3f& GetPosition (int i) const;

    // Mesh insertions and removals.
    void Insert (int i, float x, float y, float z);
    void Insert (const Vector3f& position);
    void Insert (const Edge3& edge);

    void RemoveTrianglesEC (std::vector<Vector3f>& positions,
        std::vector<TriangleKey>& triangles);

    void RemoveTrianglesSE (std::vector<Vector3f>& positions,
        std::vector<TriangleKey>& triangles);

protected:
    void RemoveVertex (int i);
    bool RemoveEC (TriangleKey& triangle);  // ear clipping

    class Vertex
    {
    public:
        Vertex ();
        Vertex (const Vector3f& position);

        Vector3f mPosition;
        int mAdjQuantity;
        int mAdjacent[4];
        bool mValid;
    };

    std::vector<Vertex> mVertices;
};

#endif

