// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Triangulation.h"

WM5_WINDOW_APPLICATION(Triangulation);

//----------------------------------------------------------------------------
Triangulation::Triangulation ()
    :
    WindowApplication2("SampleMathematics/Triangulation", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mRoot = 0;
    mType = Query::QT_FILTERED;
    mEpsilon = 0.001f;
    mExample = 0;
}
//----------------------------------------------------------------------------
bool Triangulation::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    DoExample0();

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Triangulation::OnTerminate ()
{
    TriangulateEC<float>::Delete(mRoot);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool Triangulation::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        DoExample0();
        OnDisplay();
        return true;
    case '1':
        DoExample1();
        OnDisplay();
        return true;
    case '2':
        DoExample2();
        OnDisplay();
        return true;
    case '3':
        DoExample3();
        OnDisplay();
        return true;
    case '4':
        DoExample4();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Triangulation::OnDisplay ()
{
    ClearScreen();

    ColorRGB blue(0, 128, 255), black(0, 0, 0), white(255, 255, 255);
    int i, i0, i1, numPositions, x0, y0, x1, y1;

    // Draw the polygon edges.
    switch (mExample)
    {
    case 0:
        numPositions = (int)mPositions.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[i0][0];
            y0 = (int)mPositions[i0][1];
            x1 = (int)mPositions[i1][0];
            y1 = (int)mPositions[i1][1];
            DrawLine(x0, y0, x1, y1, black);
        }
        break;
    case 1:
        numPositions = (int)mOuter.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mOuter[i0]][0];
            y0 = (int)mPositions[mOuter[i0]][1];
            x1 = (int)mPositions[mOuter[i1]][0];
            y1 = (int)mPositions[mOuter[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }

        numPositions = (int)mInner0.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mInner0[i0]][0];
            y0 = (int)mPositions[mInner0[i0]][1];
            x1 = (int)mPositions[mInner0[i1]][0];
            y1 = (int)mPositions[mInner0[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }
        break;
    case 2:
        numPositions = (int)mOuter.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mOuter[i0]][0];
            y0 = (int)mPositions[mOuter[i0]][1];
            x1 = (int)mPositions[mOuter[i1]][0];
            y1 = (int)mPositions[mOuter[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }

        numPositions = (int)mInner0.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mInner0[i0]][0];
            y0 = (int)mPositions[mInner0[i0]][1];
            x1 = (int)mPositions[mInner0[i1]][0];
            y1 = (int)mPositions[mInner0[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }
        break;
    case 3:
        numPositions = (int)mOuter.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mOuter[i0]][0];
            y0 = (int)mPositions[mOuter[i0]][1];
            x1 = (int)mPositions[mOuter[i1]][0];
            y1 = (int)mPositions[mOuter[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }

        numPositions = (int)mInner0.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mInner0[i0]][0];
            y0 = (int)mPositions[mInner0[i0]][1];
            x1 = (int)mPositions[mInner0[i1]][0];
            y1 = (int)mPositions[mInner0[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }

        numPositions = (int)mInner1.size();
        for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
        {
            x0 = (int)mPositions[mInner1[i0]][0];
            y0 = (int)mPositions[mInner1[i0]][1];
            x1 = (int)mPositions[mInner1[i1]][0];
            y1 = (int)mPositions[mInner1[i1]][1];
            DrawLine(x0, y0, x1, y1, black);
        }
        break;
    case 4:
    {
        std::queue<TriangulateEC<float>::Tree*> treeQueue;
        treeQueue.push(mRoot);
        while (treeQueue.size() > 0)
        {
            TriangulateEC<float>::Tree* tree = treeQueue.front();
            treeQueue.pop();
            numPositions = (int)tree->Polygon.size();
            for (i0 = numPositions - 1, i1 = 0; i1 < numPositions; i0 = i1++)
            {
                x0 = (int)mPositions[tree->Polygon[i0]][0];
                y0 = (int)mPositions[tree->Polygon[i0]][1];
                x1 = (int)mPositions[tree->Polygon[i1]][0];
                y1 = (int)mPositions[tree->Polygon[i1]][1];
                DrawLine(x0, y0, x1, y1, black);
            }

            for (i = 0; i < (int)tree->Child.size(); ++i)
            {
                treeQueue.push(tree->Child[i]);
            }
        }
        break;
    }
    }

    // Flood fill the polygon inside.
    for (i = 0; i < (int)mFillSeeds.size(); ++i)
    {
        x0 = (int)mFillSeeds[i][0];
        y0 = (int)mFillSeeds[i][1];
        Fill(x0, y0, blue, white);
    }

    // Draw the triangulation edges.
    int numTriangles = (int)(mTriangles.size()/3);
    const int* indices = &mTriangles.front();
    for (i = 0; i < numTriangles; ++i)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;

        x0 = (int)mPositions[v0][0];
        y0 = (int)mPositions[v0][1];
        x1 = (int)mPositions[v1][0];
        y1 = (int)mPositions[v1][1];
        DrawLine(x0, y0, x1, y1, black);

        x0 = (int)mPositions[v1][0];
        y0 = (int)mPositions[v1][1];
        x1 = (int)mPositions[v2][0];
        y1 = (int)mPositions[v2][1];
        DrawLine(x0, y0, x1, y1, black);

        x0 = (int)mPositions[v2][0];
        y0 = (int)mPositions[v2][1];
        x1 = (int)mPositions[v0][0];
        y1 = (int)mPositions[v0][1];
        DrawLine(x0, y0, x1, y1, black);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void Triangulation::ClearAll ()
{
    mPositions.clear();
    mOuter.clear();
    mInner0.clear();
    mInner1.clear();
    mInners.clear();
    TriangulateEC<float>::Delete(mRoot);
    mFillSeeds.clear();
    mTriangles.clear();
}
//----------------------------------------------------------------------------
void Triangulation::DoExample0 ()
{
    // Simple polygon.

    ClearAll();
    mExample = 0;

    mPositions.resize(10);
    mPositions[0][0] =  29.0f;  mPositions[0][1] = 139.0f;
    mPositions[1][0] =  78.0f;  mPositions[1][1] =  99.0f;
    mPositions[2][0] = 125.0f;  mPositions[2][1] = 141.0f;
    mPositions[3][0] = 164.0f;  mPositions[3][1] = 116.0f;
    mPositions[4][0] = 201.0f;  mPositions[4][1] = 168.0f;
    mPositions[5][0] = 157.0f;  mPositions[5][1] = 163.0f;
    mPositions[6][0] = 137.0f;  mPositions[6][1] = 200.0f;
    mPositions[7][0] =  98.0f;  mPositions[7][1] = 134.0f;
    mPositions[8][0] =  52.0f;  mPositions[8][1] = 146.0f;
    mPositions[9][0] =  55.0f;  mPositions[9][1] = 191.0f;

    mFillSeeds.push_back(Vector2f(66.0f, 128.0f));

    mTriangles.clear();
    TriangulateEC<float>(mPositions, mType, mEpsilon, mTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample1 ()
{
    // Polygon with one hole.  The top and bottom vertices of the outer
    // polygon are on the line containing the left edge of the inner polygon.
    // This example tests how the collinearity detection works when
    // identifying ears.

    ClearAll();
    mExample = 1;

    mPositions.resize(7);
    mPositions[0][0] =  64.0f;  mPositions[0][1] = 128.0f;
    mPositions[1][0] = 128.0f;  mPositions[1][1] =  64.0f;
    mPositions[2][0] = 192.0f;  mPositions[2][1] = 128.0f;
    mPositions[3][0] = 128.0f;  mPositions[3][1] = 192.0f;
    mPositions[4][0] = 160.0f;  mPositions[4][1] = 128.0f;
    mPositions[5][0] = 128.0f;  mPositions[5][1] =  96.0f;
    mPositions[6][0] = 128.0f;  mPositions[6][1] = 160.0f;

    mOuter.resize(4);
    mOuter[0] = 0;
    mOuter[1] = 1;
    mOuter[2] = 2;
    mOuter[3] = 3;
    mFillSeeds.push_back(Vector2f(66.0f, 128.0f));

    mInner0.resize(3);
    mInner0[0] = 4;
    mInner0[1] = 5;
    mInner0[2] = 6;

    TriangulateEC<float>(mPositions, mType, mEpsilon, mOuter, mInner0,
        mTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample2 ()
{
    // Polygon with one hole.

    ClearAll();
    mExample = 2;

    mPositions.resize(13);
    mPositions[0][0] =  29.0f;  mPositions[0][1] = 139.0f;
    mPositions[1][0] =  78.0f;  mPositions[1][1] =  99.0f;
    mPositions[2][0] = 125.0f;  mPositions[2][1] = 141.0f;
    mPositions[3][0] = 164.0f;  mPositions[3][1] = 116.0f;
    mPositions[4][0] = 201.0f;  mPositions[4][1] = 168.0f;
    mPositions[5][0] = 157.0f;  mPositions[5][1] = 163.0f;
    mPositions[6][0] = 137.0f;  mPositions[6][1] = 200.0f;
    mPositions[7][0] =  98.0f;  mPositions[7][1] = 134.0f;
    mPositions[8][0] =  52.0f;  mPositions[8][1] = 146.0f;
    mPositions[9][0] =  55.0f;  mPositions[9][1] = 191.0f;
    mPositions[10] = (mPositions[2] + mPositions[5] + mPositions[6])/3.0f;
    mPositions[11] = (mPositions[2] + mPositions[3] + mPositions[4])/3.0f;
    mPositions[12] = (mPositions[2] + mPositions[6] + mPositions[7])/3.0f;

    mOuter.resize(10);
    mOuter[0] = 5;
    mOuter[1] = 6;
    mOuter[2] = 7;
    mOuter[3] = 8;
    mOuter[4] = 9;
    mOuter[5] = 0;
    mOuter[6] = 1;
    mOuter[7] = 2;
    mOuter[8] = 3;
    mOuter[9] = 4;
    mFillSeeds.push_back(Vector2f(31.0f, 139.0f));

    mInner0.resize(3);
    mInner0[0] = 11;
    mInner0[1] = 12;
    mInner0[2] = 10;

    TriangulateEC<float>(mPositions, mType, mEpsilon, mOuter, mInner0,
        mTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample3 ()
{
    // Polygon with two holes.

    ClearAll();
    mExample = 3;

    mPositions.resize(16);
    mPositions[0][0] =  29.0f;  mPositions[0][1] = 139.0f;
    mPositions[1][0] =  78.0f;  mPositions[1][1] =  99.0f;
    mPositions[2][0] = 125.0f;  mPositions[2][1] = 141.0f;
    mPositions[3][0] = 164.0f;  mPositions[3][1] = 116.0f;
    mPositions[4][0] = 201.0f;  mPositions[4][1] = 168.0f;
    mPositions[5][0] = 157.0f;  mPositions[5][1] = 163.0f;
    mPositions[6][0] = 137.0f;  mPositions[6][1] = 200.0f;
    mPositions[7][0] =  98.0f;  mPositions[7][1] = 134.0f;
    mPositions[8][0] =  52.0f;  mPositions[8][1] = 146.0f;
    mPositions[9][0] =  55.0f;  mPositions[9][1] = 191.0f;
    mPositions[10] = (mPositions[2] + mPositions[5] + mPositions[6])/3.0f;
    mPositions[11] = (mPositions[2] + mPositions[3] + mPositions[4])/3.0f;
    mPositions[12] = (mPositions[2] + mPositions[6] + mPositions[7])/3.0f;
    mPositions[13] = (mPositions[1] + mPositions[0] + mPositions[8])/3.0f;
    mPositions[14] = (mPositions[1] + mPositions[8] + mPositions[7])/3.0f;
    mPositions[14][1] += 6.0f;
    mPositions[15] = (mPositions[1] + mPositions[7] + mPositions[2])/3.0f;

    mOuter.resize(10);
    mOuter[0] = 0;
    mOuter[1] = 1;
    mOuter[2] = 2;
    mOuter[3] = 3;
    mOuter[4] = 4;
    mOuter[5] = 5;
    mOuter[6] = 6;
    mOuter[7] = 7;
    mOuter[8] = 8;
    mOuter[9] = 9;
    mFillSeeds.push_back(Vector2f(31.0f, 139.0f));

    mInner0.resize(3);
    mInner0[0] = 11;
    mInner0[1] = 12;
    mInner0[2] = 10;
    mInners.push_back(&mInner0);

    mInner1.resize(3);
    mInner1[0] = 13;
    mInner1[1] = 14;
    mInner1[2] = 15;
    mInners.push_back(&mInner1);

    TriangulateEC<float>(mPositions, mType, mEpsilon, mOuter, mInners,
        mTriangles);
}
//----------------------------------------------------------------------------
void Triangulation::DoExample4 ()
{
    // A tree of nested polygons.

    ClearAll();
    mExample = 4;

    mPositions.resize(43);
    mPositions[ 0][0] = 102.0f;  mPositions[ 0][1] =  15.0f;
    mPositions[ 1][0] = 233.0f;  mPositions[ 1][1] =  87.0f;
    mPositions[ 2][0] = 184.0f;  mPositions[ 2][1] = 248.0f;
    mPositions[ 3][0] =  33.0f;  mPositions[ 3][1] = 232.0f;
    mPositions[ 4][0] =  14.0f;  mPositions[ 4][1] = 128.0f;
    mPositions[ 5][0] = 137.0f;  mPositions[ 5][1] =  42.0f;
    mPositions[ 6][0] =  93.0f;  mPositions[ 6][1] =  41.0f;
    mPositions[ 7][0] = 137.0f;  mPositions[ 7][1] =  79.0f;
    mPositions[ 8][0] = 146.0f;  mPositions[ 8][1] =  66.0f;
    mPositions[ 9][0] = 161.0f;  mPositions[ 9][1] = 213.0f;
    mPositions[10][0] = 213.0f;  mPositions[10][1] = 113.0f;
    mPositions[11][0] = 108.0f;  mPositions[11][1] =  67.0f;
    mPositions[12][0] =  36.0f;  mPositions[12][1] = 153.0f;
    mPositions[13][0] =  89.0f;  mPositions[13][1] = 220.0f;
    mPositions[14][0] = 133.0f;  mPositions[14][1] = 186.0f;
    mPositions[15][0] = 147.0f;  mPositions[15][1] = 237.0f;
    mPositions[16][0] = 177.0f;  mPositions[16][1] = 237.0f;
    mPositions[17][0] = 184.0f;  mPositions[17][1] = 202.0f;
    mPositions[18][0] = 159.0f;  mPositions[18][1] = 225.0f;
    mPositions[19][0] =  86.0f;  mPositions[19][1] = 113.0f;
    mPositions[20][0] = 115.0f;  mPositions[20][1] = 118.0f;
    mPositions[21][0] =  98.0f;  mPositions[21][1] = 134.0f;
    mPositions[22][0] = 109.0f;  mPositions[22][1] = 153.0f;
    mPositions[23][0] =  68.0f;  mPositions[23][1] = 133.0f;
    mPositions[24][0] =  68.0f;  mPositions[24][1] = 156.0f;
    mPositions[25][0] = 115.0f;  mPositions[25][1] = 175.0f;
    mPositions[26][0] = 108.0f;  mPositions[26][1] = 194.0f;
    mPositions[27][0] =  80.0f;  mPositions[27][1] = 192.0f;
    mPositions[28][0] = 163.0f;  mPositions[28][1] = 108.0f;
    mPositions[29][0] = 185.0f;  mPositions[29][1] = 108.0f;
    mPositions[30][0] = 172.0f;  mPositions[30][1] = 176.0f;
    mPositions[31][0] =  79.0f;  mPositions[31][1] = 170.0f;
    mPositions[32][0] =  79.0f;  mPositions[32][1] = 179.0f;
    mPositions[33][0] =  88.0f;  mPositions[33][1] = 179.0f;
    mPositions[34][0] =  88.0f;  mPositions[34][1] = 170.0f;
    mPositions[35][0] =  96.0f;  mPositions[35][1] = 179.0f;
    mPositions[36][0] =  96.0f;  mPositions[36][1] = 187.0f;
    mPositions[37][0] = 103.0f;  mPositions[37][1] = 187.0f;
    mPositions[38][0] = 103.0f;  mPositions[38][1] = 179.0f;
    mPositions[39][0] = 169.0f;  mPositions[39][1] = 121.0f;
    mPositions[40][0] = 169.0f;  mPositions[40][1] = 131.0f;
    mPositions[41][0] = 178.0f;  mPositions[41][1] = 131.0f;
    mPositions[42][0] = 178.0f;  mPositions[42][1] = 121.0f;

    // outer0 polygon
    mRoot = new0 TriangulateEC<float>::Tree();
    mRoot->Polygon.resize(5);
    mRoot->Polygon[0] = 0;
    mRoot->Polygon[1] = 1;
    mRoot->Polygon[2] = 2;
    mRoot->Polygon[3] = 3;
    mRoot->Polygon[4] = 4;
    mFillSeeds.push_back(Vector2f(82.0f, 69.0f));

    // inner0 polygon
    TriangulateEC<float>::Tree* inner0 = new0 TriangulateEC<float>::Tree();
    inner0->Polygon.resize(3);
    inner0->Polygon[0] = 5;
    inner0->Polygon[1] = 6;
    inner0->Polygon[2] = 7;
    mRoot->Child.push_back(inner0);

    // inner1 polygon
    TriangulateEC<float>::Tree* inner1 = new0 TriangulateEC<float>::Tree();
    inner1->Polygon.resize(3);
    inner1->Polygon[0] = 8;
    inner1->Polygon[1] = 9;
    inner1->Polygon[2] = 10;
    mRoot->Child.push_back(inner1);

    // inner2 polygon
    TriangulateEC<float>::Tree* inner2 = new0 TriangulateEC<float>::Tree();
    inner2->Polygon.resize(8);
    inner2->Polygon[0] = 11;
    inner2->Polygon[1] = 12;
    inner2->Polygon[2] = 13;
    inner2->Polygon[3] = 14;
    inner2->Polygon[4] = 15;
    inner2->Polygon[5] = 16;
    inner2->Polygon[6] = 17;
    inner2->Polygon[7] = 18;
    mRoot->Child.push_back(inner2);

    // outer1 polygon (contained in inner2)
    TriangulateEC<float>::Tree* outer1 = new0 TriangulateEC<float>::Tree();
    outer1->Polygon.resize(5);
    outer1->Polygon[0] = 19;
    outer1->Polygon[1] = 20;
    outer1->Polygon[2] = 21;
    outer1->Polygon[3] = 22;
    outer1->Polygon[4] = 23;
    inner2->Child.push_back(outer1);
    mFillSeeds.push_back(Vector2f(92.0f,124.0f));

    // outer2 polygon (contained in inner2)
    TriangulateEC<float>::Tree* outer2 = new0 TriangulateEC<float>::Tree();
    outer2->Polygon.resize(4);
    outer2->Polygon[0] = 24;
    outer2->Polygon[1] = 25;
    outer2->Polygon[2] = 26;
    outer2->Polygon[3] = 27;
    inner2->Child.push_back(outer2);
    mFillSeeds.push_back(Vector2f(109.0f,179.0f));

    // outer3 polygon (contained in inner1)
    TriangulateEC<float>::Tree* outer3 = new0 TriangulateEC<float>::Tree();
    outer3->Polygon.resize(3);
    outer3->Polygon[0] = 28;
    outer3->Polygon[1] = 29;
    outer3->Polygon[2] = 30;
    inner1->Child.push_back(outer3);
    mFillSeeds.push_back(Vector2f(172.0f,139.0f));

    // inner3 polygon (contained in outer2)
    TriangulateEC<float>::Tree* inner3 = new0 TriangulateEC<float>::Tree();
    inner3->Polygon.resize(4);
    inner3->Polygon[0] = 31;
    inner3->Polygon[1] = 32;
    inner3->Polygon[2] = 33;
    inner3->Polygon[3] = 34;
    outer2->Child.push_back(inner3);

    // inner4 polygon (contained in outer2)
    TriangulateEC<float>::Tree* inner4 = new0 TriangulateEC<float>::Tree();
    inner4->Polygon.resize(4);
    inner4->Polygon[0] = 35;
    inner4->Polygon[1] = 36;
    inner4->Polygon[2] = 37;
    inner4->Polygon[3] = 38;
    outer2->Child.push_back(inner4);

    // inner5 polygon (contained in outer3)
    TriangulateEC<float>::Tree* inner5 = new0 TriangulateEC<float>::Tree();
    inner5->Polygon.resize(4);
    inner5->Polygon[0] = 39;
    inner5->Polygon[1] = 40;
    inner5->Polygon[2] = 41;
    inner5->Polygon[3] = 42;
    outer3->Child.push_back(inner5);

    TriangulateEC<float>(mPositions, mType, mEpsilon, mRoot, mTriangles);
}
//----------------------------------------------------------------------------
