// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/15)

#ifndef INTERSECTIONELLIPSESELLIPSOIDS_H
#define INTERSECTIONELLIPSESELLIPSOIDS_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class IntersectionEllipsesEllipsoids : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectionEllipsesEllipsoids ();
    virtual ~IntersectionEllipsesEllipsoids ();

    virtual int Main (int numArguments, char** arguments);

protected:
};

WM5_REGISTER_INITIALIZE(IntersectionEllipsesEllipsoids);
WM5_REGISTER_TERMINATE(IntersectionEllipsesEllipsoids);

#endif
