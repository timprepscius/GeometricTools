// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef DELAUNAY2DINSERTREMOVE_H
#define DELAUNAY2DINSERTREMOVE_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class Delaunay2DInsertRemove : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Delaunay2DInsertRemove ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);

protected:
    void GetMesh ();

    IncrementalDelaunay2f* mDelaunay;
    int mNumVertices;
    Vector2f* mVertices;
    int mNumTriangles;
    int* mIndices;
    int* mAdjacencies;

    int mSize;
};

WM5_REGISTER_INITIALIZE(Delaunay2DInsertRemove);
WM5_REGISTER_TERMINATE(Delaunay2DInsertRemove);

#endif
