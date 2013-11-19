// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MINIMALCYCLEBASIS_H
#define MINIMALCYCLEBASIS_H

#include "Wm5WindowApplication2.h"
#include "Wm5PlanarGraph.h"
using namespace Wm5;

class MinimalCycleBasis : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MinimalCycleBasis ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();

protected:
    typedef PlanarGraph<IVector2> Graph;
    Graph mGraph;
    std::vector<Graph::Primitive*> mPrimitives;
    ColorRGB* mColors;
};

WM5_REGISTER_INITIALIZE(MinimalCycleBasis);
WM5_REGISTER_TERMINATE(MinimalCycleBasis);

#endif
