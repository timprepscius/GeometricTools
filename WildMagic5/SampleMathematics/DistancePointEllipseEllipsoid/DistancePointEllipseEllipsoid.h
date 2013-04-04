// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.0 (2011/06/18)

#ifndef DISTANCEPOINTELLIPSEELLIPSOID_H
#define DISTANCEPOINTELLIPSEELLIPSOID_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class DistancePointEllipseEllipsoid : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    DistancePointEllipseEllipsoid ();
    virtual ~DistancePointEllipseEllipsoid ();

    virtual int Main (int numArguments, char** arguments);

private:
    void GenerateDistance2 (ImageDouble2D& distance);
    void GenerateDistance3 (ImageDouble3D& distance);
    void HyperGenerateDistance2 (ImageDouble2D& distance);
    void HyperGenerateDistance3 (ImageDouble3D& distance);
};

WM5_REGISTER_INITIALIZE(DistancePointEllipseEllipsoid);
WM5_REGISTER_TERMINATE(DistancePointEllipseEllipsoid);

#endif
