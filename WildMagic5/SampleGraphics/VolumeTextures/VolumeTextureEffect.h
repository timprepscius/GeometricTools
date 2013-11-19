// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef VOLUMETEXTUREEFFECT_H
#define VOLUMETEXTUREEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Texture3D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class VolumeTextureEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(VolumeTextureEffect);

public:
    // Construction and destruction.
    VolumeTextureEffect (const std::string& effectFile);
    virtual ~VolumeTextureEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture3D* texture);
};

WM5_REGISTER_STREAM(VolumeTextureEffect);
typedef Pointer0<VolumeTextureEffect> VolumeTextureEffectPtr;

}

#endif
