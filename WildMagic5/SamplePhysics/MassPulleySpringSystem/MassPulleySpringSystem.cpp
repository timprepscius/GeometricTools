// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MassPulleySpringSystem.h"

WM5_WINDOW_APPLICATION(MassPulleySpringSystem);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
MassPulleySpringSystem::MassPulleySpringSystem ()
    :
    WindowApplication3("SamplePhysics/MassPulleySpringSystem", 0, 0, 640, 480,
        Float4(0.819607f, 0.909803f, 0.713725f, 1.0f))
{
    mHelixSpline = 0;
    mCableSpline = 0;
    mLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool MassPulleySpringSystem::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    APoint camPosition(0.0f, 48.0f, 326.0f);
    AVector camDVector(0.0f, sn, -cs);
    AVector camUVector(0.0f, -cs, -sn);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    InitializeModule();
    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.1f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::OnTerminate ()
{
    mScene = 0;
    mFloor = 0;
    mWireState = 0;
    mAssembly = 0;
    mCableRoot = 0;
    mCable = 0;
    mMass1 = 0;
    mMass2 = 0;
    mPulleyRoot = 0;
    mPulley = 0;
    mSpring = 0;
    mCable = 0;
    mPlate0 = 0;
    mPlate1 = 0;
    mCylinder = 0;
    mMetalEffect = 0;
    mMetalTexture = 0;
    mSpring = 0;
    mSide0 = 0;
    mSide1 = 0;
    mTop = 0;
    mHelix = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

    float currIdle = (float)GetTimeInSeconds();
    float diff = currIdle - mLastIdle;
    if (diff >= 1.0f/30.0f)
    {
        mLastIdle = currIdle;

#ifndef SINGLE_STEP
        PhysicsTick();
#endif
        GraphicsTick();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MassPulleySpringSystem::OnKeyDown (unsigned char key, int x, int y)
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
    case 'g':  // single-step simulation
    case 'G':
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::InitializeModule ()
{
    mModule.Gravity = 1.0;
    mModule.Mass1 = 1.0;
    mModule.Mass2 = 2.0;
    mModule.Mass3 = 3.0;
    mModule.Radius = 32.0;
    mModule.Inertia = Mathd::HALF_PI*Mathd::Pow(mModule.Radius, 4.0);
    mModule.WireLength = 375.0 + Mathd::PI*mModule.Radius;
    mModule.SpringLength = 100.0;
    mModule.SpringConstant = 10.0;

    double time = 0.0;
    double deltaTime = 0.1;
    double y1 = 200.0;
    double dy1 = -10.0;
    double dy3 = -20.0;
    mModule.Initialize(time, deltaTime, y1, dy1, dy3);
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreateScene ()
{
    // set up the scene graph
    // scene -+- floor
    //        |
    //        +- assembly -+- cableRoot -+- cable
    //                     |             |
    //                     |             +- mass0
    //                     |             |
    //                     |             +- mass1
    //                     |
    //                     +- pulleyRoot -+- pulley -+- plate0
    //                                    |          |
    //                                    |          +- plate1
    //                                    |          |
    //                                    |          +- cylinder
    //                                    |
    //                                    +- spring -+- side0
    //                                               |
    //                                               +- side1
    //                                               |
    //                                               +- top
    //                                               |
    //                                               +- wire

    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mAssembly = new0 Node();
    mCableRoot = new0 Node();
    mPulleyRoot = new0 Node();
    mPulley = new0 Node();
    mSpring = new0 Node();
    mFloor = CreateFloor();
    mCable = CreateCable();
    mMass1 = CreateMass(1.0f);
    mMass2 = CreateMass(2.0f);
    CreatePulley();
    CreateSpring();
    mHelix = CreateHelix();

    mScene->AttachChild(mFloor);
    mScene->AttachChild(mAssembly);
    mAssembly->AttachChild(mCableRoot);
    mAssembly->AttachChild(mPulleyRoot);
    mCableRoot->AttachChild(mCable);
    mCableRoot->AttachChild(mMass1);
    mCableRoot->AttachChild(mMass2);
    mPulleyRoot->AttachChild(mPulley);
    mPulleyRoot->AttachChild(mSpring);
    mPulley->AttachChild(mPlate0);
    mPulley->AttachChild(mPlate1);
    mPulley->AttachChild(mCylinder);
    mSpring->AttachChild(mSide0);
    mSpring->AttachChild(mSide1);
    mSpring->AttachChild(mTop);
    mSpring->AttachChild(mHelix);

    mPulleyRoot->LocalTransform.SetTranslate(
        APoint(0.0f, (float)mModule.GetCurrentY3(), 0.0f));

    UpdateCable();
    UpdateHelix();
}
//----------------------------------------------------------------------------
TriMesh* MassPulleySpringSystem::CreateFloor ()
{
    Transform transform;
    transform.SetRotate(HMatrix(-AVector::UNIT_Z, AVector::UNIT_X,
        -AVector::UNIT_Y, APoint::ORIGIN, true));
    transform.SetTranslate(APoint(0.0f, 255.0f, 0.0f));

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat, true, false, &transform);
    TriMesh* floor = sm.Rectangle(2, 2, 1024.0f, 1024.0f);

    std::string path = Environment::GetPathR("Wood.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    floor->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return floor;
}
//----------------------------------------------------------------------------
float MassPulleySpringSystem::CableRadial (float)
{
    return 0.5f;
}
//----------------------------------------------------------------------------
TubeSurface* MassPulleySpringSystem::CreateCable ()
{
    // Create a quadratic spline for the medial axis.  The control points are
    // initially zero, but the UpdateCable() function will fill them in with
    // those points needed to define the cable.
    const int numCtrlPoints = 1024;
    Vector3f* ctrlPoints = new1<Vector3f>(numCtrlPoints);
    memset(ctrlPoints, 0, numCtrlPoints*sizeof(Vector3f));
    const int degree = 2;
    mCableSpline = new0 BSplineCurve3f(numCtrlPoints, ctrlPoints, degree,
        false, true);
    delete1(ctrlPoints);

    // Generate a tube surface whose medial axis is the spline.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 1.0f);

    TubeSurface* cable = new0 TubeSurface(mCableSpline, CableRadial, false,
        Vector3f::UNIT_Z, 128, 16, false, false, &tcoordMin, &tcoordMax,
        vformat);

    std::string path = Environment::GetPathR("Rope.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    cable->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return cable;
}
//----------------------------------------------------------------------------
TriMesh* MassPulleySpringSystem::CreateMass (float radius)
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    TriMesh* mass = StandardMesh(vformat).Sphere(8, 8, radius);

    VertexBufferAccessor vba(mass);
    Float3 gray(0.75f, 0.75f, 0.75f);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = gray;
    }

    mass->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return mass;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreatePulley ()
{
    const float thickness = 4.0f;
    const float radius = (float)mModule.Radius;

    // Shared texture for plates and cylinder of pulley.
    mMetalEffect = new0 Texture2DEffect(Shader::SF_LINEAR, Shader::SC_REPEAT,
        Shader::SC_REPEAT);
    std::string path = Environment::GetPathR("Metal.wmtf");
    mMetalTexture = Texture2D::LoadWMTF(path);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);

    mPlate0 = sm.Disk(4, 32, radius);
    mPlate0->SetEffectInstance(mMetalEffect->CreateInstance(mMetalTexture));
    mPlate0->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 0.5f*thickness));

    Transform transform;
    transform.SetRotate(HMatrix(-AVector::UNIT_X, AVector::UNIT_Y,
        AVector::UNIT_Z, APoint::ORIGIN, true));

    mPlate1 = StandardMesh(vformat, true, false, &transform).Disk(4, 32,
        radius);
    mPlate1->SetEffectInstance(mMetalEffect->CreateInstance(mMetalTexture));
    mPlate1->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, -0.5f*thickness));

    mCylinder = sm.Cylinder(2, 32, radius, thickness, true);
    mCylinder->SetEffectInstance(mMetalEffect->CreateInstance(mMetalTexture));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::CreateSpring ()
{
    const float thick = 4.0f;
    const float radius = (float)mModule.Radius;
    float xExtent = 2.0f;
    float yExtent = 18.0f;
    float zExtent = 1.0f;

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    StandardMesh sm(vformat);
    Transform transform;
    VertexColor3Effect* effect = new0 VertexColor3Effect();

    transform.SetTranslate(
        APoint(0.0f, -0.5f*radius, 0.5f*thick +zExtent));
    sm.SetTransform(transform);
    mSide0 = sm.Box(xExtent, yExtent, zExtent);
    mSide0->SetEffectInstance(effect->CreateInstance());

    transform.SetTranslate(
        APoint(0.0f, -0.5f*radius, -0.5f*thick - zExtent));
    transform.SetRotate(HMatrix(-AVector::UNIT_X, AVector::UNIT_Y,
        -AVector::UNIT_Z, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    mSide1 = sm.Box(xExtent, yExtent, zExtent);
    mSide1->SetEffectInstance(effect->CreateInstance());

    transform.SetTranslate(
        APoint(0.0f, -0.5f*radius - yExtent - 0.5f, 0.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_Z, AVector::UNIT_X,
        AVector::UNIT_Y, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    yExtent = xExtent;
    xExtent = 0.5f*thick + 2.0f;
    zExtent = 1.0f;
    mTop = sm.Box(xExtent, yExtent, zExtent);
    mTop->SetEffectInstance(effect->CreateInstance());
}
//----------------------------------------------------------------------------
float MassPulleySpringSystem::HelixRadial (float)
{
    return 0.25f;
}
//----------------------------------------------------------------------------
TubeSurface* MassPulleySpringSystem::CreateHelix ()
{
    // Create a quadratic spline for the medial axis.  The control points are
    // initially zero, but the UpdateHelix() function will fill them in with
    // those points needed to define the helix.
    const int numCtrlPoints = 1024;
    Vector3f* ctrlPoints = new1<Vector3f>(numCtrlPoints);
    memset(ctrlPoints, 0, numCtrlPoints*sizeof(Vector3f));
    const int degree = 2;
    mHelixSpline = new0 BSplineCurve3f(numCtrlPoints, ctrlPoints, degree,
        false, true);
    delete1(ctrlPoints);

    // Generate a tube surface whose medial axis is the spline.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 1.0f);

    TubeSurface* helix = new0 TubeSurface(mHelixSpline, CableRadial, false,
        Vector3f::UNIT_Z, 128, 16, false, false, &tcoordMin, &tcoordMax,
        vformat);

    helix->SetEffectInstance(mMetalEffect->CreateInstance(mMetalTexture));

    return helix;
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdatePulley ()
{
    mPulley->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z,
        (float)mModule.GetAngle()));

    mPulleyRoot->LocalTransform.SetTranslate(
        APoint(0.0f, (float)mModule.GetCurrentY3(), 0.0f));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdateCable ()
{
    // Partition control points between two vertical wires and circle piece.
    const int numCtrlPoints = mCableSpline->GetNumCtrlPoints();
    float fraction1 = (float)mModule.GetCableFraction1();
    float fraction2 = (float)mModule.GetCableFraction2();
    float fractionC = 1.0f - fraction1 - fraction2;

    int imin, imax, i;
    float mult, t;
    Vector3f ctrl(0.0f, 0.0f, 0.0f);

    // Set control points for wire from mass 1 to pulley midline.
    imin = 0;
    imax = (int)(fraction1*numCtrlPoints);
    if (imin < imax)
    {
        mult = 1.0f/(imax - imin);
        ctrl.X() = -(float)mModule.Radius;
        for (i = imin; i <= imax; ++i)
        {
            t = mult*(i - imin);
            ctrl.Y() = (1.0f - t)*(float)mModule.GetCurrentY1() +
                t*(float)mModule.GetCurrentY3();
            mCableSpline->SetControlPoint(i, ctrl);
        }
    }
    else
    {
        mCableSpline->SetControlPoint(imin, ctrl);
    }

    // Set control points for wire along hemicircle of pulley.
    imin = imax + 1;
    imax += (int)(fractionC*numCtrlPoints);
    mult = 1.0f/(imax - imin);
    for (i = imin; i <= imax; ++i)
    {
        t = -1.0f + mult*(i - imin);
        float angle = t*Mathf::PI;
        ctrl.X() = Mathf::Cos(angle)*(float)mModule.Radius;
        ctrl.Y() = (float)mModule.GetCurrentY3() +
            Mathf::Sin(angle)*(float)mModule.Radius;
        mCableSpline->SetControlPoint(i, ctrl);
    }

    // Set control points for wire from pulley midline to mass 2.
    imin = imax + 1;
    imax = numCtrlPoints - 1;
    if (imin < imax)
    {
        mult = 1.0f/(imax - imin);
        ctrl.X() = (float)mModule.Radius;
        for (i = imin; i <= imax; ++i)
        {
            t = mult*(i - imin);
            ctrl.Y() = (1.0f - t)*(float)mModule.GetCurrentY3() +
                t*(float)mModule.GetCurrentY2();
            mCableSpline->SetControlPoint(i, ctrl);
        }
    }
    else
    {
        mCableSpline->SetControlPoint(imin, ctrl);
    }

    mCable->UpdateSurface();

    // Update the mass positions.
    mMass1->LocalTransform.SetTranslate(APoint(-(float)mModule.Radius,
        (float)mModule.GetCurrentY1(), 0.0f));

    mMass2->LocalTransform.SetTranslate(APoint((float)mModule.Radius,
        (float)mModule.GetCurrentY2(), 0.0f));
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::UpdateHelix ()
{
    // The current span of the helix.
    float span = (float)(mModule.GetCurrentY3() - mModule.Radius - 4.0);

    const int numCtrlPoints = mHelixSpline->GetNumCtrlPoints();
    const float radius = 2.0f;
    const float tmax = 14.0f;
    float yMult = span/tmax;
    float delta = tmax/(float)(numCtrlPoints - 1);
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        float t = i*delta;
        float angle = Mathf::TWO_PI*t;
        float cs = Mathf::Cos(angle);
        float sn = Mathf::Sin(angle);
        Vector3f ctrl(radius*cs, -(float)mModule.Radius - 4.0f - yMult*t,
            radius*sn);
        mHelixSpline->SetControlPoint(i, ctrl);
    }

    mHelix->UpdateSurface();
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::PhysicsTick ()
{
    mModule.Update();

    UpdatePulley();
    UpdateCable();
    UpdateHelix();
    mAssembly->Update();
}
//----------------------------------------------------------------------------
void MassPulleySpringSystem::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
