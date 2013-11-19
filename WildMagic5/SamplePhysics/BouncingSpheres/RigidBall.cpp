// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RigidBall.h"
#include "Wm5Environment.h"
#include "Wm5StandardMesh.h"
#include "Wm5Texture2DEffect.h"

//----------------------------------------------------------------------------
RigidBall::RigidBall (float radius)
    :
    Moved(false),
    mRadius(radius)
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mMesh = StandardMesh(vformat).Sphere(16, 16, mRadius);

    std::string path = Environment::GetPathR("BallTexture.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mMesh->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));
}
//----------------------------------------------------------------------------
float RigidBall::GetRadius () const
{ 
    return mRadius; 
}
//----------------------------------------------------------------------------
TriMeshPtr& RigidBall::Mesh ()
{
    return mMesh;
}
//----------------------------------------------------------------------------
