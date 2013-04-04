// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MESHENVELOPE_H
#define MESHENVELOPE_H

#include "Wm5WindowApplication2.h"
#include "Wm5PlanarGraph.h"
using namespace Wm5;

class MeshEnvelope : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MeshEnvelope ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();

protected:
    // mesh
    int mNumVertices;
    Vector2f* mVertices;
    int mNumIndices;
    int* mIndices;

    // envelope of mesh
    int mNumEnvelopeVertices;
    Vector2f* mEnvelopeVertices;

    int mSize;
};

WM5_REGISTER_INITIALIZE(MeshEnvelope);
WM5_REGISTER_TERMINATE(MeshEnvelope);

#endif
