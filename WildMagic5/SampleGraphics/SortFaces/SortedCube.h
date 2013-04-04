// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SORTEDCUBE_H
#define SORTEDCUBE_H

#include "Wm5Node.h"
#include "Wm5Camera.h"
#include "Wm5TriMesh.h"

namespace Wm5
{

class SortedCube : public Node
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SortedCube);

public:
    // Construction and destruction.
    SortedCube (Camera* camera,
        const std::string& xpName, const std::string& xmName,
        const std::string& ypName, const std::string& ymName,
        const std::string& zpName, const std::string& zmName);

    virtual ~SortedCube ();

    // Member access.
    void SetCamera (Camera* camera);

protected:
    void SortFaces ();
    virtual void GetVisibleSet (Culler& culler, bool noCull = false);

    CameraPtr mCamera;

    // 0 = face xp
    // 1 = face xm
    // 2 = face yp
    // 3 = face ym
    // 4 = face zp
    // 5 = face zm
    TriMeshPtr mFace[6];

    // bit 0 = face xp
    // bit 1 = face xm
    // bit 2 = face yp
    // bit 3 = face ym
    // bit 4 = face zp
    // bit 5 = face zm
    int mBackFacing;
};

WM5_REGISTER_STREAM(SortedCube);
typedef Pointer0<SortedCube> SortedCubePtr;

}

#endif
