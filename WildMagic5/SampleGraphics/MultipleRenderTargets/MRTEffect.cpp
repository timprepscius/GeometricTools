// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MRTEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, MRTEffect);
WM5_IMPLEMENT_STREAM(MRTEffect);
WM5_IMPLEMENT_FACTORY(MRTEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, MRTEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, MRTEffect);

//----------------------------------------------------------------------------
MRTEffect::MRTEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // Sampler0
    pshader->SetFilter(0, Shader::SF_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);

    // Sampler1
    pshader->SetFilter(1, Shader::SF_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(1, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
MRTEffect::~MRTEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* MRTEffect::CreateInstance (Texture2D* texture0,
    Texture2D* texture1)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetPixelTexture(0, 0, texture0);
    instance->SetPixelTexture(0, 1, texture1);
    return instance;
}
//----------------------------------------------------------------------------
