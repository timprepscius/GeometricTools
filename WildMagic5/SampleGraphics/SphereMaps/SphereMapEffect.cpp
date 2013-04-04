// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SphereMapEffect.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5VWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, SphereMapEffect);
WM5_IMPLEMENT_STREAM(SphereMapEffect);
WM5_IMPLEMENT_FACTORY(SphereMapEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, SphereMapEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, SphereMapEffect);

//----------------------------------------------------------------------------
SphereMapEffect::SphereMapEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // BaseSampler
    pshader->SetFilter(0, Shader::SF_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
SphereMapEffect::~SphereMapEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* SphereMapEffect::CreateInstance (
    Texture2D* environmentTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, new0 VWMatrixConstant());
    instance->SetPixelTexture(0, 0, environmentTexture);
    return instance;
}
//----------------------------------------------------------------------------
