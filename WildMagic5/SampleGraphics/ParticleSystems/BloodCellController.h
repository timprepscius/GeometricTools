// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BLOODCELLCONTROLLER_H
#define BLOODCELLCONTROLLER_H

#include "Wm5ParticleController.h"

namespace Wm5
{

class BloodCellController : public ParticleController
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(BloodCellController);

public:
    BloodCellController ();

protected:
    virtual void UpdatePointMotion (float ctrlTime);
};

WM5_REGISTER_STREAM(BloodCellController);
typedef Pointer0<BloodCellController> BloodCellControllerPtr;

}

#endif
