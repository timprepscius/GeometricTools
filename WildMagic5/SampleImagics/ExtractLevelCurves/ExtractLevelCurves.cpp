// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ExtractLevelCurves.h"

WM5_CONSOLE_APPLICATION(ExtractLevelCurves);

// Comment this out for linear interpolation.
#define USE_BILINEAR

ImageRGB82D* ExtractLevelCurves::msColor = 0;
unsigned int ExtractLevelCurves::msSelectedColor = 0;
float ExtractLevelCurves::msMultiply = 1.0f/(float)MAGNIFY;

//----------------------------------------------------------------------------
ExtractLevelCurves::ExtractLevelCurves ()
    :
    ConsoleApplication("SampleImagics/ExtractLevelCurves")
{
}
//----------------------------------------------------------------------------
void ExtractLevelCurves::DrawPixel (int x, int y)
{
    if (0 <= x && x < msColor->GetBound(0)
    &&  0 <= y && y < msColor->GetBound(1))
    {
        (*msColor)(x, y) = msSelectedColor;
    }
}
//----------------------------------------------------------------------------
bool ExtractLevelCurves::Extract ()
{
    msColor = new0 ImageRGB82D(MAGNIFY*SIZE, MAGNIFY*SIZE);

    // Load image for level set extraction.
    std::string imageName = Environment::GetPathR("Head.im");
    ImageInt2D image(imageName.c_str());
    if (image.GetDimensions() == 0)
    {
        // Cannot load the image.
        return false;
    }

    // Get the extremes.
    int minValue = image[0], maxValue = minValue;
    for (int i = 1; i < image.GetQuantity(); ++i)
    {
        int value = image[i];
        if (value < minValue)
        {
            minValue = value;
        }
        else if (value > maxValue)
        {
            maxValue = value;
        }
    }
    int range = maxValue - minValue;

    // Generate a color subimage to superimpose level sets.  Process the
    // subimage with upper left corner (100,100) and of size 32x32.
    for (int y = 0; y < SIZE; ++y)
    {
        for (int x = 0; x < SIZE; ++x)
        {
            for (int dy = 0; dy < MAGNIFY; ++dy)
            {
                float fy = YPOS + y + msMultiply*dy;
                for (int dx = 0; dx < MAGNIFY; ++dx)
                {
                    float fx = XPOS + x + msMultiply*dx;
#ifdef USE_BILINEAR
                    float interp = Bilerp(image, fx, fy);
#else
                    float interp = Lerp(image, fx, fy);
#endif
                    interp = (interp - minValue)/(float)range;
                    unsigned char gray = (unsigned char)(255.0f*interp);
                    (*msColor)(MAGNIFY*x + dx, MAGNIFY*y + dy) =
                        GetColor24(gray, gray, gray);
                }
            }
        }
    }

    // Extract the level set.
    std::vector<Vector2f> vertices;
    std::vector<EdgeKey> edges;
#ifdef USE_BILINEAR
    ExtractCurveSquares ecs(image.GetBound(0), image.GetBound(1),
        (int*)image.GetData());
#else
    ExtractCurveTris ecs(image.GetBound(0), image.GetBound(1),
        (int*)image.GetData());
#endif
    ecs.ExtractContour(512, vertices, edges);
    ecs.MakeUnique(vertices, edges);

    // Draw the edges in the subimage.
    msSelectedColor = GetColor24(0, 255, 0);
    std::vector<EdgeKey>::iterator eiter = edges.begin();
    std::vector<EdgeKey>::iterator eend = edges.end();
    for (/**/; eiter != eend; ++eiter)
    {
        const EdgeKey& edge = *eiter;

        Vector2f v0 = vertices[edge.V[0]];
        int x0 = (int)((v0[0] - XPOS)*MAGNIFY);
        int y0 = (int)((v0[1] - YPOS)*MAGNIFY);

        Vector2f v1 = vertices[edge.V[1]];
        int x1 = (int)((v1[0] - XPOS)*MAGNIFY);
        int y1 = (int)((v1[1] - YPOS)*MAGNIFY);

        Line2D(x0, y0, x1, y1, DrawPixel);
    }

    // Draw the vertices in the subimage.
    msSelectedColor = GetColor24(255, 0, 0);
    std::vector<Vector2f>::iterator viter = vertices.begin();
    std::vector<Vector2f>::iterator vend = vertices.end();
    for (/**/; viter != vend; ++viter)
    {
        Vector2f v = *viter;
        int x = (int)((v[0] - XPOS)*MAGNIFY);
        int y = (int)((v[1] - YPOS)*MAGNIFY);
        DrawPixel(x, y);
    }

#ifdef USE_BILINEAR
    msColor->Save("BilinearZoom.im");
#else
    msColor->Save("LinearZoom.im");
#endif

    delete0(msColor);
    msColor = 0;
    return true;
}
//----------------------------------------------------------------------------
float ExtractLevelCurves::Lerp (const ImageInt2D& image, float fx, float fy)
    const
{
    int x = (int)fx;
    if (x < 0 || x >= image.GetBound(0))
    {
        return 0.0f;
    }

    int y = (int)fy;
    if (y < 0 || y >= image.GetBound(1))
    {
        return 0.0f;
    }

    float dx = fx - x, dy = fy - y;

    int xBound = image.GetBound(0);
    const int* data = (const int*)image.GetData();
    int i00 = x + xBound*y;
    int i10 = i00 + 1;
    int i01 = i00 + xBound;
    int i11 = i10 + xBound;
    float f00 = (float)data[i00];
    float f10 = (float)data[i10];
    float f01 = (float)data[i01];
    float f11 = (float)data[i11];
    float interp;

    int xParity = (x & 1), yParity = (y & 1);
    if (xParity == yParity)
    {
        if (dx + dy <= 1.0f)
        {
            interp = f00 + dx*(f10 - f00) + dy*(f01 - f00);
        }
        else
        {
            interp = f10 + f01 - f11 + dx*(f11 - f01) + dy*(f11 - f10);
        }
    }
    else
    {
        if (dy <= dx)
        {
            interp = f00 + dx*(f10 - f00) + dy*(f11 - f10);
        }
        else
        {
            interp = f00 + dx*(f11 - f01) + dy*(f01 - f00);
        }
    }

    return interp;
}
//----------------------------------------------------------------------------
float ExtractLevelCurves::Bilerp (const ImageInt2D& image, float fx, float fy)
    const
{
    int x = (int)fx;
    if (x < 0 || x >= image.GetBound(0))
    {
        return 0.0;
    }

    int y = (int)fy;
    if (y < 0 || y >= image.GetBound(1))
    {
        return 0.0;
    }

    float dx = fx - x, dy = fy - y;
    float omdx = 1.0f - dx, omdy = 1.0f - dy;

    int xBound = image.GetBound(0);
    const int* data = (const int*)image.GetData();
    int i00 = x + xBound*y;
    int i10 = i00 + 1;
    int i01 = i00 + xBound;
    int i11 = i10 + xBound;
    float f00 = (float)data[i00];
    float f10 = (float)data[i10];
    float f01 = (float)data[i01];
    float f11 = (float)data[i11];

    float interp = omdx*(omdy*f00 + dy*f01) + dx*(omdy*f10 + dy*f11);
    return interp;
}
//----------------------------------------------------------------------------
int ExtractLevelCurves::Main (int, char**)
{
    return (Extract() ? 0 : -1);
}
//----------------------------------------------------------------------------
