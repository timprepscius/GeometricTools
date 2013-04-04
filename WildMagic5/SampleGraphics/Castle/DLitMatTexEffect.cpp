// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "DLitMatTexEffect.h"
#include "Wm5CameraModelPositionConstant.h"
#include "Wm5MaterialEmissiveConstant.h"
#include "Wm5MaterialAmbientConstant.h"
#include "Wm5MaterialSpecularConstant.h"
#include "Wm5LightAmbientConstant.h"
#include "Wm5LightDiffuseConstant.h"
#include "Wm5LightSpecularConstant.h"
#include "Wm5LightAttenuationConstant.h"
#include "Wm5LightModelDVectorConstant.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, DLitMatTexEffect);
WM5_IMPLEMENT_STREAM(DLitMatTexEffect);
WM5_IMPLEMENT_FACTORY(DLitMatTexEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, DLitMatTexEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, DLitMatTexEffect);

//----------------------------------------------------------------------------
DLitMatTexEffect::DLitMatTexEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // DiffuseSampler
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(0, 1, Shader::SC_REPEAT);
}
//----------------------------------------------------------------------------
DLitMatTexEffect::~DLitMatTexEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* DLitMatTexEffect::CreateInstance (
    Light* directionalLight, Material* material, Texture2D* diffuseTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0,
        new0 PVWMatrixConstant());
    instance->SetPixelConstant(0, 0,
        new0 CameraModelPositionConstant());
    instance->SetPixelConstant(0, 1,
        new0 MaterialAmbientConstant(material));
    instance->SetPixelConstant(0, 2,
        new0 MaterialSpecularConstant(material));
    instance->SetPixelConstant(0, 3,
        new0 LightModelDVectorConstant(directionalLight));
    instance->SetPixelConstant(0, 4,
        new0 LightAmbientConstant(directionalLight));
    instance->SetPixelTexture(0, 0, diffuseTexture);
    return instance;
}
//----------------------------------------------------------------------------
