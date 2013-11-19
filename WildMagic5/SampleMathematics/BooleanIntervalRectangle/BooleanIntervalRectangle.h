// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BOOLEANINTERVALRECTANGLE_H
#define BOOLEANINTERVALRECTANGLE_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class BooleanIntervalRectangle : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BooleanIntervalRectangle ();
    virtual ~BooleanIntervalRectangle ();
    virtual int Main (int numArguments, char** arguments);
};

WM5_REGISTER_INITIALIZE(BooleanIntervalRectangle);
WM5_REGISTER_TERMINATE(BooleanIntervalRectangle);

#endif
