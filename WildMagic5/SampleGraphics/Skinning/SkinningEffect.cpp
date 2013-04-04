// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SkinningEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, SkinningEffect);
WM5_IMPLEMENT_STREAM(SkinningEffect);
WM5_IMPLEMENT_FACTORY(SkinningEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, SkinningEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, SkinningEffect);

//----------------------------------------------------------------------------
SkinningEffect::SkinningEffect (const std::string& effectFile)
    :
    VisualEffect(effectFile)
{
}
//----------------------------------------------------------------------------
SkinningEffect::~SkinningEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* SkinningEffect::CreateInstance (
    ShaderFloat* skinningMatrix[4])
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    for (int i = 0; i < 4; ++i)
    {
        instance->SetVertexConstant(0, i + 1, skinningMatrix[i]);
    }
    return instance;
}
//----------------------------------------------------------------------------
