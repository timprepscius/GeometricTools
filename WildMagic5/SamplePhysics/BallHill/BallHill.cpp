// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#include "BallHill.h"

WM5_WINDOW_APPLICATION(BallHill);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
BallHill::BallHill ()
    :
    WindowApplication3("SamplePhysics/BallHill", 0, 0, 640, 480,
        Float4(0.839215f, 0.894117f, 0.972549f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool BallHill::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle);
    float sn = Mathf::Sin(angle);
    APoint camPosition(4.0f, 0.0f, 2.0f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    InitializeModule();
    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BallHill::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mGround = 0;
    mHill = 0;
    mBall = 0;
    mPath = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BallHill::OnIdle ()
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
bool BallHill::OnKeyDown (unsigned char key, int x, int y)
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
void BallHill::InitializeModule ()
{
    mModule.Gravity = 1.0;
    mModule.A1 = 2.0;
    mModule.A2 = 1.0;
    mModule.A3 = 1.0;
    mModule.Radius = 0.1;

    double time = 0.0;
    double deltaTime = 0.01;
    double y1 = 0.0;
    double y2 = 0.0;
    double y1Dot = 0.1;
    double y2Dot = 0.1;
    mModule.Initialize(time, deltaTime, y1, y2, y1Dot, y2Dot);
}
//----------------------------------------------------------------------------
void BallHill::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mScene->AttachChild(CreateGround());
    mScene->AttachChild(CreateHill());
    mScene->AttachChild(CreateBall());
    mScene->AttachChild(CreatePath());
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateGround ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    // Create the ground.  Change the texture repeat pattern.
    mGround = StandardMesh(vformat).Rectangle(2, 2, 32.0f, 32.0f);
    VertexBufferAccessor vba(mGround);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 8.0f;
        tcoord[1] *= 8.0f;
    }

    // Create a texture effect for the ground.
    std::string path = Environment::GetPathR("Grass.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mGround->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return mGround;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateHill ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    // Create the hill.  Change the texture repeat pattern.
    mHill = StandardMesh(vformat).Disk(32, 32, 2.0f);
    VertexBufferAccessor vba(mHill);
    const int numVertices = vba.GetNumVertices();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 8.0f;
        tcoord[1] *= 8.0f;
    }

    // Adjust disk vertices to form elliptical paraboloid for the hill.
    for (i = 0; i < numVertices; ++i)
    {
        Float3& position = vba.Position<Float3>(i);
        position[2] = mModule.GetHeight(position[0], position[1]);
    }

    mHill->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);

    // Create a texture effect for the hill.
    std::string path = Environment::GetPathR("Gravel.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mHill->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return mHill;
}
//----------------------------------------------------------------------------
TriMesh* BallHill::CreateBall ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    // Create the ball.
    mRadius = (float)mModule.Radius;
    mBall = StandardMesh(vformat).Sphere(16, 16, mRadius);

    // Move the ball to the top of the hill.
    APoint trn = mBall->LocalTransform.GetTranslate();
    trn[2] = (float)mModule.A3 + mRadius;
    mBall->LocalTransform.SetTranslate(trn);

    // Create a texture effect for the ball.
    std::string path = Environment::GetPathR("BallTexture.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mBall->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    UpdateBall();

    return mBall;
}
//----------------------------------------------------------------------------
Polysegment* BallHill::CreatePath ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    // Create the path.
    const int numVertices = 1024;
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    mPath = new0 Polysegment(vformat, vbuffer, true);
    mPath->SetNumSegments(0);
    mNextPoint = 0;

    // Initialize all path points to the origin.  The points will be set
    // dynamically during the simulation.
    VertexBufferAccessor vba(mPath);

    // Points used to display path of ball.
    Float3 zero(0.0f, 0.0f, 0.0f);
    Float3 white(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numVertices; ++i)
    {
        vba.Position<Float3>(i) = zero;
        vba.Color<Float3>(0, i) = white;
    }

    mPath->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);

    // Create a vertex color effect for the path.
    mPath->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return mPath;
}
//----------------------------------------------------------------------------
APoint BallHill::UpdateBall ()
{
    // Compute the location of the center of the ball and the incremental
    // rotation implied by its motion.
    APoint center;
    HMatrix incrRot;
    mModule.GetData(center, incrRot);

    // Update the ball position and orientation.
    mBall->LocalTransform.SetTranslate(center);
    mBall->Update();
    mBall->LocalTransform.SetRotate(
        incrRot*mBall->LocalTransform.GetRotate());

    // Return the new ball center for further use by application.
    return center;
}
//----------------------------------------------------------------------------
void BallHill::PhysicsTick ()
{
    // Allow motion only while ball is above the ground level.
    if (mBall->LocalTransform.GetTranslate().Z() <= mRadius)
    {
        return;
    }

    // Move the ball.
    mModule.Update();
    APoint center = UpdateBall();

    // Draw only the active quantity of path points for the initial portion
    // of the simulation.  Once all points are activated, then all are drawn.
    VertexBufferAccessor vba(mPath);
    int numSegments = mPath->GetNumSegments();
    if (numSegments < mPath->GetMaxNumSegments())
    {
        mPath->SetNumSegments(++numSegments);
    }

    // Update the path that the ball has followed.
    if (mNextPoint < vba.GetNumVertices())
    {
        vba.Position<Float3>(mNextPoint++) = center;
        if (numSegments == 1)
        {
            vba.Position<Float3>(mNextPoint++) = center;
        }
        mPath->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    }

    mRenderer->Update(mPath->GetVertexBuffer());
    mScene->Update();
}
//----------------------------------------------------------------------------
void BallHill::GraphicsTick ()
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
