// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CUBEMAPEFFECT_H
#define CUBEMAPEFFECT_H

#include "Wm5VisualEffect.h"
#include "Wm5Culler.h"
#include "Wm5Renderer.h"
#include "Wm5Texture2D.h"
#include "Wm5TextureCube.h"
#include "Wm5TriMesh.h"
#include "Wm5VisualEffectInstance.h"

namespace Wm5
{

class CubeMapEffect : public VisualEffect
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(CubeMapEffect);

public:
    // Construction and destruction.
    CubeMapEffect (const std::string& effectFile);
    virtual ~CubeMapEffect ();

    // Create an instance of the effect with unique parameters.  The
    // 'reflectivity' is a ShaderFloat(1) whose 0-indexed component is the
    // only one used.
    VisualEffectInstance* CreateInstance (TextureCube* texture,
        ShaderFloat* reflectivity, bool allowDynamicUpdates);

    static void DestroyUpdateMap ();

    // For dynamic updating of the cube map.  This function computes the new
    // faces only when 'allowDynamicUpdates' was set to 'true' in the
    // CreateInstance call.
    static bool AllowDynamicUpdates (VisualEffectInstance* instance);
    static void UpdateFaces (VisualEffectInstance* instance,
        Renderer* renderer, Spatial* scene, const APoint& envOrigin,
        const AVector& envDVector, const AVector& envUVector,
        const AVector& envRVector);

protected:
    class DynamicUpdate
    {
    public:
        CameraPtr DUCamera;
        TriMeshPtr DUSquare;
        RenderTargetPtr DUTarget;
        Culler DUCuller;
    };

    typedef std::map<VisualEffectInstance*,DynamicUpdate> UpdateMap;
    static UpdateMap* msUpdates;
};

WM5_REGISTER_STREAM(CubeMapEffect);
typedef Pointer0<CubeMapEffect> CubeMapEffectPtr;

}

#endif
