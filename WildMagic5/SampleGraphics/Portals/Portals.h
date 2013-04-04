// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PORTALS_H
#define PORTALS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class Portals : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Portals ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    ConvexRegionManager* CreateBspTree ();
    TriMesh* CreateOutside ();

    void CreateCenterCube (Texture2D* floorTexture, Texture2D* ceilingTexture,
        Texture2D* wallTexture, Texture2D* picture0Texture,
        Texture2D* picture1Texture, Texture2D* picture2Texture,
        Texture2D* picture3Texture, Node*& cube, Portal**& portals);

    void CreateAxisConnector (Texture2D* floorTexture,
        Texture2D* ceilingTexture, Texture2D* wallTexture, Node*& cube,
        Portal**& portals);

    void CreateEndCube (Texture2D* floorTexture, Texture2D* ceilingTexture,
        Texture2D* wallTexture, Node*& cube, Portal**& portals);

    void CreateDiagonalConnector (Texture2D* floorTexture,
        Texture2D* ceilingTexture, Texture2D* wallTexture, Node*& cube,
        Portal**& portals);

    NodePtr mScene;
    WireStatePtr mWireState;
    Texture2DEffect* mEffect;
    VertexFormat* mPTFormat;
    int mPTStride;
    Culler mCuller;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Portals);
WM5_REGISTER_TERMINATE(Portals);

#endif
