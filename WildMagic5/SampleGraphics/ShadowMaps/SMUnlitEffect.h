// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMUNLITEFFECT_H
#define SMUNLITEFFECT_H

#include "Wm5GlobalEffect.h"
#include "Wm5ProjectorMatrixConstant.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffect.h"

namespace Wm5
{

class SMUnlitEffect : public GlobalEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SMUnlitEffect);

public:
    // Construction and destruction.
    SMUnlitEffect (const std::string& effectName,
        ProjectorMatrixConstant* lightPVMatrix, ShaderFloat* lightBSMatrix,
        ShaderFloat* depthBias, ShaderFloat* texelSize,
        Texture2D* shadowTexture);

    virtual ~SMUnlitEffect ();

    virtual void Draw (Renderer* renderer, const VisibleSet& visibleSet);

private:
    VisualEffectInstancePtr mInstance;
};

WM5_REGISTER_STREAM(SMUnlitEffect);
typedef Pointer0<SMUnlitEffect> SMUnlitEffectPtr;

}

#endif
