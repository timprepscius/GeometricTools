// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef TERRAINS_H
#define TERRAINS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class Terrains : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Terrains ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();

    // Turret-based camera motion.
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void MoveUp ();
    virtual void MoveDown ();

    NodePtr mScene;
    TriMeshPtr mSkyDome;
    TerrainPtr mTerrain;
    float mHeightAboveTerrain;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Terrains);
WM5_REGISTER_TERMINATE(Terrains);

#endif
