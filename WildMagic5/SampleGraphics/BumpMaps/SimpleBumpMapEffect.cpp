// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SimpleBumpMapEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, SimpleBumpMapEffect);
WM5_IMPLEMENT_STREAM(SimpleBumpMapEffect);
WM5_IMPLEMENT_FACTORY(SimpleBumpMapEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, SimpleBumpMapEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, SimpleBumpMapEffect);

//----------------------------------------------------------------------------
SimpleBumpMapEffect::SimpleBumpMapEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // BaseSampler
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(0, 1, Shader::SC_REPEAT);

    // NormalSampler
    pshader->SetFilter(1, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(1, 1, Shader::SC_REPEAT);
}
//----------------------------------------------------------------------------
SimpleBumpMapEffect::~SimpleBumpMapEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* SimpleBumpMapEffect::CreateInstance (
    Texture2D* baseTexture, Texture2D* normalTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetPixelTexture(0, 0, baseTexture);
    instance->SetPixelTexture(0, 1, normalTexture);
    return instance;
}
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::ComputeLightVectors (Triangles* mesh,
    const AVector& worldLightDirection)
{
    // The tangent-space coordinates for the light direction vector at each
    // vertex is stored in the color0 channel.  The computations use the
    // vertex normals and the texture coordinates for the base mesh, which
    // are stored in the tcoord0 channel.  Thus, the mesh must have positions,
    // normals, colors (unit 0), and texture coordinates (unit 0).

    // The light direction D is in world-space coordinates.  Negate it,
    // transform it to model-space coordinates, and then normalize it.  The
    // world-space direction is unit-length, but the geometric primitive
    // might have non-unit scaling in its model-to-world transformation, in
    // which case the normalization is necessary.
    AVector modelLightDirection =
        -mesh->WorldTransform.Inverse()*worldLightDirection;

    // Set the light vectors to (0,0,0) as a flag that the quantity has not
    // yet been computed.  The probability that a light vector is actually
    // (0,0,0) should be small, so the flag system should save computation
    // time overall.
    VertexBufferAccessor vba(mesh);
    Float3 black(0.0f, 0.0f, 0.0f);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = black;
    }

    int numTriangles = mesh->GetNumTriangles();
    for (int t = 0; t < numTriangles; ++t)
    {
        // Get the triangle vertices and attributes.
        int v0, v1, v2;
        if (!mesh->GetTriangle(t, v0, v1, v2))
        {
            continue;
        }

        APoint position[3] =
        {
            vba.Position<Float3>(v0),
            vba.Position<Float3>(v1),
            vba.Position<Float3>(v2)
        };

        AVector normal[3] =
        {
            vba.Normal<Float3>(v0),
            vba.Normal<Float3>(v1),
            vba.Normal<Float3>(v2)
        };

        Float3* color[3] =
        {
            &vba.Color<Float3>(0, v0),
            &vba.Color<Float3>(0, v1),
            &vba.Color<Float3>(0, v2)
        };

        Float2 tcoord[3] =
        {
            vba.TCoord<Float2>(0, v0),
            vba.TCoord<Float2>(0, v1),
            vba.TCoord<Float2>(0, v2)
        };

        for (i = 0; i < 3; ++i)
        {
            Float3& colorref = *color[i];
            if (colorref != black)
            {
                continue;
            }

            int iP = (i == 0) ? 2 : i - 1;
            int iN = (i + 1) % 3;

            AVector tangent;
            if (!ComputeTangent(position[i], tcoord[i], position[iN],
                tcoord[iN], position[iP], tcoord[iP], tangent))
            {
                // The texture coordinate mapping is not properly defined for
                // this.  Just say that the tangent space light vector points
                // in the same direction as the surface normal.
                colorref[0] = normal[i][0];
                colorref[1] = normal[i][1];
                colorref[2] = normal[i][2];
                continue;
            }

            // Project T into the tangent plane by projecting out the surface
            // normal N, and then make it unit length.
            tangent -= normal[i].Dot(tangent)*(normal[i]);
            tangent.Normalize();

            // Compute the bitangent B, another tangent perpendicular to T.
            AVector bitangent = normal[i].UnitCross(tangent);

            // The set {T,B,N} is a right-handed orthonormal set.  The
            // negated light direction U = -D is represented in this
            // coordinate system as
            //   U = Dot(U,T)*T + Dot(U,B)*B + Dot(U,N)*N
            float dotUT = modelLightDirection.Dot(tangent);
            float dotUB = modelLightDirection.Dot(bitangent);
            float dotUN = modelLightDirection.Dot(normal[i]);

            // Transform the light vector into [0,1]^3 to make it a valid
            // Float3 object.
            colorref[0] = 0.5f*(dotUT + 1.0f);
            colorref[1] = 0.5f*(dotUB + 1.0f);
            colorref[2] = 0.5f*(dotUN + 1.0f);
        }
    }
}
//----------------------------------------------------------------------------
bool SimpleBumpMapEffect::ComputeTangent (const APoint& position0,
    const Float2& tcoord0, const APoint& position1, const Float2& tcoord1,
    const APoint& position2, const Float2& tcoord2, AVector& tangent)
{
    // Compute the change in positions at the vertex P0.
    AVector deltaPos1 = position1 - position0;
    AVector deltaPos2 = position2 - position0;

    if (Mathf::FAbs(deltaPos1.Length()) < Mathf::ZERO_TOLERANCE
    ||  Mathf::FAbs(deltaPos2.Length()) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle is very small, call it degenerate.
        return false;
    }

    // Compute the change in texture coordinates at the vertex P0 in the
    // direction of edge P1-P0.
    float du1 = tcoord1[0] - tcoord0[0];
    float dv1 = tcoord1[1] - tcoord0[1];
    if (Mathf::FAbs(dv1) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle effectively has no variation in the v texture
        // coordinate.
        if (Mathf::FAbs(du1) < Mathf::ZERO_TOLERANCE)
        {
            // The triangle effectively has no variation in the u coordinate.
            // Since the texture coordinates do not vary on this triangle,
            // treat it as a degenerate parametric surface.
            return false;
        }

        // The variation is effectively all in u, so set the tangent vector
        // to be T = dP/du.
        tangent = deltaPos1/du1;
        return true;
    }

    // Compute the change in texture coordinates at the vertex P0 in the
    // direction of edge P2-P0.
    float du2 = tcoord2[0] - tcoord0[0];
    float dv2 = tcoord2[1] - tcoord0[1];
    float det = dv1*du2 - dv2*du1;
    if (Mathf::FAbs(det) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle vertices are collinear in parameter space, so treat
        // this as a degenerate parametric surface.
        return false;
    }

    // The triangle vertices are not collinear in parameter space, so choose
    // the tangent to be dP/du = (dv1*dP2-dv2*dP1)/(dv1*du2-dv2*du1)
    tangent = (dv1*deltaPos2 - dv2*deltaPos1)/det;
    return true;
}
//----------------------------------------------------------------------------
