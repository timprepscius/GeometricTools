// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MRTEFFECT_H
#define MRTEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class MRTEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(MRTEffect);

public:
    // Construction and destruction.
    MRTEffect (const std::string& effectFile);
    virtual ~MRTEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Texture2D* texture0,
        Texture2D* texture1);
};

WM5_REGISTER_STREAM(MRTEffect);
typedef Pointer0<MRTEffect> MRTEffectPtr;

}

#endif
