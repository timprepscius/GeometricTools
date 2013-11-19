// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SMSceneEffect.h"
#include "Wm5CameraWorldPositionConstant.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5WMatrixConstant.h"
using namespace Wm5;

//----------------------------------------------------------------------------
SMSceneEffect::SMSceneEffect (const std::string& effectFile)
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

    // BlurSampler
    pshader->SetFilter(1, Shader::SF_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(1, 1, Shader::SC_CLAMP_EDGE);

    // SpotSampler
    pshader->SetFilter(2, Shader::SF_LINEAR);
    pshader->SetCoordinate(2, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(2, 1, Shader::SC_CLAMP_EDGE);
}
//----------------------------------------------------------------------------
SMSceneEffect::~SMSceneEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* SMSceneEffect::CreateInstance (
    ProjectorWorldPositionConstant* lightWorldPosition,
    ProjectorMatrixConstant* lightPVMatrix, ShaderFloat* lightBSMatrix,
    ShaderFloat* screenBSMatrix, ShaderFloat* lightColor,
    Texture2D* baseTexture, Texture2D* blurTexture,
    Texture2D* projectedTexture) const
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, new0 WMatrixConstant());
    instance->SetVertexConstant(0, 2, lightPVMatrix);
    instance->SetVertexConstant(0, 3, lightBSMatrix);
    instance->SetVertexConstant(0, 4, screenBSMatrix);
    instance->SetVertexConstant(0, 5, lightWorldPosition);
    instance->SetVertexConstant(0, 6, new0 CameraWorldPositionConstant());
    instance->SetPixelConstant(0, 0, lightColor);
    instance->SetPixelTexture(0, 0, baseTexture);
    instance->SetPixelTexture(0, 1, blurTexture);
    instance->SetPixelTexture(0, 2, projectedTexture);
    return instance;
}
//----------------------------------------------------------------------------
