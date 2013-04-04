// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/07/31)

#ifndef ROOTFINDEREFFECT_H
#define ROOTFINDEREFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5VisualEffectInstance.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class RootFinderEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(RootFinderEffect);

public:
    // Construction and destruction.
    RootFinderEffect ();
    virtual ~RootFinderEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance ();

private:
    static int msDx9VRegisters[1];
    static int msOglVRegisters[1];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[1];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
    Texture2DPtr mTrailing;
};

WM5_REGISTER_STREAM(RootFinderEffect);
typedef Pointer0<RootFinderEffect> RootFinderEffectPtr;

}

#endif
