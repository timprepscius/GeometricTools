// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "VolumeFogEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, VolumeFogEffect);
WM5_IMPLEMENT_STREAM(VolumeFogEffect);
WM5_IMPLEMENT_FACTORY(VolumeFogEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, VolumeFogEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, VolumeFogEffect);

//----------------------------------------------------------------------------
VolumeFogEffect::VolumeFogEffect (const std::string& effectFile)
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
VolumeFogEffect::~VolumeFogEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* VolumeFogEffect::CreateInstance (Texture2D* baseTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetPixelTexture(0, 0, baseTexture);
    return instance;
}
//----------------------------------------------------------------------------
