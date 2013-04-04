// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/01/01)

#ifndef BMPTOWMTF_H
#define BMPTOWMTF_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class BmpToWmtf : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BmpToWmtf ();
    virtual ~BmpToWmtf ();
    virtual int Main (int iQuantity, char** apcArgument);
};

WM5_REGISTER_INITIALIZE(BmpToWmtf);
WM5_REGISTER_TERMINATE(BmpToWmtf);

#endif
