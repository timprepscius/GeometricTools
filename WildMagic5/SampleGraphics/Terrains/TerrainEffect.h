// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef TERRAINEFFECT_H
#define TERRAINEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class TerrainEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(TerrainEffect);

public:
    // Construction and destruction.
    TerrainEffect (const std::string& effectFile);
    virtual ~TerrainEffect ();

    // Create an instance of the effect with unique parameters.  The input
    // 'fogColorDensity' is a ShaderFloat(1) and stores the fog color in the
    // first three slots and the fog density in the last slot.
    VisualEffectInstance* CreateInstance (Texture2D* baseTexture,
        Texture2D* detailTexture, ShaderFloat* fogColorDensity);
};

WM5_REGISTER_STREAM(TerrainEffect);
typedef Pointer0<TerrainEffect> TerrainEffectPtr;

}

#endif
