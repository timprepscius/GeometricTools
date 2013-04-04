// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef RANDOMCONTROLLER_H
#define RANDOMCONTROLLER_H

#include "Wm5PointController.h"

namespace Wm5
{

class RandomController : public PointController
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(RandomController);

public:
    RandomController ();

protected:
    virtual void UpdatePointMotion (float ctrlTime);
};

WM5_REGISTER_STREAM(RandomController);
typedef Pointer0<RandomController> RandomControllerPtr;

}

#endif
