// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/09/08)

#include "CubeMapEffect.h"
#include "Wm5CameraWorldPositionConstant.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5ScreenTarget.h"
#include "Wm5TextureCube.h"
#include "Wm5Utility.h"
#include "Wm5WMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, CubeMapEffect);
WM5_IMPLEMENT_STREAM(CubeMapEffect);
WM5_IMPLEMENT_FACTORY(CubeMapEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, CubeMapEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, CubeMapEffect);

CubeMapEffect::UpdateMap* CubeMapEffect::msUpdates = 0;

//----------------------------------------------------------------------------
CubeMapEffect::CubeMapEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // CubeMapSampler
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
CubeMapEffect::~CubeMapEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* CubeMapEffect::CreateInstance (TextureCube* texture,
    ShaderFloat* reflectivity, bool allowDynamicUpdates)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, new0 WMatrixConstant());
    instance->SetVertexConstant(0, 2, new0 CameraWorldPositionConstant());
    instance->SetPixelConstant(0, 0, reflectivity);
    instance->SetPixelTexture(0, 0, texture);

    if (allowDynamicUpdates)
    {
        DynamicUpdate update;

        update.DUCamera = new0 Camera();

        VertexFormat* vformat = VertexFormat::Create(2,
            VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
            VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

        int rtSize = texture->GetWidth();
        update.DUSquare = ScreenTarget::CreateRectangle(vformat, rtSize,
            rtSize, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

        update.DUTarget = new0 RenderTarget(1, texture->GetFormat(), rtSize,
            rtSize, false, true);

        if (!msUpdates)
        {
            msUpdates = new0 UpdateMap();
        }
        msUpdates->insert(std::make_pair(instance, update));
    }

    return instance;
}
//----------------------------------------------------------------------------
void CubeMapEffect::DestroyUpdateMap ()
{
    delete0(msUpdates);
}
//----------------------------------------------------------------------------
bool CubeMapEffect::AllowDynamicUpdates (VisualEffectInstance* instance)
{
    if (msUpdates)
    {
        return msUpdates->find(instance) != msUpdates->end();
    }
    return false;
}
//----------------------------------------------------------------------------
void CubeMapEffect::UpdateFaces (VisualEffectInstance* instance,
    Renderer* renderer, Spatial* scene, const APoint& envOrigin,
    const AVector& envDVector, const AVector& envUVector,
    const AVector& envRVector)
{
    // TODO.  Implement cube-map render targets to avoid the expensive copies
    // of render target textures to the cube texture.

    if (!msUpdates)
    {
        // No update instances exist.
        return;
    }

    UpdateMap::iterator iter = msUpdates->find(instance);
    if (iter == msUpdates->end())
    {
        // Dynamic updates were not enabled in the CreateInstance call.
        return;
    }

    DynamicUpdate& update = iter->second;
    TextureCube* cubeTexture = (TextureCube*)instance->GetPixelTexture(0, 0);

    AVector dVector[6] =
    {
        -envRVector,
        envRVector,
        envUVector,
        -envUVector,
        envDVector,
        -envDVector
    };

    AVector uVector[6] =
    {
        envUVector,
        envUVector,
        -envDVector,
        envDVector,
        envUVector,
        envUVector
    };

    AVector rVector[6] =
    {
        envDVector,
        -envDVector,
        envRVector,
        envRVector,
        envRVector,
        -envRVector
    };

    // The camera is oriented six times along the coordinate axes and using
    // a frustum with a 90-degree field of view and an aspect ratio of 1 (the
    // cube faces are squares).  Save the current camera for restoration at
    // the end of this function.
    CameraPtr camera = renderer->GetCamera();
    update.DUCamera->SetFrustum(90.0f, 1.0f, camera->GetDMin(),
        camera->GetDMax());
    update.DUCuller.SetCamera(update.DUCamera);
    renderer->SetCamera(update.DUCamera);

    int faceSize = cubeTexture->GetWidth();
    int pixelSize = cubeTexture->GetPixelSize();
    Texture2D* texture = 0;
    for (int face = 0; face < 6; ++face)
    {
        update.DUCamera->SetFrame(envOrigin, dVector[face], uVector[face],
            rVector[face]);
        update.DUCuller.ComputeVisibleSet(scene);

        renderer->Enable(update.DUTarget);
        renderer->ClearBuffers();
        renderer->Draw(update.DUCuller.GetVisibleSet());
        renderer->Disable(update.DUTarget);

        renderer->ReadColor(0, update.DUTarget, texture);
        char* cubeData = cubeTexture->GetData(face, 0);
        char* textData = texture->GetData(0);
        int numBytes = texture->GetNumLevelBytes(0);
        Utility::ReflectX(faceSize, faceSize, pixelSize, textData);
        if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
        {
            Utility::ReflectY(faceSize, faceSize, pixelSize, textData);
        }
        memcpy(cubeData, textData, numBytes);
    }
    delete0(texture);

    // Request a refresh of VRAM with the new cube map.
    renderer->Update(cubeTexture, 0, 0);
    renderer->Update(cubeTexture, 1, 0);
    renderer->Update(cubeTexture, 2, 0);
    renderer->Update(cubeTexture, 3, 0);
    renderer->Update(cubeTexture, 4, 0);
    renderer->Update(cubeTexture, 5, 0);
    cubeTexture->GenerateMipmaps();

    // Restore the original camera and propagate the information to the
    // renderer.
    renderer->SetCamera(camera);
}
//----------------------------------------------------------------------------
