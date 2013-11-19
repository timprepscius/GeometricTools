// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MASSPULLEYSPRINGSYSTEM_H
#define MASSPULLEYSPRINGSYSTEM_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class MassPulleySpringSystem : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MassPulleySpringSystem ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void InitializeModule ();
    void CreateScene ();
    TriMesh* CreateFloor ();
    TubeSurface* CreateCable ();
    TriMesh* CreateMass (float radius);
    void CreatePulley ();
    void CreateSpring ();
    TubeSurface* CreateHelix ();
    void UpdatePulley ();
    void UpdateCable ();
    void UpdateHelix ();
    void PhysicsTick ();
    void GraphicsTick ();

    // Root of scene and floor mesh.
    NodePtr mScene;
    TriMeshPtr mFloor;
    WireStatePtr mWireState;
    Culler mCuller;

    // Assembly to parent the cable root and pulley root.
    NodePtr mAssembly;

    // Cable modeled as a tube surface, masses attached to ends.
    NodePtr mCableRoot;
    BSplineCurve3f* mCableSpline;
    TubeSurfacePtr mCable;
    static float CableRadial (float);
    TriMeshPtr mMass1, mMass2;

    // Node to parent the pulley and spring.
    NodePtr mPulleyRoot;

    // Pulley modeled as a disk with thickness.
    NodePtr mPulley;
    TriMeshPtr mPlate0, mPlate1, mCylinder;
    Texture2DEffectPtr mMetalEffect;
    Texture2DPtr mMetalTexture;

    // Spring modeled as a tube surface in the shape of a helix, then attached
    // to a U-bracket to hold the pulley disk.
    NodePtr mSpring;
    TriMeshPtr mSide0, mSide1, mTop;
    BSplineCurve3f* mHelixSpline;
    TubeSurfacePtr mHelix;
    static float HelixRadial (float);

    // The physics system.
    PhysicsModule mModule;

    // Support for a controlled frame rate.
    float mLastIdle;
};

WM5_REGISTER_INITIALIZE(MassPulleySpringSystem);
WM5_REGISTER_TERMINATE(MassPulleySpringSystem);

#endif
