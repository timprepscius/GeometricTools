// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PROJECTEDTEXTUREEFFECT_H
#define PROJECTEDTEXTUREEFFECT_H

#include "Wm5VisualEffectInstance.h"
#include "Wm5Projector.h"
#include "Wm5Light.h"
#include "Wm5Material.h"
#include "Wm5Texture2D.h"

namespace Wm5
{

class ProjectedTextureEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(ProjectedTextureEffect);

public:
    // Construction and destruction.
    ProjectedTextureEffect (const std::string& effectFile);
    virtual ~ProjectedTextureEffect ();

    // Create an instance of the effect with unique parameters.
    VisualEffectInstance* CreateInstance (Projector* projector, Light* light,
        Material* material, Texture2D* projectedTexture);
};

WM5_REGISTER_STREAM(ProjectedTextureEffect);
typedef Pointer0<ProjectedTextureEffect> ProjectedTextureEffectPtr;

}

#endif
