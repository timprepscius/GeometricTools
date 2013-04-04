// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Cloth.h"

WM5_WINDOW_APPLICATION(Cloth);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
Cloth::Cloth ()
    :
    WindowApplication3("SamplePhysics/Cloth", 0, 0, 640, 480,
        Float4(0.85f, 0.85f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mSpline = 0;
    mModule = 0;
}
//----------------------------------------------------------------------------
bool Cloth::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void Cloth::OnTerminate ()
{
    delete0(mModule);
    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mCloth = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Cloth::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

#ifndef SINGLE_STEP
    PhysicsTick();
#endif

    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Cloth::OnKeyDown (unsigned char key, int x, int y)
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
    case 'G':
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void Cloth::CreateSprings ()
{
    // Set up the mass-spring system.
    int numRows = 8;
    int numCols = 16;
    float step = 0.01f;
    Vector3f gravity(0.0f, 0.0f, -1.0f);
    Vector3f wind(0.5f, 0.0f, 0.0f);
    float viscosity = 10.0f;
    float maxAmplitude = 2.0f;
    mModule = new0 PhysicsModule(numRows, numCols, step, gravity, wind,
        viscosity, maxAmplitude);

    // The top r of the mesh is immovable (infinite mass).  All other
    // masses are constant.
    int r, c;
    for (c = 0; c < numCols; ++c)
    {
        mModule->SetMass(numRows - 1, c, Mathf::MAX_REAL);
    }
    for (r = 0; r < numRows-1; ++r)
    {
        for (c = 0; c < numCols; ++c)
        {
            mModule->SetMass(r, c, 1.0f);
        }
    }

    // Initial position on a vertical axis-aligned rectangle, zero velocity.
    float rowFactor = 1.0f/(float)(numRows - 1);
    float colFactor = 1.0f/(float)(numCols - 1);
    for (r = 0; r < numRows; ++r)
    {
        for (c = 0; c < numCols; ++c)
        {
            float x = c*colFactor;
            float z = r*rowFactor;
            mModule->Position(r, c) = Vector3f(x, 0.0f, z);
            mModule->Velocity(r, c) = Vector3f::ZERO;
        }
    }

    // Springs are at rest in the initial configuration.
    const float rowConstant = 1000.0f;
    const float bottomConstant = 100.0f;
    Vector3f diff;
    for (r = 0; r < numRows; ++r)
    {
        for (c = 0; c < numCols-1; ++c)
        {
            mModule->ConstantC(r, c) = rowConstant;
            diff = mModule->Position(r, c + 1) - mModule->Position(r, c);
            mModule->LengthC(r, c) = diff.Length();
        }
    }

    for (r = 0; r < numRows-1; ++r)
    {
        for (c = 0; c < numCols; ++c)
        {
            mModule->ConstantR(r, c) = bottomConstant;
            diff = mModule->Position(r, c) - mModule->Position(r + 1, c);
            mModule->LengthR(r,c) = diff.Length();
        }
    }
}
//----------------------------------------------------------------------------
void Cloth::CreateCloth ()
{
    // Create quadratic spline using particles as control points.
    mSpline = new0 BSplineRectanglef(mModule->GetNumRows(),
        mModule->GetNumCols(), mModule->Positions2D(), 2, 2, false, false,
        true, true);

    // Generate a rectangle surface.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    int numUSamples = 16;
    int numVSamples = 32;
    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 1.0f);
    mCloth = new0 RectangleSurface(mSpline, numUSamples, numVSamples,
        vformat, &tcoordMin, &tcoordMax);

    std::string path = Environment::GetPathR("Cloth.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT);
    mCloth->SetEffectInstance(instance);

    // In case the cloth folds over, draw both sides.
    instance->GetEffect()->GetCullState(0, 0)->Enabled = false;

    mTrnNode->AttachChild(mCloth);
}
//----------------------------------------------------------------------------
void Cloth::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateSprings();
    CreateCloth();
}
//----------------------------------------------------------------------------
void Cloth::PhysicsTick ()
{
    mModule->Update((float)GetTimeInSeconds());

    // Update spline surface.  Remember that the spline maintains its own
    // copy of the control points, so this update is necessary.
    int numRows = mModule->GetNumRows();
    int numCols = mModule->GetNumCols();
    Vector3f** ctrlPoints = mModule->Positions2D();
    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            mSpline->SetControlPoint(r, c, ctrlPoints[r][c]);
        }
    }

    mCloth->UpdateSurface();
}
//----------------------------------------------------------------------------
void Cloth::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
