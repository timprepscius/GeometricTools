// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef EXTRACTLEVELCURVES_H
#define EXTRACTLEVELCURVES_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class ExtractLevelCurves : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ExtractLevelCurves ();

    virtual int Main (int numArguments, char** arguments);

protected:
    enum { MAGNIFY = 8, SIZE = 32, XPOS = 100, YPOS = 100 };

    bool Extract ();
    float Lerp (const ImageInt2D& image, float fx, float fy) const;
    float Bilerp (const ImageInt2D& image, float fx, float fy) const;

    static void DrawPixel (int x, int y);
    static ImageRGB82D* msColor;
    static unsigned int msSelectedColor;
    static float msMultiply;
};

WM5_REGISTER_INITIALIZE(ExtractLevelCurves);
WM5_REGISTER_TERMINATE(ExtractLevelCurves);

#endif
