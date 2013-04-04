// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef QUADRATICFREEFORM2D_H
#define QUADRATICFREEFORM2D_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class QuadraticFreeForm2D : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    QuadraticFreeForm2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();

    // Allows user to drag control points.
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);

protected:
    float ScreenToControl (int screen) const;
    int ControlToScreen (float control) const;
    Vector2f Evaluate (const Vector2f& param) const;
    void SelectVertex (int x, int y);

    // The image to map to the free-form region.
    Texture2D* mTexture;

    // The control points that define the free-form region.
    int mCtrlX[3][3], mCtrlY[3][3];
    Vector2f mCtrl[3][3];

    // Support for dragging the control points.
    bool mMouseDown;
    int mRow, mCol;

    int mSize;
};

WM5_REGISTER_INITIALIZE(QuadraticFreeForm2D);
WM5_REGISTER_TERMINATE(QuadraticFreeForm2D);

#endif
