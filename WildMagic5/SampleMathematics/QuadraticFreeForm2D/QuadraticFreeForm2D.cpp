// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "QuadraticFreeForm2D.h"

WM5_WINDOW_APPLICATION(QuadraticFreeForm2D);

//----------------------------------------------------------------------------
QuadraticFreeForm2D::QuadraticFreeForm2D ()
    :
    WindowApplication2("SampleMathematics/QuadraticFreeForm2D", 0, 0, 512,
        512, Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mTexture = 0;
    mMouseDown = false;
    mRow = -1;
    mCol = -1;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // The texture whose image will be drawn on the quadrilateral.
    std::string path = Environment::GetPathR("Magician.wmtf");
    mTexture = Texture2D::LoadWMTF(path);

    // The default control points produce the identity function.
    int x = mSize/4, y = mSize/4;
    mCtrlX[0][0] =   x;  mCtrlY[0][0] =   y;
    mCtrlX[0][1] =   x;  mCtrlY[0][1] = 2*y;
    mCtrlX[0][2] =   x;  mCtrlY[0][2] = 3*y;
    mCtrlX[1][0] = 2*x;  mCtrlY[1][0] =   y;
    mCtrlX[1][1] = 2*x;  mCtrlY[1][1] = 2*y;
    mCtrlX[1][2] = 2*x;  mCtrlY[1][2] = 3*y;
    mCtrlX[2][0] = 3*x;  mCtrlY[2][0] =   y;
    mCtrlX[2][1] = 3*x;  mCtrlY[2][1] = 2*y;
    mCtrlX[2][2] = 3*x;  mCtrlY[2][2] = 3*y;
    mCtrl[0][0] = Vector2f(0.0f, 0.0f);
    mCtrl[0][1] = Vector2f(0.0f, 0.5f);
    mCtrl[0][2] = Vector2f(0.0f, 1.0f);
    mCtrl[1][0] = Vector2f(0.5f, 0.0f);
    mCtrl[1][1] = Vector2f(0.5f, 0.5f);
    mCtrl[1][2] = Vector2f(0.5f, 1.0f);
    mCtrl[2][0] = Vector2f(1.0f, 0.0f);
    mCtrl[2][1] = Vector2f(1.0f, 0.5f);
    mCtrl[2][2] = Vector2f(1.0f, 1.0f);

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::OnTerminate ()
{
    delete0(mTexture);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::OnDisplay ()
{
    ClearScreen();

    int dim0 = mTexture->GetDimension(0, 0);
    int dim1 = mTexture->GetDimension(1, 0);
    unsigned char* data = (unsigned char*)mTexture->GetData(0);

    const int numSamples = 2*mSize;
    const float invNumSamples = 1.0f/(float)numSamples;
    Vector2f param;
    for (int s = 0; s < numSamples; ++s)
    {
        int u = s*dim0/numSamples;
        param.X() = invNumSamples*s;
        for (int t = 0; t < numSamples; ++t)
        {
            int v = t*dim1/numSamples;
            param.Y() = invNumSamples*t;
            Vector2f result = Evaluate(param);

            int x = ControlToScreen(result.X());
            int y = ControlToScreen(result.Y());
            int index = 4*(u + dim0*v);
            unsigned char b = data[index++];
            unsigned char g = data[index++];
            unsigned char r = data[index++];
            ++index;

            SetPixel(x, y, ColorRGB(r, g, b));
        }
    }

    // Draw the control points.
    ColorRGB green(0, 255, 0);
    for (int row = 0; row <= 2; ++row)
    {
        for (int col = 0; col <= 2; ++col)
        {
            SetThickPixel(mCtrlX[row][col], mCtrlY[row][col], 2, green);
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    // Handle the flip in y that is used for drawing.
    y = mSize - 1 - y;

    if (state == MOUSE_DOWN)
    {
        mMouseDown = true;
        SelectVertex(x, y);
        return true;
    }

    if (state == MOUSE_UP)
    {
        mMouseDown = false;
        if (mRow >= 0 && mCol >= 0
        &&  0 <= x && x < GetWidth() && 0 <= y && y < GetHeight())
        {
            mCtrlX[mRow][mCol] = x;
            mCtrlY[mRow][mCol] = y;
            mCtrl[mRow][mCol].X() = ScreenToControl(x);
            mCtrl[mRow][mCol].Y() = ScreenToControl(y);
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool QuadraticFreeForm2D::OnMotion (int, int x, int y, unsigned int)
{
    if (mMouseDown)
    {
        // Handle the flip in y that is used for drawing.
        y = mSize - 1 - y;

        if (mRow >= 0 && mCol >= 0
        &&  0 <= x && x < GetWidth() && 0 <= y && y < GetHeight())
        {
            mCtrlX[mRow][mCol] = x;
            mCtrlY[mRow][mCol] = y;
            mCtrl[mRow][mCol].X() = ScreenToControl(x);
            mCtrl[mRow][mCol].Y() = ScreenToControl(y);
            OnDisplay();
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
float QuadraticFreeForm2D::ScreenToControl (int screen) const
{
    return 2*screen/(float)mSize - 0.5f;
}
//----------------------------------------------------------------------------
int QuadraticFreeForm2D::ControlToScreen (float control) const
{
    return (int)(mSize*(0.5f*control + 0.25f));
}
//----------------------------------------------------------------------------
Vector2f QuadraticFreeForm2D::Evaluate (const Vector2f& param) const
{
    float s = param.X();
    float t = param.Y();
    float omS = 1.0f - s;
    float omT = 1.0f - t;
    float b0s = omS*omS;
    float b0t = omT*omT;
    float b1s = 2.0f*omS*s;
    float b1t = 2.0f*omT*t;
    float b2s = s*s;
    float b2t = t*t;

    Vector2f result =
        b0s*(b0t*mCtrl[0][0] + b1t*mCtrl[0][1] + b2t*mCtrl[0][2]) +
        b1s*(b0t*mCtrl[1][0] + b1t*mCtrl[1][1] + b2t*mCtrl[1][2]) +
        b2s*(b0t*mCtrl[2][0] + b1t*mCtrl[2][1] + b2t*mCtrl[2][2]);

    return result;
}
//----------------------------------------------------------------------------
void QuadraticFreeForm2D::SelectVertex (int x, int y)
{
    // Identify vertex within 5 pixels of mouse click.
    const int pixelRange = 5;
    mRow = -1;
    mCol = -1;
    for (int row = 0; row <= 2; ++row)
    {
        for (int col = 0; col <= 2; ++col)
        {
            int dx = x - mCtrlX[row][col];
            int dy = y - mCtrlY[row][col];
            if (abs(dx) <= pixelRange && abs(dy) <= pixelRange)
            {
                mRow = row;
                mCol = col;
                return;
            }
        }
    }
}
//----------------------------------------------------------------------------
