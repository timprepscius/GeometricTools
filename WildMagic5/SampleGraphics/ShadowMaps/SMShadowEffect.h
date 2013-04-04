// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMSHADOWEFFECT_H
#define SMSHADOWEFFECT_H

#include "Wm5GlobalEffect.h"
#include "Wm5VisualEffect.h"

namespace Wm5
{

class SMShadowEffect : public GlobalEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SMShadowEffect);

public:
    // Construction and destruction.
    SMShadowEffect (const std::string& effectName,
        ShaderFloat* lightPVMatrix);

    virtual ~SMShadowEffect ();

    virtual void Draw (Renderer* renderer, const VisibleSet& visibleSet);

private:
    VisualEffectInstancePtr mInstance;
};

WM5_REGISTER_STREAM(SMShadowEffect);
typedef Pointer0<SMShadowEffect> SMShadowEffectPtr;

}

#endif
