// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMSCENEEFFECT_H
#define SMSCENEEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5ProjectorMatrixConstant.h"
#include "Wm5ProjectorWorldPositionConstant.h"
#include "Wm5Texture2D.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class SMSceneEffect : public VisualEffect
{
public:
    // Construction and destruction.
    SMSceneEffect (const std::string& effectFile);
    virtual ~SMSceneEffect ();

    // Create an instance of the effect with unique parameters.  The
    // 'lightPVMatrix' and 'lightBSPVMatrix' inputs are ShaderFloat(4)
    // objects.  The 'lightWorldPosition' and 'lightColor' inputs are
    // ShaderFloat(1) objects.
    VisualEffectInstance* CreateInstance (
        ProjectorWorldPositionConstant* lightWorldPosition,
        ProjectorMatrixConstant* lightPVMatrix, ShaderFloat* lightBSMatrix,
        ShaderFloat* screenBSMatrix, ShaderFloat* lightColor,
        Texture2D* baseTexture, Texture2D* blurTexture,
        Texture2D* projectedTexture) const;
};

}

#endif
