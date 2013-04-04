// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/04)

#include "BlendedTerrainEffect.h"
#include "Wm5Environment.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5Texture1D.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, BlendedTerrainEffect);
WM5_IMPLEMENT_STREAM(BlendedTerrainEffect);
WM5_IMPLEMENT_FACTORY(BlendedTerrainEffect);

//----------------------------------------------------------------------------
BlendedTerrainEffect::BlendedTerrainEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = GetPixelShader(0, 0);

    // GrassSampler
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(0, 1, Shader::SC_REPEAT);

    // StoneSampler
    pshader->SetFilter(1, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(1, 1, Shader::SC_REPEAT);

    // BlendSampler
    pshader->SetFilter(2, Shader::SF_LINEAR);
    pshader->SetCoordinate(2, 0, Shader::SC_REPEAT);

    // CloudSampler
    pshader->SetFilter(3, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(3, 0, Shader::SC_REPEAT);
    pshader->SetCoordinate(3, 1, Shader::SC_REPEAT);

    // Create a 1-dimensional texture whose intensities are proportional to
    // height.
    const int numTexels = 256;
    mBlendTexture = new0 Texture1D(Texture::TF_L8, numTexels, 1);
    unsigned char* data = (unsigned char*)mBlendTexture->GetData(0);
    for (int i = 0; i < numTexels; ++i)
    {
        *data++ = (unsigned char)i;
    }
}
//----------------------------------------------------------------------------
BlendedTerrainEffect::~BlendedTerrainEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* BlendedTerrainEffect::CreateInstance (
    ShaderFloat* flowDirection, ShaderFloat* powerFactor,
    Texture2D* grassTexture, Texture2D* stoneTexture, Texture2D* cloudTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1, flowDirection);
    instance->SetPixelConstant(0, 0, powerFactor);
    instance->SetPixelTexture(0, 0, grassTexture);
    instance->SetPixelTexture(0, 1, stoneTexture);
    instance->SetPixelTexture(0, 2, mBlendTexture);
    instance->SetPixelTexture(0, 3, cloudTexture);
    return instance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* BlendedTerrainEffect::GetObjectByName (const std::string& name)
{
    Object* found = VisualEffect::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mBlendTexture, name, found);

    return 0;
}
//----------------------------------------------------------------------------
void BlendedTerrainEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    VisualEffect::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mBlendTexture, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
BlendedTerrainEffect::BlendedTerrainEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void BlendedTerrainEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    source.ReadPointer(mBlendTexture);

    WM5_END_DEBUG_STREAM_LOAD(BlendedTerrainEffect, source);
}
//----------------------------------------------------------------------------
void BlendedTerrainEffect::Link (InStream& source)
{
    VisualEffect::Link(source);

    source.ResolveLink(mBlendTexture);
}
//----------------------------------------------------------------------------
void BlendedTerrainEffect::PostLink ()
{
    VisualEffect::PostLink();

    //VisualPass* pass = mTechniques[0]->GetPass(0);
    //VertexShader* vshader = pass->GetVertexShader();
    //PixelShader* pshader = pass->GetPixelShader();
    //vshader->SetBaseRegisters(msVRegisters);
    //vshader->SetPrograms(msVPrograms);
    //pshader->SetBaseRegisters(msPRegisters);
    //pshader->SetTextureUnits(msPTextureUnits);
    //pshader->SetPrograms(msPPrograms);
}
//----------------------------------------------------------------------------
bool BlendedTerrainEffect::Register (OutStream& target) const
{
    if (VisualEffect::Register(target))
    {
        target.Register(mBlendTexture);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void BlendedTerrainEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    target.WritePointer(mBlendTexture);

    WM5_END_DEBUG_STREAM_SAVE(BlendedTerrainEffect, target);
}
//----------------------------------------------------------------------------
int BlendedTerrainEffect::GetStreamingSize () const
{
    int size = VisualEffect::GetStreamingSize();
    size += WM5_POINTERSIZE(mBlendTexture);
    return size;
}
//----------------------------------------------------------------------------
