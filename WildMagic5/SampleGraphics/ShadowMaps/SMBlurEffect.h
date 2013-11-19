// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMBLUREFFECT_H
#define SMBLUREFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5ShaderFloat.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class SMBlurEffect : public VisualEffect
{
public:
    // Construction and destruction.
    SMBlurEffect (const std::string& effectFile);
    virtual ~SMBlurEffect ();

    // Create an instance of the effect with unique parameters.  The 'weights'
    // input is a ShaderFloat(11), an array of 11 float[4] values.  Only the
    // first three components of each float[4] are used by the shader.  The
    // 'offsets' input is a ShaderFloat(11), also an array of 11 float[4]
    // values.  Only the first two components of each float[4] are used by the
    // shader.
    VisualEffectInstance* CreateInstance (ShaderFloat* weights,
        ShaderFloat* offsets, Texture2D* baseTexture) const;
};

}

#endif
