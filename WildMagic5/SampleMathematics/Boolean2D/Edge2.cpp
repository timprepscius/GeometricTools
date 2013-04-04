// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Edge2.h"

//----------------------------------------------------------------------------
Edge2::Edge2 ()
{
    I0 = -1;
    I1 = -1;
}
//----------------------------------------------------------------------------
Edge2::Edge2 (int i0, int i1)
{
    I0 = i0;
    I1 = i1;
}
//----------------------------------------------------------------------------
bool Edge2::operator< (const Edge2& edge) const
{
    if (I1 < edge.I1)
    {
        return true;
    }

    if (I1 > edge.I1)
    {
        return false;
    }

    return I0 < edge.I0;
}
//----------------------------------------------------------------------------
