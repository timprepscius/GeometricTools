// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef ADAPTIVESKELETONCLIMBING2_H
#define ADAPTIVESKELETONCLIMBING2_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class AdaptiveSkeletonClimbing2 : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    AdaptiveSkeletonClimbing2 ();
    virtual ~AdaptiveSkeletonClimbing2 ();

    void Test0 ();
    void Test1 ();

    virtual int Main (int numArguments, char** arguments);

private:
    static void SetPixel (int x, int y);
    static int msData[81];
    static ImageRGB82D* msColor;
};

WM5_REGISTER_INITIALIZE(AdaptiveSkeletonClimbing2);
WM5_REGISTER_TERMINATE(AdaptiveSkeletonClimbing2);

#endif
