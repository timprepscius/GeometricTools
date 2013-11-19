// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef RIGIDTETRA_H
#define RIGIDTETRA_H

#include "Wm5RigidBody.h"
#include "Wm5TriMesh.h"
using namespace Wm5;

class RigidTetra : public RigidBodyf
{
public:
    RigidTetra (float size, float mass, const Vector3f& position,
        const Vector3f& linearMomentum, const Vector3f& angularMomentum);

    TriMeshPtr Mesh () const;
    float GetRadius () const;
    void GetVertices (Vector3f* akVertex) const;

    bool Moved;

private:
    TriMeshPtr mMesh;
    float mRadius;
};

#endif
