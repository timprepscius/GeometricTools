// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ProjectedTextureEffect.h"
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
#include "Wm5ProjectorMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, ProjectedTextureEffect);
WM5_IMPLEMENT_STREAM(ProjectedTextureEffect);
WM5_IMPLEMENT_FACTORY(ProjectedTextureEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, ProjectedTextureEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, ProjectedTextureEffect);

//----------------------------------------------------------------------------
ProjectedTextureEffect::ProjectedTextureEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // ProjectedSampler
    pshader->SetFilter(0, Shader::SF_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
ProjectedTextureEffect::~ProjectedTextureEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* ProjectedTextureEffect::CreateInstance (
    Projector* projector, Light* light, Material* material,
    Texture2D* projectedTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1,
        new0 ProjectorMatrixConstant(projector, true, 1));
    instance->SetVertexConstant(0, 2, new0 CameraModelPositionConstant());
    instance->SetVertexConstant(0, 3,
        new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 4, new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 5, new0 MaterialDiffuseConstant(material));
    instance->SetVertexConstant(0, 6,
        new0 MaterialSpecularConstant(material));
    instance->SetVertexConstant(0, 7, new0 LightModelDVectorConstant(light));
    instance->SetVertexConstant(0, 8, new0 LightAmbientConstant(light));
    instance->SetVertexConstant(0, 9, new0 LightDiffuseConstant(light));
    instance->SetVertexConstant(0, 10, new0 LightSpecularConstant(light));
    instance->SetPixelTexture(0, 0, projectedTexture);
    return instance;
}
//----------------------------------------------------------------------------
