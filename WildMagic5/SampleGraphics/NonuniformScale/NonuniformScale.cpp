// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "NonuniformScale.h"

WM5_WINDOW_APPLICATION(NonuniformScale);

//----------------------------------------------------------------------------
NonuniformScale::NonuniformScale ()
    :
    WindowApplication3("SampleGraphics/NonuniformScale", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool NonuniformScale::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    float cs = 0.866025f, sn = 0.5f;
    APoint camPosition(0.0f, -4.0f, 2.0f);
    AVector camDVector(0.0f, cs, -sn);
    AVector camUVector(0.0f, sn, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void NonuniformScale::OnTerminate ()
{
    mScene = 0;
    mMesh = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void NonuniformScale::OnIdle ()
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
bool NonuniformScale::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    APoint scale;

    switch (key)
    {
    case 'x':
        scale = mMesh->LocalTransform.GetScale();
        scale[0] /= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'X':
        scale = mMesh->LocalTransform.GetScale();
        scale[0] *= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'y':
        scale = mMesh->LocalTransform.GetScale();
        scale[1] /= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'Y':
        scale = mMesh->LocalTransform.GetScale();
        scale[1] *= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'z':
        scale = mMesh->LocalTransform.GetScale();
        scale[2] /= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'Z':
        scale = mMesh->LocalTransform.GetScale();
        scale[2] *= 1.1f;
        mMesh->LocalTransform.SetScale(scale);
        mMesh->Update();
        return true;
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void NonuniformScale::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mMesh = StandardMesh(vformat).Dodecahedron();
    std::string path = Environment::GetPathR("Flower.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mMesh->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));
    mScene->AttachChild(mMesh);
}
//----------------------------------------------------------------------------
