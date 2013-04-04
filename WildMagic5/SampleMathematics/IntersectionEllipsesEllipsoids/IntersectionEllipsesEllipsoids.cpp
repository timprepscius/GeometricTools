// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.2 (2012/07/07)

#include "IntersectionEllipsesEllipsoids.h"

WM5_CONSOLE_APPLICATION(IntersectionEllipsesEllipsoids);

//----------------------------------------------------------------------------
IntersectionEllipsesEllipsoids::IntersectionEllipsesEllipsoids ()
    :
    ConsoleApplication("SampleMathematics/IntersectionEllipsesEllipsoids")
{
}
//----------------------------------------------------------------------------
IntersectionEllipsesEllipsoids::~IntersectionEllipsesEllipsoids ()
{
}
//----------------------------------------------------------------------------
void AffineTransform (int warmups, Ellipse2f& ellipse0, Ellipse2f& ellipse1)
{
    float random = 0.0f;
    for (int i = 0; i < warmups; ++i)
    {
        random = Mathf::SymmetricRandom();
    }

    Matrix2f A;
    Vector2f B;
    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            A[row][col] = Mathf::SymmetricRandom();
        }
        B[row] = Mathf::SymmetricRandom();
    }
    if (A.Determinant() < 0.0f)
    {
        A[0][0] = -A[0][0];
        A[0][1] = -A[0][1];
    }

    Matrix2f invA = A.Inverse();
    Vector2f K = invA*(ellipse0.Center - B);
    Matrix2f D(
        1.0f/(ellipse0.Extent[0]*ellipse0.Extent[0]),
        1.0f/(ellipse0.Extent[1]*ellipse0.Extent[1]));
    Matrix2f N = A.TransposeTimes(D)*A;
    Matrix2f R;
    N.EigenDecomposition(R, D);

    ellipse0.Center = K;
    ellipse0.Axis[0] = R.GetColumn(0);
    ellipse0.Axis[1] = R.GetColumn(1);
    ellipse0.Extent[0] = Mathf::InvSqrt(D[0][0]);
    ellipse0.Extent[1] = Mathf::InvSqrt(D[1][1]);

    K = invA*(ellipse1.Center - B);
    D = Matrix2f(
        1.0f/(ellipse1.Extent[0]*ellipse1.Extent[0]),
        1.0f/(ellipse1.Extent[1]*ellipse1.Extent[1]));
    N = A.TransposeTimes(D)*A;
    N.EigenDecomposition(R, D);

    ellipse1.Center = K;
    ellipse1.Axis[0] = R.GetColumn(0);
    ellipse1.Axis[1] = R.GetColumn(1);
    ellipse1.Extent[0] = Mathf::InvSqrt(D[0][0]);
    ellipse1.Extent[1] = Mathf::InvSqrt(D[1][1]);
}
//----------------------------------------------------------------------------
void AffineTransform (int warmups, Ellipsoid3f& ellipsoid0,
    Ellipsoid3f& ellipsoid1)
{
    float random = 0.0f;
    for (int i = 0; i < warmups; ++i)
    {
        random = Mathf::SymmetricRandom();
    }

    Matrix3f A;
    Vector3f B;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            A[row][col] = Mathf::SymmetricRandom();
        }
        B[row] = Mathf::SymmetricRandom();
    }
    if (A.Determinant() < 0.0f)
    {
        A[0][0] = -A[0][0];
        A[0][1] = -A[0][1];
        A[0][2] = -A[0][2];
    }

    Matrix3f invA = A.Inverse();
    Vector3f K = invA*(ellipsoid0.Center - B);
    Matrix3f D(
        1.0f/(ellipsoid0.Extent[0]*ellipsoid0.Extent[0]),
        1.0f/(ellipsoid0.Extent[1]*ellipsoid0.Extent[1]),
        1.0f/(ellipsoid0.Extent[2]*ellipsoid0.Extent[2]));
    Matrix3f N = A.TransposeTimes(D)*A;
    Matrix3f R;
    N.EigenDecomposition(R, D);

    ellipsoid0.Center = K;
    ellipsoid0.Axis[0] = R.GetColumn(0);
    ellipsoid0.Axis[1] = R.GetColumn(1);
    ellipsoid0.Axis[2] = R.GetColumn(2);
    ellipsoid0.Extent[0] = Mathf::InvSqrt(D[0][0]);
    ellipsoid0.Extent[1] = Mathf::InvSqrt(D[1][1]);
    ellipsoid0.Extent[2] = Mathf::InvSqrt(D[2][2]);

    K = invA*(ellipsoid1.Center - B);
    D = Matrix3f(
        1.0f/(ellipsoid1.Extent[0]*ellipsoid1.Extent[0]),
        1.0f/(ellipsoid1.Extent[1]*ellipsoid1.Extent[1]),
        1.0f/(ellipsoid1.Extent[1]*ellipsoid1.Extent[1]));
    N = A.TransposeTimes(D)*A;
    N.EigenDecomposition(R, D);

    ellipsoid1.Center = K;
    ellipsoid1.Axis[0] = R.GetColumn(0);
    ellipsoid1.Axis[1] = R.GetColumn(1);
    ellipsoid1.Axis[2] = R.GetColumn(2);
    ellipsoid1.Extent[0] = Mathf::InvSqrt(D[0][0]);
    ellipsoid1.Extent[1] = Mathf::InvSqrt(D[1][1]);
    ellipsoid1.Extent[2] = Mathf::InvSqrt(D[2][2]);
}
//----------------------------------------------------------------------------
void TestEllipsesSeparated0 ()
{
    Ellipse2f ellipse0;
    ellipse0.Center = Vector2f::ZERO;
    ellipse0.Axis[0] = Vector2f::UNIT_X;
    ellipse0.Axis[1] = Vector2f::UNIT_Y;
    ellipse0.Extent[0] = 1.0f;
    ellipse0.Extent[1] = 1.0f;

    Ellipse2f ellipse1;
    ellipse1.Center = Vector2f(4.0f, 4.0f);
    ellipse1.Axis[0] = Vector2f::UNIT_X;
    ellipse1.Axis[1] = Vector2f::UNIT_Y;
    ellipse1.Extent[0] = 1.0f;
    ellipse1.Extent[1] = 1.0f;

    AffineTransform(0, ellipse0, ellipse1);

    IntrEllipse2Ellipse2f calc(ellipse0, ellipse1);
    IntrEllipse2Ellipse2f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipse2Ellipse2f::EC_ELLIPSES_SEPARATED,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsesSeparated1 ()
{
    Ellipse2f ellipse0;
    ellipse0.Center = Vector2f::ZERO;
    ellipse0.Axis[0] = Vector2f::UNIT_X;
    ellipse0.Axis[1] = Vector2f::UNIT_Y;
    ellipse0.Extent[0] = 1.0f;
    ellipse0.Extent[1] = 1.0f;

    Ellipse2f ellipse1;
    ellipse1.Center = Vector2f(4.0f, 4.0f);
    ellipse1.Axis[0] = Vector2f::UNIT_X;
    ellipse1.Axis[1] = Vector2f::UNIT_Y;
    ellipse1.Extent[0] = 0.1f;
    ellipse1.Extent[1] = 0.3f;

    AffineTransform(1, ellipse0, ellipse1);

    IntrEllipse2Ellipse2f calc(ellipse0, ellipse1);
    IntrEllipse2Ellipse2f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipse2Ellipse2f::EC_ELLIPSES_SEPARATED,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsesE0ContainsE1 ()
{
    Ellipse2f ellipse0;
    ellipse0.Center = Vector2f::ZERO;
    ellipse0.Axis[0] = Vector2f::UNIT_X;
    ellipse0.Axis[1] = Vector2f::UNIT_Y;
    ellipse0.Extent[0] = 1.0f;
    ellipse0.Extent[1] = 1.0f;

    Ellipse2f ellipse1;
    ellipse1.Center = Vector2f(0.01f, 0.02f);
    ellipse1.Axis[0] = Vector2f::UNIT_X;
    ellipse1.Axis[1] = Vector2f::UNIT_Y;
    ellipse1.Extent[0] = 0.3f;
    ellipse1.Extent[1] = 0.2f;

    AffineTransform(2, ellipse0, ellipse1);

    IntrEllipse2Ellipse2f calc(ellipse0, ellipse1);
    IntrEllipse2Ellipse2f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipse2Ellipse2f::EC_ELLIPSE0_CONTAINS_ELLIPSE1,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsesE1ContainsE0 ()
{
    Ellipse2f ellipse0;
    ellipse0.Center = Vector2f::ZERO;
    ellipse0.Axis[0] = Vector2f::UNIT_X;
    ellipse0.Axis[1] = Vector2f::UNIT_Y;
    ellipse0.Extent[0] = 1.0f;
    ellipse0.Extent[1] = 1.0f;

    Ellipse2f ellipse1;
    ellipse1.Center = Vector2f(0.01f, 0.02f);
    ellipse1.Axis[0] = Vector2f::UNIT_X;
    ellipse1.Axis[1] = Vector2f::UNIT_Y;
    ellipse1.Extent[0] = 9.0f;
    ellipse1.Extent[1] = 10.0f;

    AffineTransform(3, ellipse0, ellipse1);

    IntrEllipse2Ellipse2f calc(ellipse0, ellipse1);
    IntrEllipse2Ellipse2f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipse2Ellipse2f::EC_ELLIPSE1_CONTAINS_ELLIPSE0,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsesIntersecting ()
{
    Ellipse2f ellipse0;
    ellipse0.Center = Vector2f::ZERO;
    ellipse0.Axis[0] = Vector2f::UNIT_X;
    ellipse0.Axis[1] = Vector2f::UNIT_Y;
    ellipse0.Extent[0] = 1.0f;
    ellipse0.Extent[1] = 1.0f;

    Ellipse2f ellipse1;
    ellipse1.Center = Vector2f(0.01f, 0.02f);
    ellipse1.Axis[0] = Vector2f::UNIT_X;
    ellipse1.Axis[1] = Vector2f::UNIT_Y;
    ellipse1.Extent[0] = 0.1f;
    ellipse1.Extent[1] = 10.0f;

    AffineTransform(4, ellipse0, ellipse1);

    IntrEllipse2Ellipse2f calc(ellipse0, ellipse1);
    IntrEllipse2Ellipse2f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipse2Ellipse2f::EC_ELLIPSES_INTERSECTING,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsoidsSeparated0 ()
{
    Ellipsoid3f ellipsoid0;
    ellipsoid0.Center = Vector3f::ZERO;
    ellipsoid0.Axis[0] = Vector3f::UNIT_X;
    ellipsoid0.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid0.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid0.Extent[0] = 1.0f;
    ellipsoid0.Extent[1] = 1.0f;
    ellipsoid0.Extent[2] = 1.0f;

    Ellipsoid3f ellipsoid1;
    ellipsoid1.Center = Vector3f(4.0f, 4.0f, 4.0f);
    ellipsoid1.Axis[0] = Vector3f::UNIT_X;
    ellipsoid1.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid1.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid1.Extent[0] = 1.0f;
    ellipsoid1.Extent[1] = 1.0f;
    ellipsoid1.Extent[2] = 1.0f;

    AffineTransform(0, ellipsoid0, ellipsoid1);

    IntrEllipsoid3Ellipsoid3f calc(ellipsoid0, ellipsoid1);
    IntrEllipsoid3Ellipsoid3f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipsoid3Ellipsoid3f::EC_ELLIPSOIDS_SEPARATED,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsoidsSeparated1 ()
{
    Ellipsoid3f ellipsoid0;
    ellipsoid0.Center = Vector3f::ZERO;
    ellipsoid0.Axis[0] = Vector3f::UNIT_X;
    ellipsoid0.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid0.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid0.Extent[0] = 1.0f;
    ellipsoid0.Extent[1] = 1.0f;
    ellipsoid0.Extent[2] = 1.0f;

    Ellipsoid3f ellipsoid1;
    ellipsoid1.Center = Vector3f(4.0f, 4.0f, 4.0f);
    ellipsoid1.Axis[0] = Vector3f::UNIT_X;
    ellipsoid1.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid1.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid1.Extent[0] = 0.1f;
    ellipsoid1.Extent[1] = 0.3f;
    ellipsoid1.Extent[2] = 0.2f;

    AffineTransform(1, ellipsoid0, ellipsoid1);

    IntrEllipsoid3Ellipsoid3f calc(ellipsoid0, ellipsoid1);
    IntrEllipsoid3Ellipsoid3f::Classification type = calc.GetClassification();
    assertion(type == IntrEllipsoid3Ellipsoid3f::EC_ELLIPSOIDS_SEPARATED,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsoidsE0ContainsE1 ()
{
    Ellipsoid3f ellipsoid0;
    ellipsoid0.Center = Vector3f::ZERO;
    ellipsoid0.Axis[0] = Vector3f::UNIT_X;
    ellipsoid0.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid0.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid0.Extent[0] = 1.0f;
    ellipsoid0.Extent[1] = 1.0f;
    ellipsoid0.Extent[2] = 1.0f;

    Ellipsoid3f ellipsoid1;
    ellipsoid1.Center = Vector3f(0.01f, 0.02f, 0.03f);
    ellipsoid1.Axis[0] = Vector3f::UNIT_X;
    ellipsoid1.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid1.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid1.Extent[0] = 0.3f;
    ellipsoid1.Extent[1] = 0.2f;
    ellipsoid1.Extent[2] = 0.1f;

    AffineTransform(2, ellipsoid0, ellipsoid1);

    IntrEllipsoid3Ellipsoid3f calc(ellipsoid0, ellipsoid1);
    IntrEllipsoid3Ellipsoid3f::Classification type = calc.GetClassification();
    assertion(
        type == IntrEllipsoid3Ellipsoid3f::EC_ELLIPSOID0_CONTAINS_ELLIPSOID1,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsoidsE1ContainsE0 ()
{
    Ellipsoid3f ellipsoid0;
    ellipsoid0.Center = Vector3f::ZERO;
    ellipsoid0.Axis[0] = Vector3f::UNIT_X;
    ellipsoid0.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid0.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid0.Extent[0] = 1.0f;
    ellipsoid0.Extent[1] = 1.0f;
    ellipsoid0.Extent[2] = 1.0f;

    Ellipsoid3f ellipsoid1;
    ellipsoid1.Center = Vector3f(0.01f, 0.02f, 0.03f);
    ellipsoid1.Axis[0] = Vector3f::UNIT_X;
    ellipsoid1.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid1.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid1.Extent[0] = 9.0f;
    ellipsoid1.Extent[1] = 10.0f;
    ellipsoid1.Extent[2] = 8.0f;

    AffineTransform(3, ellipsoid0, ellipsoid1);

    IntrEllipsoid3Ellipsoid3f calc(ellipsoid0, ellipsoid1);
    IntrEllipsoid3Ellipsoid3f::Classification type = calc.GetClassification();
    assertion(
        type == IntrEllipsoid3Ellipsoid3f::EC_ELLIPSOID1_CONTAINS_ELLIPSOID0,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
void TestEllipsoidsIntersecting ()
{
    Ellipsoid3f ellipsoid0;
    ellipsoid0.Center = Vector3f::ZERO;
    ellipsoid0.Axis[0] = Vector3f::UNIT_X;
    ellipsoid0.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid0.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid0.Extent[0] = 1.0f;
    ellipsoid0.Extent[1] = 1.0f;
    ellipsoid0.Extent[2] = 1.0f;

    Ellipsoid3f ellipsoid1;
    ellipsoid1.Center = Vector3f(0.01f, 0.02f, 0.03f);
    ellipsoid1.Axis[0] = Vector3f::UNIT_X;
    ellipsoid1.Axis[1] = Vector3f::UNIT_Y;
    ellipsoid1.Axis[2] = Vector3f::UNIT_Z;
    ellipsoid1.Extent[0] = 0.1f;
    ellipsoid1.Extent[1] = 10.0f;
    ellipsoid1.Extent[2] = 8.0f;

    AffineTransform(4, ellipsoid0, ellipsoid1);

    IntrEllipsoid3Ellipsoid3f calc(ellipsoid0, ellipsoid1);
    IntrEllipsoid3Ellipsoid3f::Classification type = calc.GetClassification();
    assertion(
        type == IntrEllipsoid3Ellipsoid3f::EC_ELLIPSOIDS_INTERSECTING,
        "Incorrect result.\n");
    WM5_UNUSED(type);
}
//----------------------------------------------------------------------------
int IntersectionEllipsesEllipsoids::Main (int, char**)
{
    TestEllipsesSeparated0();
    TestEllipsesSeparated1();
    TestEllipsesE0ContainsE1();
    TestEllipsesE1ContainsE0();
    TestEllipsesIntersecting();

    TestEllipsoidsSeparated0();
    TestEllipsoidsSeparated1();
    TestEllipsoidsE0ContainsE1();
    TestEllipsoidsE1ContainsE0();
    TestEllipsoidsIntersecting();
    return 0;
}
//----------------------------------------------------------------------------
