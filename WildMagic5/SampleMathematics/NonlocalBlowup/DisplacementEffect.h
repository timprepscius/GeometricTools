// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef DISPLACEMENTEFFECT_H
#define DISPLACEMENTEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Texture1D.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class DisplacementEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(DisplacementEffect);

public:
    // Construction and destruction.
    DisplacementEffect ();
    virtual ~DisplacementEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture2D* displacementTexture,
        Texture2D* gridTexture, Texture1D* colorTexture,
        Texture2D* domainTexture);

private:
    // The shader is hard-coded until "vp*" and "fp*" profiles are added to
    // the WmfxCompiler.
    static int msAllVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static int msAllVTextureUnits[1];
    static int* msVTextureUnits[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[3];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_STREAM(DisplacementEffect);
typedef Pointer0<DisplacementEffect> DisplacementEffectPtr;

}

#endif
