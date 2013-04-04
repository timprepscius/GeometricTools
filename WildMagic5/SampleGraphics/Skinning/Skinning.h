// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SKINNING_H
#define SKINNING_H

#include "Wm5WindowApplication3.h"
#include "SkinningEffect.h"
using namespace Wm5;

class Skinning : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Skinning ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();
    Float4 ComputeWeights (int a);
    void UpdateConstants (float time);

    NodePtr mScene, mTrnNode;
    Culler mCuller;
    float* mSkinningMatrix[4];

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Skinning);
WM5_REGISTER_TERMINATE(Skinning);

#endif
