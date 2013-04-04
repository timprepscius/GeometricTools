// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SMShadowEffect.h"
#include "Wm5WMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, GlobalEffect, SMShadowEffect);
WM5_IMPLEMENT_STREAM(SMShadowEffect);
WM5_IMPLEMENT_FACTORY(SMShadowEffect);

//----------------------------------------------------------------------------
SMShadowEffect::SMShadowEffect (const std::string& effectName,
    ShaderFloat* lightPVMatrix)
{
    VisualEffect* effect = VisualEffect::LoadWMFX(effectName);

    mInstance = new0 VisualEffectInstance(effect, 0);
    mInstance->SetVertexConstant(0, 0, new0 WMatrixConstant());
    mInstance->SetVertexConstant(0, 1, lightPVMatrix);
}
//----------------------------------------------------------------------------
SMShadowEffect::~SMShadowEffect ()
{
}
//----------------------------------------------------------------------------
void SMShadowEffect::Draw (Renderer* renderer, const VisibleSet& visibleSet)
{
    const int numVisible = visibleSet.GetNumVisible();
    for (int j = 0; j < numVisible; ++j)
    {
        // Replace the object's effect instance by the shadow-effect instance.
        Visual* visual = (Visual*)visibleSet.GetVisible(j);
        VisualEffectInstancePtr save = visual->GetEffectInstance();
        visual->SetEffectInstance(mInstance);

        // Draw the object using the shadow effect.
        renderer->Draw(visual);

        // Restore the object's effect instance.
        visual->SetEffectInstance(save);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* SMShadowEffect::GetObjectByName (const std::string& name)
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
void SMShadowEffect::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    GlobalEffect::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mInstance, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SMShadowEffect::SMShadowEffect (LoadConstructor value)
    :
    GlobalEffect(value)
{
}
//----------------------------------------------------------------------------
void SMShadowEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    GlobalEffect::Load(source);

    source.ReadPointer(mInstance);

    WM5_END_DEBUG_STREAM_LOAD(SMShadowEffect, source);
}
//----------------------------------------------------------------------------
void SMShadowEffect::Link (InStream& source)
{
    GlobalEffect::Link(source);

    source.ResolveLink(mInstance);
}
//----------------------------------------------------------------------------
void SMShadowEffect::PostLink ()
{
    GlobalEffect::PostLink();
}
//----------------------------------------------------------------------------
bool SMShadowEffect::Register (OutStream& target) const
{
    if (GlobalEffect::Register(target))
    {
        target.Register(mInstance);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void SMShadowEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    GlobalEffect::Save(target);

    target.WritePointer(mInstance);

    WM5_END_DEBUG_STREAM_SAVE(SMShadowEffect, target);
}
//----------------------------------------------------------------------------
int SMShadowEffect::GetStreamingSize () const
{
    int size = GlobalEffect::GetStreamingSize();
    size += WM5_POINTERSIZE(mInstance);
    return size;
}
//----------------------------------------------------------------------------
