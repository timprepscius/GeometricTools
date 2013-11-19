// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTITRIANGLE_H
#define MTITRIANGLE_H

class MTITriangle
{
public:
    MTITriangle (int label0, int label1, int lLabel2);

    bool operator< (const MTITriangle& triangle) const;
    bool operator== (const MTITriangle& triangle) const;
    bool operator!= (const MTITriangle& triangle) const;

    int GetLabel (int i) const;

protected:
    int mLabel[3];
};

#endif
