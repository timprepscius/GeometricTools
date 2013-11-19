// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef FPUSUPPORT_H
#define FPUSUPPORT_H

#ifdef WIN32
#include <cfloat>
inline bool IsFinite (float x)
{
    return _finite(x) != 0;
}
#else
inline bool IsFinite (float x)
{
    union { uint32_t encoding; float x; } value;
    value.x = x;
    return ((value.encoding & 0x7F800000u) >> 23) < 255;
}
#endif

#endif
