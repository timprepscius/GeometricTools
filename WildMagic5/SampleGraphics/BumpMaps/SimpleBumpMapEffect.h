// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SIMPLEBUMPMAPEFFECT_H
#define SIMPLEBUMPMAPEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Float2.h"
#include "Wm5Texture2D.h"
#include "Wm5Triangles.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class SimpleBumpMapEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SimpleBumpMapEffect);

public:
    // Construction and destruction.
    SimpleBumpMapEffect (const std::string& effectFile);
    virtual ~SimpleBumpMapEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture2D* baseTexture,
        Texture2D* normalTexture);

    // The 'mesh' is one to which an instance of this effect is attached.
    static void ComputeLightVectors (Triangles* mesh,
        const AVector& worldLightDirection);

protected:
    // Compute a tangent at the vertex P0.  The triangle is counterclockwise
    // ordered, <P0,P1,P2>.
    static bool ComputeTangent (
        const APoint& position0, const Float2& tcoord0,
        const APoint& position1, const Float2& tcoord1,
        const APoint& position2, const Float2& tcoord2,
        AVector& tangent);
};

WM5_REGISTER_STREAM(SimpleBumpMapEffect);
typedef Pointer0<SimpleBumpMapEffect> SimpleBumpMapEffectPtr;

}

#endif
