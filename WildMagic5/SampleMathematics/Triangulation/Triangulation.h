// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class Triangulation : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Triangulation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void ClearAll ();

    void DoExample0 ();
    void DoExample1 ();
    void DoExample2 ();
    void DoExample3 ();
    void DoExample4 ();

    std::vector<Vector2f> mPositions;
    std::vector<int> mOuter, mInner0, mInner1;
    std::vector<std::vector<int>*> mInners;
    TriangulateEC<float>::Tree* mRoot;
    std::vector<Vector2f> mFillSeeds;
    std::vector<int> mTriangles;

    Query::Type mType;
    float mEpsilon;
    int mExample;
};

WM5_REGISTER_INITIALIZE(Triangulation);
WM5_REGISTER_TERMINATE(Triangulation);

#endif
