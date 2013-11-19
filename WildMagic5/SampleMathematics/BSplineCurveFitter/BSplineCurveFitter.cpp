// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BSplineCurveFitter.h"

WM5_WINDOW_APPLICATION(BSplineCurveFitter);

//----------------------------------------------------------------------------
BSplineCurveFitter::BSplineCurveFitter ()
    :
    WindowApplication3("SampleMathematics/BSplineCurveFitter", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mSpline = 0;
    mSamples = 0;
}
//----------------------------------------------------------------------------
bool BSplineCurveFitter::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, 0.0f, -4.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.1f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::OnTerminate ()
{
    delete0(mSpline);
    delete1(mSamples);

    mScene = 0;
    mEffect = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());

        sprintf(mMessage, "samples = %d, degree = %d, controls = %d",
            mNumSamples, mDegree, mNumCtrlPoints);
        mRenderer->Draw(8, 16, mTextColor, mMessage);

        sprintf(mMessage, "avr error = %f, rms error = %f", mAvrError,
            mRmsError);
        mRenderer->Draw(8, 36, mTextColor, mMessage);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BSplineCurveFitter::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'd':  // reduce the degree
        if (mDegree > 1)
        {
            --mDegree;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'D':  // increase the degree
        ++mDegree;
        CreateBSplinePolysegment();
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;

    case 's':  // (small) reduce the number of control points by 1
        if (mNumCtrlPoints - 1 > mDegree + 1)
        {
            --mNumCtrlPoints;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'S':  // (small) increase the number of control points by 1
        if (mNumCtrlPoints + 1 < mNumSamples)
        {
            ++mNumCtrlPoints;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'm':  // (medium) reduce the number of control points by 10
        if (mNumCtrlPoints - 10 > mDegree + 1)
        {
            mNumCtrlPoints -= 10;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'M':  // (medium) increase the number of control points by 10
        if (mNumCtrlPoints + 10 < mNumSamples)
        {
            mNumCtrlPoints += 10;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'l':  // (large) reduce the number of control points by 100
        if (mNumCtrlPoints - 100 > mDegree + 1)
        {
            mNumCtrlPoints -= 100;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'L':  // (large) increase the number of control points by 100
        if (mNumCtrlPoints + 100 < mNumSamples)
        {
            mNumCtrlPoints += 100;
            CreateBSplinePolysegment();
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::CreateScene ()
{
    mScene = new0 Node();

    // Generate a spiral curve on a sphere and visualize as a randomly
    // colored polyline.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    mNumSamples = 1000;
    VertexBuffer* vbuffer = new0 VertexBuffer(mNumSamples, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    mSamples = new1<Vector3f>(mNumSamples);
    float mult = 2.0f/(mNumSamples - 1), t;
    int i;
    for (i = 0; i < mNumSamples; ++i)
    {
        t = -1.0f + mult*i;
        float angle = 2.0f*Mathf::TWO_PI*t;
        float amplitude = 1.0f - t*t;

        mSamples[i].X() = amplitude*Mathf::Cos(angle);
        mSamples[i].Y() = amplitude*Mathf::Sin(angle);
        mSamples[i].Z() = t;
        mSamples[i].Normalize();
        vba.Position<Vector3f>(i) = mSamples[i];
        vba.Color<Float3>(0, i) = Float3(Mathf::UnitRandom(),
            Mathf::UnitRandom(), Mathf::UnitRandom());
    }

    mEffect = new0 VertexColor3Effect();

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(mEffect->CreateInstance());
    mScene->AttachChild(segment);

    // Generate a least-squares fit to the spiral.  The degree and number of
    // control points can be modified during run time.
    mDegree = 3;
    mNumCtrlPoints = mNumSamples/2;
    CreateBSplinePolysegment();
}
//----------------------------------------------------------------------------
void BSplineCurveFitter::CreateBSplinePolysegment ()
{
    delete0(mSpline);

    // Create the curve from the current parameters.
    mSpline = new0 BSplineCurveFitf(3, mNumSamples, (const float*)mSamples,
        mDegree, mNumCtrlPoints);

    // Sample it the same number of times as the original data.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(mNumSamples, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float mult = 1.0f/(mNumSamples - 1);
    int i;
    for (i = 0; i < mNumSamples; ++i)
    {
        mSpline->GetPosition(mult*i, (float*)vba.Position<Vector3f>(i));
    }

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(mEffect->CreateInstance());
    mScene->SetChild(1, segment);

    // Compute error measurements.
    mAvrError = 0.0f;
    mRmsError = 0.0f;
    for (i = 0; i < mNumSamples; ++i)
    {
        Vector3f diff = mSamples[i] - vba.Position<Vector3f>(i);
        float sqrLength = diff.SquaredLength();
        mRmsError += sqrLength;
        float length = Mathf::Sqrt(sqrLength);
        mAvrError += length;
    }
    mAvrError /= (float)mNumSamples;
    mRmsError /= (float)mNumSamples;
    mRmsError = Mathf::Sqrt(mRmsError);
}
//----------------------------------------------------------------------------
