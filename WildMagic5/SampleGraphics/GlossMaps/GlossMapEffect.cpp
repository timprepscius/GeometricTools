// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GlossMapEffect.h"
#include "Wm5CameraModelPositionConstant.h"
#include "Wm5MaterialEmissiveConstant.h"
#include "Wm5MaterialAmbientConstant.h"
#include "Wm5MaterialDiffuseConstant.h"
#include "Wm5MaterialSpecularConstant.h"
#include "Wm5LightAmbientConstant.h"
#include "Wm5LightDiffuseConstant.h"
#include "Wm5LightSpecularConstant.h"
#include "Wm5LightModelDVectorConstant.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, GlossMapEffect);
WM5_IMPLEMENT_STREAM(GlossMapEffect);
WM5_IMPLEMENT_FACTORY(GlossMapEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, GlossMapEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, GlossMapEffect);

//----------------------------------------------------------------------------
GlossMapEffect::GlossMapEffect (const std::string& effectFile)
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
GlossMapEffect::~GlossMapEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* GlossMapEffect::CreateInstance (Texture2D* texture,
    Light* light, Material* material)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, new0 CameraModelPositionConstant());
    instance->SetVertexConstant(0, 2, new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 3, new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 4, new0 MaterialDiffuseConstant(material));
    instance->SetVertexConstant(0, 5, new0 MaterialSpecularConstant(material));
    instance->SetVertexConstant(0, 6, new0 LightModelDVectorConstant(light));
    instance->SetVertexConstant(0, 7, new0 LightAmbientConstant(light));
    instance->SetVertexConstant(0, 8, new0 LightDiffuseConstant(light));
    instance->SetVertexConstant(0, 9, new0 LightSpecularConstant(light));
    instance->SetPixelTexture(0, 0, texture);
    return instance;
}
//----------------------------------------------------------------------------
