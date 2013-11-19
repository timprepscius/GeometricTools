// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SMUnlitEffect.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5WMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, GlobalEffect, SMUnlitEffect);
WM5_IMPLEMENT_STREAM(SMUnlitEffect);
WM5_IMPLEMENT_FACTORY(SMUnlitEffect);

//----------------------------------------------------------------------------
SMUnlitEffect::SMUnlitEffect (const std::string& effectName,
    ProjectorMatrixConstant* lightPVMatrix, ShaderFloat* lightBSMatrix,
    ShaderFloat* depthBias, ShaderFloat* texelSize, Texture2D* shadowTexture)
{
    VisualEffect* effect = VisualEffect::LoadWMFX(effectName);

    // TODO:  Once WmfxCompiler parses the Cg FX files, we will not need to
    // set the sampler state.
    PixelShader* pshader = effect->GetPixelShader(0, 0);

    // ShadowSampler
    pshader->SetFilter(0, Shader::SF_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);

    mInstance = new0 VisualEffectInstance(effect, 0);
    mInstance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    mInstance->SetVertexConstant(0, 1, new0 WMatrixConstant());
    mInstance->SetVertexConstant(0, 2, lightPVMatrix);
    mInstance->SetVertexConstant(0, 3, lightBSMatrix);
    mInstance->SetPixelConstant(0, 0, depthBias);
    mInstance->SetPixelConstant(0, 1, texelSize);
    mInstance->SetPixelTexture(0, 0, shadowTexture);
}
//----------------------------------------------------------------------------
SMUnlitEffect::~SMUnlitEffect ()
{
}
//----------------------------------------------------------------------------
void SMUnlitEffect::Draw (Renderer* renderer, const VisibleSet& visibleSet)
{
    const int numVisible = visibleSet.GetNumVisible();
    for (int j = 0; j < numVisible; ++j)
    {
        // Replace the object's effect instance by the unlit-effect instance.
        Visual* visual = (Visual*)visibleSet.GetVisible(j);
        VisualEffectInstancePtr save = visual->GetEffectInstance();
        visual->SetEffectInstance(mInstance);

        // Draw the object using the unlit effect.
        renderer->Draw(visual);

        // Restore the object's effect instance.
        visual->SetEffectInstance(save);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* SMUnlitEffect::GetObjectByName (const std::string& name)
{
    Object* found = GlobalEffect::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mInstance, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void SMUnlitEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    GlobalEffect::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mInstance, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SMUnlitEffect::SMUnlitEffect (LoadConstructor value)
    :
    GlobalEffect(value)
{
}
//----------------------------------------------------------------------------
void SMUnlitEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    GlobalEffect::Load(source);

    source.ReadPointer(mInstance);

    WM5_END_DEBUG_STREAM_LOAD(SMUnlitEffect, source);
}
//----------------------------------------------------------------------------
void SMUnlitEffect::Link (InStream& source)
{
    GlobalEffect::Link(source);

    source.ResolveLink(mInstance);
}
//----------------------------------------------------------------------------
void SMUnlitEffect::PostLink ()
{
    GlobalEffect::PostLink();
}
//----------------------------------------------------------------------------
bool SMUnlitEffect::Register (OutStream& target) const
{
    if (GlobalEffect::Register(target))
    {
        target.Register(mInstance);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void SMUnlitEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    GlobalEffect::Save(target);

    target.WritePointer(mInstance);

    WM5_END_DEBUG_STREAM_SAVE(SMUnlitEffect, target);
}
//----------------------------------------------------------------------------
int SMUnlitEffect::GetStreamingSize () const
{
    int size = GlobalEffect::GetStreamingSize();
    size += WM5_POINTERSIZE(mInstance);
    return size;
}
//----------------------------------------------------------------------------
