// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GLOSSMAPEFFECT_H
#define GLOSSMAPEFFECT_H

#include "Wm5VisualEffectInstance.h"
#include "Wm5Light.h"
#include "Wm5Material.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class GlossMapEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(GlossMapEffect);

public:
    // Construction and destruction.
    GlossMapEffect (const std::string& effectFile);
    virtual ~GlossMapEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture2D* texture, Light* light,
        Material* material);
};

WM5_REGISTER_STREAM(GlossMapEffect);
typedef Pointer0<GlossMapEffect> GlossMapEffectPtr;

}

#endif
