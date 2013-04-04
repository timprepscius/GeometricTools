// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "HelixTubeSurface.h"

WM5_WINDOW_APPLICATION(HelixTubeSurface);

//----------------------------------------------------------------------------
HelixTubeSurface::HelixTubeSurface ()
    :
    WindowApplication3("SamplePhysics/HelixTubeSurface", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mDeltaTime = 0.01f;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.  The coordinate frame will be set by the MoveCamera
    // member function.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 10.0f);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // The initial culling of scene.  The visible set is constructed in the
    // MoveCamera call.
    mCuller.SetCamera(mCamera);

    MoveCamera();
    return true;
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void HelixTubeSurface::OnIdle ()
{
    MeasureTime();

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    case '+':
    case '=':
        mDeltaTime *= 2.0f;
        return true;
    case '-':
    case '_':
        mDeltaTime *= 0.5f;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::OnSpecialKeyDown (int key, int, int)
{
    if (key == KEY_UP_ARROW)
    {
        mCurveTime += mDeltaTime;
        if (mCurveTime > mMaxCurveTime)
        {
            mCurveTime -= mCurvePeriod;
        }
        MoveCamera();
        return true;
    }

    if (key == KEY_DOWN_ARROW)
    {
        mCurveTime -= mDeltaTime;
        if (mCurveTime < mMinCurveTime)
        {
            mCurveTime += mCurvePeriod;
        }
        MoveCamera();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void HelixTubeSurface::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mCurve = CreateCurve();
    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 32.0f);
    TubeSurface* tube = new TubeSurface(mCurve, Radial, false,
        Vector3f::UNIT_Z, 256, 32, false, true, &tcoordMin, &tcoordMax,
        vformat);

    std::string path = Environment::GetPathR("Grating.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    tube->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    mScene->AttachChild(tube);
}
//----------------------------------------------------------------------------
bool HelixTubeSurface::MoveCamera ()
{
    APoint position = mCurve->GetPosition(mCurveTime);
    AVector tangent = mCurve->GetTangent(mCurveTime);
    AVector binormal = tangent.UnitCross(AVector::UNIT_Z);
    AVector normal = binormal.UnitCross(tangent);
    mCamera->SetFrame(position, tangent, normal, binormal);
    mCuller.ComputeVisibleSet(mScene);
    return true;
}
//----------------------------------------------------------------------------
MultipleCurve3f* HelixTubeSurface::CreateCurve ()
{
    // Sample points on a looped helix (first and last point must match).
    const float fourPi = 4.0f*Mathf::PI;
    const int numSegments = 32;
    const int numSegmentsP1 = numSegments + 1;
    float* times = new1<float>(numSegmentsP1);
    Vector3f* points = new1<Vector3f>(numSegmentsP1);
    float t;
    int i;
    for (i = 0; i <= numSegmentsP1/2; ++i)
    {
        t = i*fourPi/(float)numSegmentsP1;
        times[i] = t;
        points[i] = Vector3f(Mathf::Cos(t), Mathf::Sin(t), t);
    }

    for (i = numSegmentsP1/2 + 1; i < numSegments; ++i)
    {
        t = i*fourPi/(float)numSegments;
        times[i] = t;
        points[i] = Vector3f(2.0f - Mathf::Cos(t), Mathf::Sin(t), fourPi - t);
    }

    times[numSegments] = fourPi;
    points[numSegments] = points[0];

    // Save min and max times.
    mMinCurveTime = 0.0f;
    mMaxCurveTime = fourPi;
    mCurvePeriod = mMaxCurveTime - mMinCurveTime;
    mCurveTime = mMinCurveTime;

    // Create a closed cubic curve containing the sample points.
    NaturalSpline3f* curve = new0 NaturalSpline3f(
        NaturalSpline3f::BT_CLOSED, numSegments, times, points);

    return curve;
}
//----------------------------------------------------------------------------
float HelixTubeSurface::Radial (float)
{
    return 0.0625f;
}
//----------------------------------------------------------------------------
