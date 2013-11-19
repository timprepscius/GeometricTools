// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef DLITMATTEXEFFECT_H
#define DLITMATTEXEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5Light.h"
#include "Wm5Material.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class DLitMatTexEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(DLitMatTexEffect);

public:
    // Construction and destruction.
    DLitMatTexEffect (const std::string& effectFile);
    virtual ~DLitMatTexEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Light* directionalLight,
        Material* material, Texture2D* diffuseTexture);
};

WM5_REGISTER_STREAM(DLitMatTexEffect);
typedef Pointer0<DLitMatTexEffect> DLitMatTexEffectPtr;

}

#endif
