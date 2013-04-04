// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BSplineFitContinuous.h"

WM5_WINDOW_APPLICATION(BSplineFitContinuous);

//----------------------------------------------------------------------------
BSplineFitContinuous::BSplineFitContinuous ()
    :
    WindowApplication3("SampleMathematics/BSplineFitContinuous", 0, 0, 512,
        512, Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mDegree = 3;
}
//----------------------------------------------------------------------------
bool BSplineFitContinuous::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 10000.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(10.0f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineFitContinuous::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineFitContinuous::OnIdle ()
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
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void BSplineFitContinuous::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);

    // Get control points for input curve.
    std::string path = Environment::GetPathR("ControlPoints.txt");
    std::ifstream inFile(path.c_str());
    int numCtrlPoints;
    inFile >> numCtrlPoints;
    Vector3d* ctrlPoints = new1<Vector3d>(numCtrlPoints);
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        inFile >> ctrlPoints[i][0];
        inFile >> ctrlPoints[i][1];
        inFile >> ctrlPoints[i][2];
    }

    // Create polysegment connecting control points of input B-spline curve.
    double fraction = 0.10;
    Polysegment* segment = OriginalPolysegment(numCtrlPoints, ctrlPoints);
    mTrnNode->AttachChild(segment);

    // Create polysegment that approximates the reduced B-spline curve.
    segment = ReducedPolysegment(numCtrlPoints, ctrlPoints, fraction);
    mTrnNode->AttachChild(segment);
    delete1(ctrlPoints);
}
//----------------------------------------------------------------------------
Polysegment* BSplineFitContinuous::OriginalPolysegment (int numCtrlPoints,
    Vector3d* ctrlPoints)
{
    BSplineCurve3d spline(numCtrlPoints, ctrlPoints, mDegree, false, true);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numCtrlPoints, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 red(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        double t = i/(double)numCtrlPoints;
        Vector3d pos = spline.GetPosition(t);
        vba.Position<Float3>(i) = Float3((float)pos[0], (float)pos[1],
            (float)pos[2]);
        vba.Color<Float3>(0, i) = red;
    }

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    return segment;
}
//----------------------------------------------------------------------------
Polysegment* BSplineFitContinuous::ReducedPolysegment (int numCtrlPoints,
    Vector3d* ctrlPoints, double fraction)
{
    int numLSCtrlPoints;
    Vector3d* lsCtrlPoints;
    BSplineReduction3d(numCtrlPoints, ctrlPoints, mDegree, fraction,
        numLSCtrlPoints, lsCtrlPoints);

    BSplineCurve3d spline(numLSCtrlPoints, lsCtrlPoints, mDegree, false,
        true);
    delete1(lsCtrlPoints);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numCtrlPoints, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 blue(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        double t = i/(double)numCtrlPoints;
        Vector3d pos = spline.GetPosition(t);
        vba.Position<Float3>(i) = Float3((float)pos[0], (float)pos[1],
            (float)pos[2]);
        vba.Color<Float3>(0, i) = blue;
    }

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    return segment;
}
//----------------------------------------------------------------------------
