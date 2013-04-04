// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef EDGE2_H
#define EDGE2_H

class Edge2
{
public:
    // Construction.
    Edge2 ();
    Edge2 (int i0, int i1);

    // Comparison to support STL sorting.
    bool operator< (const Edge2& edge) const;

    int I0, I1;
};

#endif
