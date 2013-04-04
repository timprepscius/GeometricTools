// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BEADSLIDE_H
#define BEADSLIDE_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class BeadSlide : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BeadSlide ();

    virtual int Main (int numArguments, char** arguments);

protected:
    void Simulation ();
};

WM5_REGISTER_INITIALIZE(BeadSlide);
WM5_REGISTER_TERMINATE(BeadSlide);

#endif
