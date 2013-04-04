// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef ADAPTIVESKELETONCLIMBING3_H
#define ADAPTIVESKELETONCLIMBING3_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class AdaptiveSkeletonClimbing3 : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    AdaptiveSkeletonClimbing3 ();
    virtual ~AdaptiveSkeletonClimbing3 ();

    void CreateImage (ImageInt3D& image);
    void Test ();

    virtual int Main (int numArguments, char** arguments);
};

WM5_REGISTER_INITIALIZE(AdaptiveSkeletonClimbing3);
WM5_REGISTER_TERMINATE(AdaptiveSkeletonClimbing3);

#endif
