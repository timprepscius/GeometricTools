// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTIEDGE_H
#define MTIEDGE_H

class MTIEdge
{
public:
    MTIEdge (int label0, int label1);

    bool operator< (const MTIEdge& edge) const;
    bool operator== (const MTIEdge& edge) const;
    bool operator!= (const MTIEdge& edge) const;

    int GetLabel (int i) const;

protected:
    int mLabel[2];
};

#endif
