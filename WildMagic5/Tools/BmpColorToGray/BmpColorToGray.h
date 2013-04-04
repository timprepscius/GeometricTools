// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/01/01)

#ifndef BMPCOLORTOGRAY_H
#define BMPCOLORTOGRAY_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class BmpColorToGray : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BmpColorToGray ();
    virtual ~BmpColorToGray ();
    virtual int Main (int quantity, char** argument);
};

WM5_REGISTER_INITIALIZE(BmpColorToGray);
WM5_REGISTER_TERMINATE(BmpColorToGray);

#endif
