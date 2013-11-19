// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPTREE2_H
#define BSPTREE2_H

#include "Types2.h"
class BspPolygon2;

class BspTree2
{
public:
    // Construction and destruction.
    BspTree2 (BspPolygon2& polygon, const EArray& edges);
    ~BspTree2 ();

    BspTree2* GetCopy () const;

    // Polygon Boolean operation support.
    void Negate ();
    void GetPartition (const BspPolygon2& polygon, const Vector2d& v0,
        const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
        BspPolygon2& coSame, BspPolygon2& coDiff) const;

    // Point-in-polygon support (-1 outside, 0 on polygon, +1 inside).
    int PointLocation (const BspPolygon2& polygon, const Vector2d& vertex)
        const;

    void Indent (std::ofstream& outFile, int numSpaces);
    void Print (std::ofstream& outFile, int level, char type);

private:
    BspTree2 ()
    {
        // support for get copy
    }

    BspTree2 (const BspTree2&)
    {
        // not supported
    }

    BspTree2& operator= (const BspTree2&)
    {
        // not supported
        return *this;
    }

    enum
    {
        TRANSVERSE_POSITIVE,
        TRANSVERSE_NEGATIVE,
        ALL_POSITIVE,
        ALL_NEGATIVE,
        COINCIDENT
    };

    int Classify (const Vector2d& end0, const Vector2d& end1,
        const Vector2d& v0, const Vector2d& v1, Vector2d& intr) const;

    void GetPosPartition (const BspPolygon2& polygon, const Vector2d& v0,
        const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
        BspPolygon2& coSame,  BspPolygon2& coDiff) const;

    void GetNegPartition (const BspPolygon2& polygon, const Vector2d& v0,
        const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
        BspPolygon2& coSame, BspPolygon2& coDiff) const;

    class Interval
    {
    public:
        Interval (double t0, double t1, bool sameDir, bool touching)
        {
            T0 = t0;
            T1 = t1;
            SameDir = sameDir;
            Touching = touching;
        }

        double T0, T1;
        bool SameDir, Touching;
    };

    void GetCoPartition (const BspPolygon2& polygon, const Vector2d& v0,
        const Vector2d& v1, BspPolygon2& pos, BspPolygon2& neg,
        BspPolygon2& coSame, BspPolygon2& coDiff) const;

    // point-in-polygon support
    int Classify (const Vector2d& end0, const Vector2d& end1,
        const Vector2d& vertex) const;
    int CoPointLocation (const BspPolygon2& polygon, const Vector2d& vertex)
        const;

    EArray mCoincident;
    BspTree2* mPosChild;
    BspTree2* mNegChild;
};

#endif
