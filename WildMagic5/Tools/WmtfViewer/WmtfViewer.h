// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WMTFVIEWER_H
#define WMTFVIEWER_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class WmtfViewer : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    WmtfViewer ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);
    virtual void ScreenOverlay ();

protected:
    void CopySliceToScreen ();
    void ReadPixelValue (int x, int y);

    void SetStrR5G6B5 (int x, int y);
    void SetStrA1R5G5B5 (int x, int y);
    void SetStrA4R4G4B4 (int x, int y);
    void SetStrA8 (int x, int y);
    void SetStrL8 (int x, int y);
    void SetStrA8L8 (int x, int y);
    void SetStrR8G8B8 (int x, int y);
    void SetStrA8R8G8B8 (int x, int y);
    void SetStrA8B8G8R8 (int x, int y);
    void SetStrL16 (int x, int y);
    void SetStrG16R16 (int x, int y);
    void SetStrA16B16G16R16 (int x, int y);
    void SetStrR16F (int x, int y);
    void SetStrG16R16F (int x, int y);
    void SetStrA16B16G16R16F (int x, int y);
    void SetStrR32F (int x, int y);
    void SetStrG32R32F (int x, int y);
    void SetStrA32B32G32R32F (int x, int y);

    Texture2DPtr mTexture;
    Texture::Format mFormat;
    int mNumTexels, mXDim, mYDim;
    char* mTexels;
    Float4* mImage;
    int mSliceIndex;
    float mRGBMin, mRGBMax, mInvRGBRange;
    float mAMin, mAMax, mInvARange;
    bool mAlphaActive, mMouseDown;
    Float4 mTextColor;

    enum { PIXEL_STRING_SIZE = 512 };
    char mPixelString[PIXEL_STRING_SIZE];

    typedef void (WmtfViewer::*SetStrFunction)(int,int);
    SetStrFunction mSFunction[Color::CF_QUANTITY];
};

WM5_REGISTER_INITIALIZE(WmtfViewer);
WM5_REGISTER_TERMINATE(WmtfViewer);

#endif
