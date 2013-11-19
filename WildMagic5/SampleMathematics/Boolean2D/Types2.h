// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef TYPES2_H
#define TYPES2_H

#include "Edge2.h"
#include "Wm5Vector2.h"
using namespace Wm5;

// vertices
typedef std::map<Vector2d,int> VMap;
typedef std::map<Vector2d,int>::iterator VIterator;
typedef std::map<Vector2d,int>::const_iterator VCIterator;
typedef std::vector<Vector2d> VArray;

// edges
typedef std::map<Edge2,int> EMap;
typedef std::map<Edge2,int>::iterator EIterator;
typedef std::map<Edge2,int>::const_iterator ECIterator;
typedef std::vector<Edge2> EArray;

#endif
