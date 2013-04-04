// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef VOLUMEFOGEFFECT_H
#define VOLUMEFOGEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class VolumeFogEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VolumeFogEffect);

public:
    // Construction and destruction.
    VolumeFogEffect (const std::string& effectFile);
    virtual ~VolumeFogEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture2D* baseTexture);
};

WM5_REGISTER_STREAM(VolumeFogEffect);
typedef Pointer0<VolumeFogEffect> VolumeFogEffectPtr;

}

#endif
