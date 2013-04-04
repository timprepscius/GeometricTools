// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef IMVIEWER_H
#define IMVIEWER_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class ImViewer : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ImViewer ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnSpecialKeyDown (int key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);
    virtual void ScreenOverlay ();

protected:
    bool LoadImage ();
    void CopySliceToScreen ();
    void ReadPixelValue (int x, int y);
    void WritePixelString ();

    int mNumDimensions, mNumPixels, mNumSlicePixels;
    int* mBound;
    float* mFloatData;
    ColorRGB* mColorData;
    float mMin, mMax, mRange, mInvRange;
    int mZ;
    enum { PIXEL_STRING_SIZE = 256 };
    char mPixelString[PIXEL_STRING_SIZE];
    bool mMouseDown;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ImViewer);
WM5_REGISTER_TERMINATE(ImViewer);

#endif
