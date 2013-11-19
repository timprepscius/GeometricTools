// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RigidTetra.h"
#include "Wm5Float3.h"
#include "Wm5VertexBufferAccessor.h"
#include "Wm5VertexColor3Effect.h"

//----------------------------------------------------------------------------
RigidTetra::RigidTetra (float size, float mass, const Vector3f& position,
    const Vector3f& linearMomentum, const Vector3f& angularMomentum)
    :
    Moved(false)
{
    Moved = false;

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = -(size/3.0f)*Vector3f(1.0f, 1.0f, 1.0f);
    vba.Position<Vector3f>(1) = Vector3f(size, 0.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(0.0f, size, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(0.0f, 0.0f, size);
    vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 1.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 2) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0, 3) = Float3(0.0f, 0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(12, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0; indices[ 1] = 2; indices[ 2] = 1;
    indices[ 3] = 0; indices[ 4] = 3; indices[ 5] = 2;
    indices[ 6] = 0; indices[ 7] = 1; indices[ 8] = 3;
    indices[ 9] = 1; indices[10] = 2; indices[11] = 3;

    mMesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mMesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    // Compute the inertia tensor.
    Matrix3f inertia;
    int j;
    for (int i = 0; i < 3; ++i)
    {
        inertia[i][i] = 0.0f;
        for (j = 0; j < 3; ++j)
        {
            if (i != j)
            {
                inertia[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    Vector3f pos = vba.Position<Vector3f>(k);
                    inertia[i][i] += 0.25f*mass*pos[j]*pos[j];
                    inertia[i][j] -= 0.25f*mass*pos[i]*pos[j];
                }
            }
        }
    }

    // Compute the radius of a sphere bounding the tetrahedron.
    Vector3f centroid = (size/6.0f)*Vector3f(1.0f, 1.0f, 1.0f);
    mRadius = 0.0f;
    for (j = 0; j < 4; ++j)
    {
        vba.Position<Vector3f>(j) -= centroid;
        float temp = (vba.Position<Vector3f>(j) - centroid).Length();
        if (temp > mRadius)
        {
            mRadius = temp;
        }
    }

    SetMass(mass);
    SetBodyInertia(inertia);
    SetPosition(position);
    SetQOrientation(Quaternionf::IDENTITY);
    SetLinearMomentum(linearMomentum);
    SetAngularMomentum(angularMomentum);
}
//----------------------------------------------------------------------------
TriMeshPtr RigidTetra::Mesh () const
{
    return mMesh;
}
//----------------------------------------------------------------------------
float RigidTetra::GetRadius () const
{
    return mRadius;
}
//----------------------------------------------------------------------------
void RigidTetra::GetVertices (Vector3f* vertices) const
{
    // Do not move the boundaries.  The hard-coded numbers here depend on
    // those of the floor/walls in the application.
    Vector3f offset;
    if (0.0f < mPosition.X() && mPosition.X() < 20.0f)
    {
        offset = mPosition;
    }
    else
    {
        offset = Vector3f::ZERO;
    }

    // Move the tetra vertices.
    VertexBufferAccessor vba(mMesh);
    for (int i = 0; i < 4; ++i)
    {
        vertices[i] = mRotOrient*vba.Position<Vector3f>(i) + offset;
    }
}
//----------------------------------------------------------------------------
