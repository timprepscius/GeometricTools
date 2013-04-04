// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Rope.h"

WM5_WINDOW_APPLICATION(Rope);

//----------------------------------------------------------------------------
Rope::Rope ()
    :
    WindowApplication3("SamplePhysics/Rope", 0, 0, 640, 480,
        Float4(0.75f, 0.85f, 0.95f, 1.0f))
{
    mSpline = 0;
    mModule = 0;
    mLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool Rope::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    AVector camDVector(0.0f, -1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector - 0.5f*camUVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void Rope::OnTerminate ()
{
    delete0(mModule);

    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mRope = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Rope::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

    float currIdle = (float)GetTimeInSeconds();
    float diff = currIdle - mLastIdle;
    if (diff >= 0.001f)
    {
        mLastIdle = currIdle;
        PhysicsTick();
        GraphicsTick();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Rope::OnKeyDown (unsigned char key, int x, int y)
{
    int i;

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    case 'm':  // decrease mass
        if (mModule->GetMass(1) > 0.05f)
        {
            for (i = 1; i < mModule->GetNumParticles() - 1; ++i)
            {
                mModule->SetMass(i, mModule->GetMass(i) - 0.01f);
            }
        }
        return true;
    case 'M':  // increase mass
        for (i = 1; i < mModule->GetNumParticles() - 1; ++i)
        {
            mModule->SetMass(i, mModule->GetMass(i) + 0.01f);
        }
        return true;
    case 'c':  // decrease spring constant
        if (mModule->Constant(0) > 0.05f)
        {
            for (i = 0; i < mModule->GetNumSprings(); ++i)
            {
                mModule->Constant(i) -= 0.01f;
            }
        }
        return true;
    case 'C':  // increase spring constant
        for (i = 0; i < mModule->GetNumSprings(); ++i)
        {
            mModule->Constant(i) += 0.01f;
        }
        return true;
    case 'l':  // decrease spring resting length
        if (mModule->Length(0) > 0.05f)
        {
            for (i = 0; i < mModule->GetNumSprings(); ++i)
            {
                mModule->Length(i) -= 0.01f;
            }
        }
        return true;
    case 'L':  // increase spring resting length
        for (i = 0; i < mModule->GetNumSprings(); ++i)
        {
            mModule->Length(i) += 0.01f;
        }
        return true;
    case 'f':  // toggle wind force on/off
    case 'F':
        mModule->EnableWind = !mModule->EnableWind;
        return true;
    case 'r':  // toggle random wind direction change on/off
    case 'R':
        mModule->EnableWindChange = !mModule->EnableWindChange;
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void Rope::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateSprings();
    CreateRope();
}
//----------------------------------------------------------------------------
void Rope::CreateSprings ()
{
    const int numParticles = 8;
    const float step = 0.1f;
    Vector3f gravity(0.0f, 0.0f, -1.0f);
    Vector3f wind(0.0f, -0.25f, 0.0f);
    float windChangeAmplitude = 0.01f;
    float viscosity = 10.0f;
    mModule = new0 PhysicsModule(numParticles, step, gravity, wind,
        windChangeAmplitude, viscosity);

    // Constant mass at interior points (endpoints are immovable).
    mModule->SetMass(0, Mathf::MAX_REAL);
    mModule->SetMass(numParticles - 1, Mathf::MAX_REAL);
    int i;
    for (i = 1; i < numParticles - 1; ++i)
    {
        mModule->SetMass(i, 1.0f);
    }

    // Initial position on a horizontal line segment.
    float factor = 1.0f/(float)(numParticles - 1);
    for (i = 0; i < numParticles; ++i)
    {
        mModule->Position(i) = Vector3f(i*factor, 0.0f, 1.0f);
    }

    // Initial velocities are all zero.
    for (i = 0; i < numParticles; ++i)
    {
        mModule->Velocity(i) = Vector3f::ZERO;
    }

    // Springs are at rest in the initial horizontal configuration.
    int numSprings = numParticles - 1;
    float restLength = 1.0f/(float)numSprings;
    for (i = 0; i < numSprings; ++i)
    {
        mModule->Constant(i) = 10.0f;
        mModule->Length(i) = restLength;
    }
}
//----------------------------------------------------------------------------
void Rope::CreateRope ()
{
    // Create a quadratic spline using particles as control points.
    int numCtrlPoints = mModule->GetNumParticles();
    Vector3f* ctrlPoints = mModule->Positions();
    const int degree = 2;
    mSpline = new0 BSplineCurve3f(numCtrlPoints, ctrlPoints, degree, false,
        true);

    // Generate a tube surface whose medial axis is the spline.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 1.0f);

    mRope = new0 TubeSurface(mSpline, Radial, false, Vector3f::UNIT_Z, 64, 8,
        false, false, &tcoordMin, &tcoordMax, vformat);

    // Attach a texture for the rope.
    std::string path = Environment::GetPathR("Rope.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mRope->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    mTrnNode->AttachChild(mRope);
}
//----------------------------------------------------------------------------
void Rope::PhysicsTick ()
{
    // Forces are independent of time, just pass in t = 0.
    mModule->Update(0.0f);

    // Update spline curve.  Remember that the spline maintains its own copy
    // of the control points, so this update is necessary.
    int numCtrlPoints = mModule->GetNumParticles();
    Vector3f* ctrlPoints = mModule->Positions();
    for (int i = 0; i < numCtrlPoints; ++i)
    {
        mSpline->SetControlPoint(i, ctrlPoints[i]);
    }

    mRope->UpdateSurface();
}
//----------------------------------------------------------------------------
void Rope::GraphicsTick ()
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
