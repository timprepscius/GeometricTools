// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BLENDEDTERRAINEFFECT_H
#define BLENDEDTERRAINEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5ShaderFloat.h"
#include "Wm5Texture1D.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class BlendedTerrainEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BlendedTerrainEffect);

public:
    // Construction and destruction.
    BlendedTerrainEffect (const std::string& effectFile);
    virtual ~BlendedTerrainEffect ();

    // Create an instance of the effect with unique parameters.  The
    // constant 'flowDirection' uses the first two floats of one register.
    // The constant 'powerFactor' uses one float of one register.  The
    // shader constants should be initialized by the caller.
    VisualEffectInstance* CreateInstance (ShaderFloat* flowDirection,
        ShaderFloat* powerFactor, Texture2D* grassTexture,
        Texture2D* stoneTexture, Texture2D* cloudTexture);

protected:
    Texture1DPtr mBlendTexture;
};

WM5_REGISTER_STREAM(BlendedTerrainEffect);
typedef Pointer0<BlendedTerrainEffect> BlendedTerrainEffectPtr;

}

#endif
