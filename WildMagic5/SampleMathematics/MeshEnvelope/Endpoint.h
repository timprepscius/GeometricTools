// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "Rational.h"

class Endpoint
{
public:
    RScalar Value;  // x-value for a segment endpoint
    int Type;        // '0' if interval min, '1' if interval max
    int Index;       // index of interval containing this endpoint

    // Support for sorting.
    inline bool operator< (const Endpoint& point) const
    {
        if (Value < point.Value)
        {
            return true;
        }
        if (Value > point.Value)
        {
            return false;
        }
        return Type < point.Type;
    }
};

#endif
