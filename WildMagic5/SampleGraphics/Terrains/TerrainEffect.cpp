// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "TerrainEffect.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5VWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, TerrainEffect);
WM5_IMPLEMENT_STREAM(TerrainEffect);
WM5_IMPLEMENT_FACTORY(TerrainEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, TerrainEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, TerrainEffect);

//----------------------------------------------------------------------------
TerrainEffect::TerrainEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // BaseSampler
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);

    // DetailSampler
    pshader->SetFilter(1, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(1, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
TerrainEffect::~TerrainEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* TerrainEffect::CreateInstance (Texture2D* baseTexture,
    Texture2D* detailTexture, ShaderFloat* fogColorDensity)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, new0 VWMatrixConstant());
    instance->SetVertexConstant(0, 2, fogColorDensity);
    instance->SetPixelTexture(0, 0, baseTexture);
    instance->SetPixelTexture(0, 1, detailTexture);
    return instance;
}
//----------------------------------------------------------------------------
