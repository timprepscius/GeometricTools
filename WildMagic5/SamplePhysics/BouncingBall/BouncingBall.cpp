// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BouncingBall.h"

WM5_WINDOW_APPLICATION(BouncingBall);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
BouncingBall::BouncingBall ()
    :
    WindowApplication3("SamplePhysics/BouncingBall", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mBall = 0;
    mSimTime = 0.0f;

#ifdef SINGLE_STEP
    mSimDelta = 0.05f;
#else
    mSimDelta = 0.005f;
#endif
}
//----------------------------------------------------------------------------
bool BouncingBall::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle);
    float sn = Mathf::Sin(angle);
    APoint camPosition(6.75f, 0.0f, 2.3f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();
    mBallNode->Update();

    // Initialize ball with correct transformations.
    PhysicsTick();
    mSimTime = 0.0f;

    // All objects are visible.
    mSceneVisibleSet.Insert(mWall);
    mBallNodeVisibleSet.Insert(mBall->GetMesh());

    InitializeCameraMotion(0.1f, 0.01f);
    return true;
}
//----------------------------------------------------------------------------
void BouncingBall::OnTerminate ()
{
    delete0(mBall);
    mPREffect = 0;
    mScene = 0;
    mBallNode = 0;
    mFloor = 0;
    mWall = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BouncingBall::OnIdle ()
{
    MeasureTime();

#ifndef SINGLE_STEP
    PhysicsTick();
#endif

    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BouncingBall::OnKeyDown (unsigned char key, int x, int y)
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
    case 's':  // toggle scaling
        mBall->DoAffine() = !mBall->DoAffine();
        return true;
#ifdef SINGLE_STEP
    case 'g':
        mSimTime += mSimDelta;
        DoPhysical();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void BouncingBall::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateBall();
    CreateFloor();
    CreateWall();
    mScene->AttachChild(mFloor);
    mScene->AttachChild(mWall);

    // The floor reflects an image of the ball.
    mPREffect = new0 PlanarReflectionEffect(1);
    mPREffect->SetPlane(0, mFloor);
    mPREffect->SetReflectance(0, 0.2f);
}
//----------------------------------------------------------------------------
void BouncingBall::CreateBall ()
{
    mBall = new0 DeformableBall(1.0f, 2.0f);
    mBallNode = new0 Node();

    TriMesh* mesh = mBall->GetMesh();
    mBallNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void BouncingBall::CreateFloor ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    const float xExtent = 8.0f;
    const float yExtent = 16.0f;
    const float zValue = 0.0f;
    vba.Position<Float3>(0) = Float3(-xExtent, -yExtent, zValue);
    vba.Position<Float3>(1) = Float3(+xExtent, -yExtent, zValue);
    vba.Position<Float3>(2) = Float3(+xExtent, +yExtent, zValue);
    vba.Position<Float3>(3) = Float3(-xExtent, +yExtent, zValue);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mFloor = new0 TriMesh(vformat, vbuffer, ibuffer);

    std::string path = Environment::GetPathR("Floor.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mFloor->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));
}
//----------------------------------------------------------------------------
void BouncingBall::CreateWall ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    const float xValue = -8.0f;
    const float yExtent = 16.0f;
    const float zExtent = 16.0f;
    vba.Position<Float3>(0) = Float3(xValue, -yExtent, 0.0f);
    vba.Position<Float3>(1) = Float3(xValue, +yExtent, 0.0f);
    vba.Position<Float3>(2) = Float3(xValue, +yExtent, zExtent);
    vba.Position<Float3>(3) = Float3(xValue, -yExtent, zExtent);

    const float maxValue = 4.0f;
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(maxValue, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(maxValue, maxValue);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, maxValue);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mWall = new0 TriMesh(vformat, vbuffer, ibuffer);

    std::string path = Environment::GetPathR("Wall1.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mWall->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));
}
//----------------------------------------------------------------------------
void BouncingBall::PhysicsTick ()
{
    // Update the ball.
    mBall->DoSimulationStep(mSimTime);
    mRenderer->Update(mBall->GetMesh()->GetVertexBuffer());

    // Get the ball parameters.
    float period = mBall->GetPeriod();
    float tMin = mBall->GetMinActive();
    float tMax = mBall->GetMaxActive();

    // Translate the ball.
    const float yMax = 2.5f, zMax = 0.75f;
    float yTrn, zTrn, ratio, amp;
    float time = fmodf(mSimTime, 2.0f*period);
    if (time < tMin)
    {
        ratio = time/tMin;
        yTrn = yMax*ratio;
        zTrn = zMax*(1.0f - ratio*ratio);
    }
    else if (time < tMax)
    {
        yTrn = yMax;
        amp = mBall->GetAmplitude(time);
        if (amp <= 0.999f)
        {
            zTrn = -(1.0f - Mathf::Sqrt(1.0f - amp + amp*amp))/(1.0f - amp);
        }
        else
        {
            zTrn = -0.5f;
        }
    }
    else if (time < period + tMin)
    {
        yTrn = -yMax*(time - period)/tMin;
        zTrn = zMax*(time - tMax)*(period + tMin - time) /
            (tMin*(period - tMax));
    }
    else if (time < period + tMax)
    {
        yTrn = -yMax;
        amp = mBall->GetAmplitude(time - period);
        if (amp <= 0.999f)
        {
            zTrn = -(1.0f - Mathf::Sqrt(1.0f - amp + amp*amp))/(1.0f - amp);
        }
        else
        {
            zTrn = -0.5f;
        }
    }
    else
    {
        yTrn = yMax*(time - 2.0f*period)/(period - tMax);
        zTrn = zMax*(time - (period + tMax))*(2.0f*period + tMin - time) /
            (tMin*(period - tMax));
    }
    mBallNode->LocalTransform.SetTranslate(APoint(0.0f, yTrn, zTrn));

    // Rotate the ball.
    float angle = Mathf::HALF_PI + 0.5f*yTrn*Mathf::PI/yMax;
    mBallNode->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, angle));

    // Update the scene graph.
    mBallNode->Update();

    // Next simulation time.
    mSimTime += mSimDelta;
}
//----------------------------------------------------------------------------
void BouncingBall::GraphicsTick ()
{
    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mSceneVisibleSet);
        mRenderer->Draw(mBallNodeVisibleSet, mPREffect);

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[256];
        sprintf(message, "time = %6.4f", mSimTime);
        mRenderer->Draw(128, GetHeight()-8, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
