// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef MORPHFACES_H
#define MORPHFACES_H

#include "Wm5WindowApplication3.h"
#include "CubicInterpolator.h"
using namespace Wm5;

class MorphFaces : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MorphFaces ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateMorphResult ();
    void CreateBaseTarget ();
    void LoadTarget (int i, const std::string& targetName);
    void UpdateMorph (float time);

    Float4 mTextColor;
    NodePtr mScene, mMorphResult;
    WireStatePtr mWireState;
    LightPtr mLight;
    VisibleSet mVisibleSet;

    enum { NUM_TARGETS = 12 };

    struct InVertex
    {
        Vector3f Position;
        Vector3f Normal;
    };

    struct OutVertex
    {
        Vector3f Position;
        Vector3f Normal;
        Vector2f TCoord;
    };

    int mNumVertices;
    InVertex* mVertices[NUM_TARGETS];
    CubicInterpolator<1,float>* mWeightInterpolator[NUM_TARGETS];
    CubicInterpolator<3,float>* mColorInterpolator;
    float mAnimStartTime;
};

WM5_REGISTER_INITIALIZE(MorphFaces);
WM5_REGISTER_TERMINATE(MorphFaces);

#endif
