// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CLODMESHES_H
#define CLODMESHES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

// The sample creates a clodified mesh with a texture.  To see the original
// mesh with the texture, comment out the #define of USE_CLOD_MESH.
#define USE_CLOD_MESH

class ClodMeshes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ClodMeshes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    Culler mCuller;

#ifdef USE_CLOD_MESH
    virtual void MoveForward ();
    virtual void MoveBackward ();
    void UpdateClods ();
    
    ClodMeshPtr mClod[2], mActive;
#endif

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ClodMeshes);
WM5_REGISTER_TERMINATE(ClodMeshes);

#endif
