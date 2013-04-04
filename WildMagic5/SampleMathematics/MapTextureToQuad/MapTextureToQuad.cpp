// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MapTextureToQuad.h"

WM5_WINDOW_APPLICATION(MapTextureToQuad);

//----------------------------------------------------------------------------
MapTextureToQuad::MapTextureToQuad ()
    :
    WindowApplication2("SampleMathematics/MapTextureToQuad", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mTexture = 0;
    mMapping = 0;
    mMouseDown = false;
    mSelected = -1;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // The texture whose image will be drawn on the quadrilateral.
    std::string path = Environment::GetPathR("MagicianSmallReflectY.wmtf");
    mTexture = Texture2D::LoadWMTF(path);

    // The quadrilateral to which the image is perspectively mapped.  The
    // default is the original image rectangle (the initial mapping is the
    // identity).
    int dim0 = mTexture->GetDimension(0, 0);
    int dim1 = mTexture->GetDimension(1, 0);
    float xmax = dim0 - 1.0f;
    float ymax = dim1 - 1.0f;
    mVertex[0] = Vector2f(0.0f, 0.0f);
    mVertex[1] = Vector2f(xmax, 0.0f);
    mVertex[2] = Vector2f(xmax, ymax);
    mVertex[3] = Vector2f(0.0f, ymax);

    Vector2f offset(0.5f*(GetWidth() - dim0), 0.5f*(GetHeight() - dim1));
    for (int i = 0; i < 4; ++i)
    {
        mVertex[i] += offset;
    }

    CreateMapping();
    return true;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::OnTerminate ()
{
    delete0(mTexture);
    delete0(mMapping);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (state == MOUSE_DOWN)
    {
        mMouseDown = true;
        SelectVertex(Vector2f((float)x, (float)y));
        return true;
    }

    if (state == MOUSE_UP)
    {
        mMouseDown = false;
        if (mSelected >= 0
        &&  0 <= x && x < GetWidth()
        &&  0 <= y && y < GetHeight())
        {
            UpdateQuadrilateral(Vector2f((float)x, (float)y));
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnMotion (int, int x, int y, unsigned int)
{
    if (mMouseDown)
    {
        if (mSelected >= 0
        &&  0 <= x && x < GetWidth()
        &&  0 <= y && y < GetHeight())
        {
            UpdateQuadrilateral(Vector2f((float)x, (float)y));
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::CreateMapping ()
{
    // Create the new perspective mapping from the *target* quadrilateral to
    // the *source* square bitmap.  The mapping is in this direction to avoid
    // holes in the drawn quadrilateral.
    delete0(mMapping);
#ifdef USE_HM_QUAD_TO_SQR
    mMapping = new0 HmQuadToSqrf(mVertex[0], mVertex[1], mVertex[2],
        mVertex[3]);
#else
    mMapping = new0 BiQuadToSqrf(mVertex[0], mVertex[1], mVertex[2],
        mVertex[3]);
#endif

    // Compute axis-aligned bounding box.
    int xmin = GetWidth(), xmax = 0, ymin = GetWidth(), ymax = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (xmin > (int)mVertex[i].X())
        {
            xmin = (int)mVertex[i].X();
        }

        if (xmax < (int)mVertex[i].X())
        {
            xmax = (int)mVertex[i].X();
        }

        if (ymin > (int)mVertex[i].Y())
        {
            ymin = (int)mVertex[i].Y();
        }

        if (ymax < (int)mVertex[i].Y())
        {
            ymax = (int)mVertex[i].Y();
        }
    }

    // Draw perspective mapping of image (...inefficient drawing...).
    ClearScreen();
    int xSize = mTexture->GetDimension(0, 0);
    int ySize = mTexture->GetDimension(1, 0);
    unsigned char* data = (unsigned char*)mTexture->GetData(0);
    for (int trgY = ymin; trgY <= ymax; ++trgY)
    {
        Vector2f quad;
        quad.Y() = (float)trgY;

        int xStart = xmin;
        while (xStart <= xmax)
        {
            quad.X() = (float)xStart;
            if (PointInPolygon2f(4, mVertex).ContainsQuadrilateral(quad))
            {
                break;
            }
            ++xStart;
        }

        int xFinal = xmax;
        while (xFinal >= xmin)
        {
            quad.X() = (float)xFinal;
            if (PointInPolygon2f(4, mVertex).ContainsQuadrilateral(quad))
            {
                break;
            }
            --xFinal;
        }

        for (int trgX = xStart; trgX <= xFinal; ++trgX)
        {
            // Transform point to unit square.
            quad.X() = (float)trgX;
            Vector2f square = mMapping->Transform(quad);

            // Convert to bitmap coordinates (using clamping).
            int srcX = (int)((xSize - 1)*square.X());
            if (srcX < 0)
            {
                srcX = 0;
            }
            else if (srcX >= xSize)
            {
                srcX = xSize - 1;
            }

            int srcY = (int)((ySize - 1)*square.Y());
            if (srcY < 0)
            {
                srcY = 0;
            }
            else if (srcY >= ySize)
            {
                srcY = ySize - 1;
            }

            int srcIndex = 4*(srcX + xSize*srcY);
            unsigned char* srcRGBA = data + srcIndex;
            int trgIndex = trgX + GetWidth()*trgY;
            unsigned char* trgRGB = (unsigned char*)(mScreen + trgIndex);

            for (int i = 0; i < 3; ++i)
            {
                trgRGB[i] = srcRGBA[i];
            }
        }
    }
    OnDisplay();
}
//----------------------------------------------------------------------------
void MapTextureToQuad::SelectVertex (const Vector2f& position)
{
    // Identify vertex within 5 pixels of mouse click.
    const float pixelRange = 5.0f;
    mSelected = -1;
    for (int i = 0; i < 4; ++i)
    {
        Vector2f diff = position - mVertex[i];
        if (diff.Length() <= pixelRange)
        {
            mSelected = i;
            break;
        }
    }
}
//----------------------------------------------------------------------------
void MapTextureToQuad::UpdateQuadrilateral (const Vector2f& position)
{
    // Quadrilateral must remain convex.
    int prev = (mSelected > 0 ? mSelected - 1 : 3);
    int next = (mSelected < 3 ? mSelected + 1 : 0);
    Vector2f diff1 = position - mVertex[prev];
    Vector2f diff2 = mVertex[next] - position;
    float det = diff1.X()*diff2.Y() - diff1.Y()*diff2.X();
    if (det > 0.0f)
    {
        mVertex[mSelected] = position;
        CreateMapping();
    }
}
//----------------------------------------------------------------------------
