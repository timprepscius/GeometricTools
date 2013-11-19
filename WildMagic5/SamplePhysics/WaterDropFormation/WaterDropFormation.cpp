// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "WaterDropFormation.h"

WM5_WINDOW_APPLICATION(WaterDropFormation);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
WaterDropFormation::WaterDropFormation ()
    :
    WindowApplication3("SamplePhysics/WaterDropFormation", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mSpline = 0;
    mCircle = 0;
    mCtrlPoints = 0;
    mTargets = 0;
    mSimTime = 0.0f;
    mSimDelta = 0.05f;
}
//----------------------------------------------------------------------------
bool WaterDropFormation::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 1000.0f);
    float angle = 0.01f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(sn, 0.0f, -cs);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        0.9f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);

    mLastSeconds = (float)GetTimeInSeconds();
    return true;
}
//----------------------------------------------------------------------------
void WaterDropFormation::OnTerminate ()
{
    delete0(mSpline);
    delete0(mCircle);
    delete1(mCtrlPoints);
    delete1(mTargets);

    mScene = 0;
    mTrnNode = 0;
    mWaterRoot = 0;
    mWireState = 0;
    mPlane = 0;
    mWall = 0;
    mWaterSurface = 0;
    mWaterDrop = 0;
    mWaterEffect = 0;
    mWaterTexture = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void WaterDropFormation::OnIdle ()
{
    MeasureTime();

#ifndef SINGLE_STEP
    float currSeconds = (float)GetTimeInSeconds();
    float diff = currSeconds - mLastSeconds;
    if (diff >= 0.033333f)
    {
        PhysicsTick();
        mCuller.ComputeVisibleSet(mScene);
        mLastSeconds = currSeconds;
    }
#endif

    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool WaterDropFormation::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreatePlane();
    CreateWall();
    CreateWaterRoot();

    Configuration0();
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreatePlane ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mPlane = StandardMesh(vformat).Rectangle(2, 2, 8.0f, 16.0f);

    std::string path = Environment::GetPathR("StoneCeiling.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mPlane->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    mTrnNode->AttachChild(mPlane);
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreateWall ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);

    Transform transform;
    transform.SetTranslate(APoint(-8.0f, 0.0f, 8.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_Y, AVector::UNIT_Z,
        AVector::UNIT_X, APoint::ORIGIN, true));
    sm.SetTransform(transform);

    mWall = sm.Rectangle(2, 2, 16.0f, 8.0f);

    std::string path = Environment::GetPathR("Stone.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mWall->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    mTrnNode->AttachChild(mWall);
}
//----------------------------------------------------------------------------
void WaterDropFormation::CreateWaterRoot ()
{
    mWaterRoot = new0 Node();
    mTrnNode->AttachChild(mWaterRoot);
    mWaterRoot->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 0.1f));
    mWaterRoot->LocalTransform.SetUniformScale(8.0f);

    // The texture for the water objects.  This will be attached to children
    // of mWaterRoot when the need arises.
    mWaterEffect =  new0 Texture2DEffect();
    std::string path = Environment::GetPathR("WaterWithAlpha.wmtf");
    mWaterTexture = Texture2D::LoadWMTF(path);

    // The texture has an alpha channel of 1/2.
    mWaterEffect->GetAlphaState(0, 0)->BlendEnabled = true;
}
//----------------------------------------------------------------------------
void WaterDropFormation::Configuration0 ()
{
    // Application loops between Configuration0() and Configuration1().
    // Delete all the objects from "1" when restarting with "0".
    delete1(mCtrlPoints);
    delete1(mTargets);
    delete0(mSpline);
    delete0(mCircle);
    mCircle = 0;
    mSimTime = 0.0f;
    mSimDelta = 0.05f;

    mWaterRoot->DetachChildAt(0);
    mWaterRoot->DetachChildAt(1);
    mWaterSurface = 0;
    mWaterDrop = 0;

    // Create water surface curve of revolution.
    const int numCtrlPoints = 13;
    const int degree = 2;
    mCtrlPoints = new1<Vector2f>(numCtrlPoints);
    mTargets = new1<Vector2f>(numCtrlPoints);
    int i;
    for (i = 0; i < numCtrlPoints; ++i)
    {
        mCtrlPoints[i] = Vector2f(0.125f + 0.0625f*i, 0.0625f);
    }

    float h = 0.5f;
    float d = 0.0625f;
    float extra = 0.1f;

    mTargets[ 0] = mCtrlPoints[ 0];
    mTargets[ 1] = mCtrlPoints[ 6];
    mTargets[ 2] = Vector2f(mCtrlPoints[6].X(), h - d - extra);
    mTargets[ 3] = Vector2f(mCtrlPoints[5].X(), h - d - extra);
    mTargets[ 4] = Vector2f(mCtrlPoints[5].X(), h);
    mTargets[ 5] = Vector2f(mCtrlPoints[5].X(), h + d);
    mTargets[ 6] = Vector2f(mCtrlPoints[6].X(), h + d);
    mTargets[ 7] = Vector2f(mCtrlPoints[7].X(), h + d);
    mTargets[ 8] = Vector2f(mCtrlPoints[7].X(), h);
    mTargets[ 9] = Vector2f(mCtrlPoints[7].X(), h - d - extra);
    mTargets[10] = Vector2f(mCtrlPoints[6].X(), h - d - extra);
    mTargets[11] = mCtrlPoints[ 6];
    mTargets[12] = mCtrlPoints[12];

    float* weights = new1<float>(numCtrlPoints);
    for (i = 0; i < numCtrlPoints; ++i)
    {
        weights[i] = 1.0f;
    }

    const float modWeight = 0.3f;
    weights[3] = modWeight;
    weights[5] = modWeight;
    weights[7] = modWeight;
    weights[9] = modWeight;

    mSpline = new0 NURBSCurve2f(numCtrlPoints, mCtrlPoints, weights, degree,
        false, true);

    // Restrict evaluation to a subinterval of the domain.
    mSpline->SetTimeInterval(0.5f, 1.0f);

    delete1(weights);

    // Create the water surface.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mWaterSurface = new0 RevolutionSurface(mSpline, mCtrlPoints[6].X(),
        RevolutionSurface::REV_DISK_TOPOLOGY, 32, 16, false, true, vformat);
    mWaterSurface->SetEffectInstance(
        mWaterEffect->CreateInstance(mWaterTexture));

    mWaterRoot->AttachChild(mWaterSurface);
    mWaterRoot->Update();
}
//----------------------------------------------------------------------------
void WaterDropFormation::Configuration1 ()
{
    delete1(mTargets);
    mTargets = 0;

    const int numCtrlPoints = 14;
    const int degree = 2;
    delete1(mCtrlPoints);
    mCtrlPoints = new1<Vector2f>(numCtrlPoints);
    float* weights = new1<float>(numCtrlPoints);

    // spline
    mCtrlPoints[0] = mSpline->GetControlPoint(0);
    mCtrlPoints[1] = mSpline->GetControlPoint(1);
    mCtrlPoints[2] = 0.5f*(mSpline->GetControlPoint(1) +
        mSpline->GetControlPoint(2));
    mCtrlPoints[3] = mSpline->GetControlPoint(11);
    mCtrlPoints[4] = mSpline->GetControlPoint(12);

    // circle
    int i, j;
    for (i = 2, j = 5; i <= 10; ++i, ++j)
    {
        mCtrlPoints[j] = mSpline->GetControlPoint(i);
    }
    mCtrlPoints[5] = 0.5f*(mCtrlPoints[2] + mCtrlPoints[5]);
    mCtrlPoints[13] = mCtrlPoints[5];

    for (i = 0; i < numCtrlPoints; ++i)
    {
        weights[i] = 1.0f;
    }

    weights[ 6] = mSpline->GetControlWeight(3);
    weights[ 8] = mSpline->GetControlWeight(5);
    weights[10] = mSpline->GetControlWeight(7);
    weights[12] = mSpline->GetControlWeight(9);

    delete0(mSpline);
    mSpline = new0 NURBSCurve2f(5, mCtrlPoints, weights ,degree, false,
        true);

    // Restrict evaluation to a subinterval of the domain.
    mSpline->SetTimeInterval(0.5f, 1.0f);

    mWaterSurface->SetCurve(mSpline);

    mCircle = new0 NURBSCurve2f(9, &mCtrlPoints[5], &weights[5], degree,
        true, false);

    delete1(weights);

    // Restrict evaluation to a subinterval of the domain.  Why 0.375?  The
    // circle NURBS is a loop and not open.  The curve is constructed with
    // iDegree (2) replicated control points.  Although the curve is
    // geometrically symmetric about the vertical axis, it is not symmetric
    // in t about the half way point (0.5) of the domain [0,1].
    mCircle->SetTimeInterval(0.375f, 1.0f);

    // Create water drop.  The outside view value is set to 'false' because
    // the curve (x(t),z(t)) has the property dz/dt < 0.  If the curve
    // instead had the property dz/dt > 0, then 'true' is the correct value
    // for the outside view.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mWaterDrop = new0 RevolutionSurface(mCircle, mCtrlPoints[9].X(),
        RevolutionSurface::REV_SPHERE_TOPOLOGY, 32, 16, false, false,
        vformat);
    mWaterDrop->SetEffectInstance(
        mWaterEffect->CreateInstance(mWaterTexture));

    mWaterRoot->AttachChild(mWaterDrop);
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical1 ()
{
    // Modify control points.
    float t = mSimTime, oneMinusT = 1.0f - t;
    float t2 = t*t, oneMinusT2 = 1.0f - t2;
    int numControlPOints = mSpline->GetNumCtrlPoints();
    for (int i = 0; i < numControlPOints; ++i)
    {
        if (i != 4)
        {
            mSpline->SetControlPoint(i, oneMinusT*mCtrlPoints[i] +
                t*mTargets[i]);
        }
        else
        {
            mSpline->SetControlPoint(i, oneMinusT2*mCtrlPoints[i] +
                t2*mTargets[i]);
        }
    }

    // Modify mesh vertices.
    mWaterSurface->UpdateSurface();
    mScene->Update();
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical2 ()
{
    if (!mCircle)
    {
        Configuration1();
    }

    mSimTime += mSimDelta;

    // Surface evolves to a disk.
    float t = mSimTime - 1.0f, oneMinusT = 1.0f - t;
    Vector2f newCtrl = oneMinusT*mSpline->GetControlPoint(2) +
        t*mSpline->GetControlPoint(1);
    mSpline->SetControlPoint(2, newCtrl);

    // Sphere floats down a little bit.
    int numCtrlPoints = mCircle->GetNumCtrlPoints();
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        newCtrl = mCircle->GetControlPoint(i) + Vector2f::UNIT_Y/32.0f;
        mCircle->SetControlPoint(i, newCtrl);
    }

    mWaterSurface->UpdateSurface();
    mWaterDrop->UpdateSurface();
    mScene->Update();
}
//----------------------------------------------------------------------------
void WaterDropFormation::DoPhysical3 ()
{
    mSimTime += mSimDelta;

    // Sphere floats down a little bit.
    int numCtrlPoints = mCircle->GetNumCtrlPoints();
    int i;
    for (i = 0; i < numCtrlPoints; ++i)
    {
        Vector2f newCtrl = mCircle->GetControlPoint(i);
        if (i == 0 || i == numCtrlPoints - 1)
        {
            newCtrl += 1.3f*Vector2f::UNIT_Y/32;
        }
        else
        {
            newCtrl += Vector2f::UNIT_Y/32;
        }
        mCircle->SetControlPoint(i, newCtrl);
    }

    mWaterDrop->UpdateSurface();
    mScene->Update();
}
//----------------------------------------------------------------------------
void WaterDropFormation::PhysicsTick ()
{
    mSimTime += mSimDelta;
    if (mSimTime <= 1.0f)
    {
        // Water surface extruded to form a water drop.
        DoPhysical1();
    }
    else if (mSimTime <= 2.0f)
    {
        // Water drop splits from water surface.
        DoPhysical2();
    }
    else if (mSimTime <= 4.0f)
    {
        // Water drop continues downward motion, surface no longer changes.
        DoPhysical3();
    }
    else
    {
        // Restart the animation.
        Configuration0();
    }
}
//----------------------------------------------------------------------------
void WaterDropFormation::GraphicsTick ()
{
    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[256];
        sprintf(message, "time = %6.4f", mSimTime);
        mRenderer->Draw(96, GetHeight()-8, mTextColor, message);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
