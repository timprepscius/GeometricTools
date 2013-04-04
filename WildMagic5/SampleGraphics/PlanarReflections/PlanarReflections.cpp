// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PlanarReflections.h"

WM5_WINDOW_APPLICATION(PlanarReflections);

//----------------------------------------------------------------------------
PlanarReflections::PlanarReflections ()
    :
    WindowApplication3("SampleGraphics/PlanarReflections", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mUpdateTime = 0.0;
}
//----------------------------------------------------------------------------
bool PlanarReflections::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(180.0f, 0.0f, 23.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();
    mBiped->Update(mUpdateTime);

    // Initial culling of scene,
    mSceneCuller.SetCamera(mCamera);
    mSceneCuller.ComputeVisibleSet(mScene);
    mBipedCuller.SetCamera(mCamera);
    mBipedCuller.ComputeVisibleSet(mBiped);

    InitializeCameraMotion(0.1f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void PlanarReflections::OnTerminate ()
{
    mScene = 0;
    mBiped = 0;
    mPlane0 = 0;
    mPlane1 = 0;
    mWireState = 0;
    mPREffect = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PlanarReflections::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mSceneCuller.ComputeVisibleSet(mScene);
        mBipedCuller.ComputeVisibleSet(mBiped);
    }

    if (MoveObject())
    {
        mScene->Update();
        mBiped->Update(mUpdateTime);
        mSceneCuller.ComputeVisibleSet(mScene);
        mBipedCuller.ComputeVisibleSet(mBiped);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mSceneCuller.GetVisibleSet());
        mRenderer->Draw(mBipedCuller.GetVisibleSet(), mPREffect);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool PlanarReflections::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    case 'g':
        mUpdateTime += 0.01;
        mBiped->Update(mUpdateTime);
        return true;
    case 'G':
        mUpdateTime = 0.0;
        mBiped->Update(mUpdateTime);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void PlanarReflections::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    LoadBiped();
    CreatePlanes();
    CreatePlanarReflection();
    mScene->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, 0.75f));
}
//----------------------------------------------------------------------------
void PlanarReflections::LoadBiped ()
{
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("SkinnedBipedPN.wmof");
#else
    std::string path = Environment::GetPathR("SkinnedBipedPN.be.wmof");
#endif
    InStream source;
    source.Load(path);
    mBiped = (Node*)source.GetObjectAt(0);

    // This guarantees that the reflection is drawn, even when the biped is
    // not visible to the camera.
    mBiped->Culling = Spatial::CULL_NEVER;
}
//----------------------------------------------------------------------------
void PlanarReflections::CreatePlanes ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    // Create the floor mesh.
    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor floor(vformat, vbuffer);

    float xValue = 128.0f;
    float yValue = 256.0f;
    float zValue = 0.0f;
    floor.Position<Float3>(0) = Float3(-xValue, -yValue, zValue);
    floor.Position<Float3>(1) = Float3(+xValue, -yValue, zValue);
    floor.Position<Float3>(2) = Float3(+xValue, +yValue, zValue);
    floor.Position<Float3>(3) = Float3(-xValue, +yValue, zValue);
    floor.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    floor.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    floor.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    floor.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mPlane0 = new0 TriMesh(vformat, vbuffer, ibuffer);

    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR,
        Shader::SC_REPEAT, Shader::SC_REPEAT);
    std::string path = Environment::GetPathR("Sand.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mPlane0->SetEffectInstance(effect->CreateInstance(texture));

    mScene->AttachChild(mPlane0);

    // Create the wall mesh.
    vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor wall(vformat, vbuffer);

    xValue = -128.0f;
    yValue = 256.0f;
    zValue = 128.0f;
    wall.Position<Float3>(0) = Float3(xValue, -yValue, 0.0f);
    wall.Position<Float3>(1) = Float3(xValue, +yValue, 0.0f);
    wall.Position<Float3>(2) = Float3(xValue, +yValue, zValue);
    wall.Position<Float3>(3) = Float3(xValue, -yValue, zValue);
    wall.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    wall.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    wall.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    wall.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    mPlane1 = new0 TriMesh(vformat, vbuffer, ibuffer);

    path = Environment::GetPathR("Stone.wmtf");
    texture = Texture2D::LoadWMTF(path);
    mPlane1->SetEffectInstance(effect->CreateInstance(texture));

    mScene->AttachChild(mPlane1);
}
//----------------------------------------------------------------------------
void PlanarReflections::CreatePlanarReflection ()
{
    Light* projector = new0 Light(Light::LT_DIRECTIONAL);
    projector->DVector = -AVector::UNIT_X;
    LightNode* projectorNode = new0 LightNode(projector);
    mScene->AttachChild(projectorNode);

    mPREffect = new0 PlanarReflectionEffect(2);
    mPREffect->SetPlane(0, mPlane0);
    mPREffect->SetReflectance(0, 0.1f);
    mPREffect->SetPlane(1, mPlane1);
    mPREffect->SetReflectance(1, 0.5f);
}
//----------------------------------------------------------------------------
