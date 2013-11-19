// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SKINNINGEFFECT_H
#define SKINNINGEFFECT_H

#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class SkinningEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SkinningEffect);

public:
    // Construction and destruction.
    SkinningEffect (const std::string& effectFile);
    virtual ~SkinningEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (ShaderFloat* skinningMatrix[4]);
};

WM5_REGISTER_STREAM(SkinningEffect);
typedef Pointer0<SkinningEffect> SkinningEffectPtr;

}

#endif
