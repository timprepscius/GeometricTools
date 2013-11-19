// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/30)

#ifndef BINARYOPERATIONS_H
#define BINARYOPERATIONS_H

#include "Wm5ConsoleApplication.h"
#include "Wm5Imagics.h"
using namespace Wm5;

class BinaryOperations : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BinaryOperations ();

    virtual int Main (int numArguments, char** arguments);

protected:
    void Get2DBoundaries ();
    void Get2DComponents8 ();
    void Get2DComponents4 ();
    void Get2DL1Distance ();
    void Get2DL2Distance ();
    void Get2DSkeleton ();
    void Get3DComponents26 ();
    void Get3DComponents18 ();
    void Get3DComponents6 ();
};

WM5_REGISTER_INITIALIZE(BinaryOperations);
WM5_REGISTER_TERMINATE(BinaryOperations);

#endif
