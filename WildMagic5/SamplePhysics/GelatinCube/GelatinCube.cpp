// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GelatinCube.h"

WM5_WINDOW_APPLICATION(GelatinCube);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
GelatinCube::GelatinCube ()
    :
    WindowApplication3("SamplePhysics/GelatinCube", 0, 0, 640, 480,
        Float4(0.713725f, 0.807843f, 0.929411f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mSpline = 0;
    mModule = 0;
}
//----------------------------------------------------------------------------
bool GelatinCube::OnInitialize ()
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
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    // Sort the box faces based on current camera parameters.
    mBox->SortFaces(mCamera->GetDVector());

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void GelatinCube::OnTerminate ()
{
    delete0(mModule);

    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mBox = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GelatinCube::OnIdle ()
{
    MeasureTime();

    bool needSort = MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
        needSort = true;
    }
    if (needSort)
    {
        mBox->SortFaces(mCamera->GetDVector());
    }

#ifndef SINGLE_STEP
    PhysicsTick();
#endif

    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool GelatinCube::OnKeyDown (unsigned char key, int x, int y)
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
void GelatinCube::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateSprings();
    CreateBox();
}
//----------------------------------------------------------------------------
void GelatinCube::CreateSprings ()
{
    // The inner 4-by-4-by-4 particles are used as the control points of a
    // B-spline volume.  The outer layer of particles are immovable to
    // prevent the cuboid from collapsing into itself.
    int numSlices = 6;
    int numRows = 6;
    int numCols = 6;

    // Viscous forces applied.  If you set viscosity to zero, the cuboid
    // wiggles indefinitely since there is no dissipation of energy.  If
    // the viscosity is set to a positive value, the oscillations eventually
    // stop.  The length of time to steady state is inversely proportional
    // to the viscosity.
#ifdef _DEBUG
    float step = 0.1f;
#else
    float step = 0.01f;  // simulation needs to run slower in release mode
#endif
    float viscosity = 0.01f;
    mModule = new0 PhysicsModule(numSlices, numRows, numCols, step,
        viscosity);

    // The initial cuboid is axis-aligned.  The outer shell is immovable.
    // All other masses are constant.
    float sFactor = 1.0f/(float)(numSlices - 1);
    float rFactor = 1.0f/(float)(numRows - 1);
    float cFactor = 1.0f/(float)(numCols - 1);
    int s, r, c;
    for (s = 0; s < numSlices; ++s)
    {
        for (r = 0; r < numRows; ++r)
        {
            for (c = 0; c < numCols; ++c)
            {
                mModule->Position(s, r, c) =
                    Vector3f(c*cFactor, r*rFactor, s*sFactor);

                if (1 <= s && s < numSlices - 1
                &&  1 <= r && r < numRows - 1
                &&  1 <= c && c < numCols - 1)
                {
                    mModule->SetMass(s, r, c, 1.0f);
                    mModule->Velocity(s, r, c) = 0.1f*Vector3f(
                        Mathf::SymmetricRandom(), Mathf::SymmetricRandom(),
                        Mathf::SymmetricRandom());
                }
                else
                {
                    mModule->SetMass(s, r, c, Mathf::MAX_REAL);
                    mModule->Velocity(s, r, c) = Vector3f::ZERO;
                }
            }
        }
    }

    // Springs are at rest in the initial configuration.
    const float constant = 10.0f;
    Vector3f diff;

    for (s = 0; s < numSlices - 1; ++s)
    {
        for (r = 0; r < numRows; ++r)
        {
            for (c = 0; c < numCols; ++c)
            {
                mModule->ConstantS(s, r, c) = constant;
                diff = mModule->Position(s + 1, r, c) -
                    mModule->Position(s, r, c);
                mModule->LengthS(s, r, c) = diff.Length();
            }
        }
    }

    for (s = 0; s < numSlices; ++s)
    {
        for (r = 0; r < numRows - 1; ++r)
        {
            for (c = 0; c < numCols; ++c)
            {
                mModule->ConstantR(s, r, c) = constant;
                diff = mModule->Position(s, r + 1, c) -
                    mModule->Position(s, r, c);
                mModule->LengthR(s, r, c) = diff.Length();
            }
        }
    }

    for (s = 0; s < numSlices; ++s)
    {
        for (r = 0; r < numRows; ++r)
        {
            for (c = 0; c < numCols - 1; ++c)
            {
                mModule->ConstantC(s, r, c) = constant;
                diff = mModule->Position(s, r, c + 1) -
                    mModule->Position(s, r, c);
                mModule->LengthC(s, r, c) = diff.Length();
            }
        }
    }
}
//----------------------------------------------------------------------------
void GelatinCube::CreateBox ()
{
    // Create a quadratic spline using the interior particles as control
    // points.
    int numSlices = mModule->GetNumSlices() - 2;
    int numRows = mModule->GetNumRows() - 2;
    int numCols = mModule->GetNumCols() - 2;
    mSpline = new0 BSplineVolumef(numCols, numRows, numSlices, 2, 2, 2);

    for (int s = 0; s < numSlices; ++s)
    {
        for (int r = 0; r < numRows; ++r)
        {
            for (int c = 0; c < numCols; ++c)
            {
                mSpline->SetControlPoint(c, r, s,
                    mModule->Position(s + 1, r + 1, c + 1));
            }
        }
    }

    // Generate the box.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    VertexFormat* vformats[6] =
        { vformat, vformat, vformat, vformat, vformat, vformat };

    mBox = new0 BoxSurface(mSpline, 8, 8, 8, vformats);

    // The texture effect for the box faces.
    std::string path = Environment::GetPathR("WaterWithAlpha.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT);
    for (int i = 0; i < 6; ++i)
    {
        TriMesh* mesh = StaticCast<TriMesh>(mBox->GetChild(i));
        mesh->SetEffectInstance(instance);
    }

    // The texture has an alpha channel of 1/2 for all texels.
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;

    mBox->EnableSorting();
    mTrnNode->AttachChild(mBox);
}
//----------------------------------------------------------------------------
void GelatinCube::PhysicsTick ()
{
    mModule->Update((float)GetTimeInSeconds());

    // Update spline surface.  Remember that the spline maintains its own
    // copy of the control points, so this update is necessary.
    int numSlices = mModule->GetNumSlices() - 2;
    int numRows = mModule->GetNumRows() - 2;
    int numCols = mModule->GetNumCols() - 2;

    for (int s = 0; s < numSlices; ++s)
    {
        for (int r = 0; r < numRows; ++r)
        {
            for (int c = 0; c < numCols; ++c)
            {
                mSpline->SetControlPoint(c, r, s,
                    mModule->Position(s + 1, r + 1, c + 1));
            }
        }
    }

    mBox->UpdateSurface();

    for (int i = 0; i < mBox->GetNumChildren(); ++i)
    {
        TriMesh* mesh = StaticCast<TriMesh>(mBox->GetChild(i));
        mRenderer->Update(mesh->GetVertexBuffer());
    }
}
//----------------------------------------------------------------------------
void GelatinCube::GraphicsTick ()
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
