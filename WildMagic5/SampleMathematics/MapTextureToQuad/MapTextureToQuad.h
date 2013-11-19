// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MAPTEXTURETOQUAD_H
#define MAPTEXTURETOQUAD_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

// If this is defined, use a HmQuadToSqrf mapping.  If this is not defined,
// use a BiQuadToSqrf mapping.
#define USE_HM_QUAD_TO_SQR

class MapTextureToQuad : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MapTextureToQuad ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    // Allows user to drag vertices of convex quadrilateral.
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y,
        unsigned int modifiers);

protected:
    void CreateMapping ();
    void SelectVertex (const Vector2f& position);
    void UpdateQuadrilateral (const Vector2f& position);

    // The image to perspectively draw onto the convex quadrilateral.
    Texture2D* mTexture;

    // The four vertices of the convex quadrilateral in counterclockwise
    // order:  Q00 = V[0], Q10 = V[1], Q11 = V[2], Q01 = V[3].
    Vector2f mVertex[4];

#ifdef USE_HM_QUAD_TO_SQR
    HmQuadToSqrf* mMapping;
#else
    BiQuadToSqrf* mMapping;
#endif

    // For dragging the quadrilateral vertices.
    bool mMouseDown;
    int mSelected;
};

WM5_REGISTER_INITIALIZE(MapTextureToQuad);
WM5_REGISTER_TERMINATE(MapTextureToQuad);

#endif
