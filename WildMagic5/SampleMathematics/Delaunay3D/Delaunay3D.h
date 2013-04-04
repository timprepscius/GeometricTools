// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef DELAUNAY3D_H
#define DELAUNAY3D_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class Delaunay3D : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Delaunay3D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    TriMesh* CreateSphere () const;
    TriMesh* CreateTetra (int index) const;
    void ChangeTetraStatus (int index, const Float4& color, bool enableWire);
    void ChangeLastTetraStatus (int index, int vOpposite,
        const Float4& color, const Float4& oppositeColor);
    void DoSearch ();

    NodePtr mScene;
    CullStatePtr mCullState;
    Culler mCuller;
    Delaunay3f* mDelaunay;
    Vector3f mMin, mMax;
};

WM5_REGISTER_INITIALIZE(Delaunay3D);
WM5_REGISTER_TERMINATE(Delaunay3D);

#endif
