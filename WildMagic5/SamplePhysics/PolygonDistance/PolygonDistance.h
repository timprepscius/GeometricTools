// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef POLYGONDISTANCE_H
#define POLYGONDISTANCE_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class PolygonDistance : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    PolygonDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void InitialConfiguration ();
    void NextConfiguration ();

    void GetPolarRepresentation (int numVertices, const Vector2f* vertices,
        Vector2f& centroid, Vector2f* polars);

    void GetCartesianRepresentation (int numVertices, Vector2f* vertices,
        const Vector2f& centroid, const Vector2f* polars);

    void RotatePolygon (int numVertices, int sign, Vector2f* polars);

    void ComputePerpendiculars (int numVertices, const Vector2f* vertices,
        const Vector2f& closest, Vector2f end[2]);

    void DrawLineSegment (int thick, ColorRGB color, const Vector2f& end0,
        const Vector2f& end1);

    void DrawPerpendiculars (const Vector2f end[2]);
    void DrawPoints (int thick, ColorRGB color, const Vector2f& point);

    class Polygon
    {
    public:
        int NumVertices;
        int Sign;
        Vector2f* Vertices;
        Vector2f Centroid;
        Vector2f* Polars;
        Tuple<2,int>* Faces;
    };

    enum { NUM_POLYGONS = 3 };
    Polygon mPolygons[NUM_POLYGONS];

    int mSize;
    bool mDrawPerpendiculars;
};

WM5_REGISTER_INITIALIZE(PolygonDistance);
WM5_REGISTER_TERMINATE(PolygonDistance);

#endif
