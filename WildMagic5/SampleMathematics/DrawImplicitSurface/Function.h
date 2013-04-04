// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FUNCTION_H
#define FUNCTION_H

#include "Wm5Vector3.h"
using namespace Wm5;

// Evaluate the function F at the specified position.
float F (const Vector3f& position);

// Evaluate the gradient of the function F at the specified position.
Vector3f DF (const Vector3f& position);

#endif
