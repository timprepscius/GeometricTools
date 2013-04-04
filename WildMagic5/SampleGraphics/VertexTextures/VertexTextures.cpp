// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "VertexTextures.h"
#include "DisplacementEffect.h"

WM5_WINDOW_APPLICATION(VertexTextures);

//----------------------------------------------------------------------------
VertexTextures::VertexTextures ()
    :
    WindowApplication3("SampleGraphics/VertexTextures", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool VertexTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 10000.0f);
    APoint camPosition(0.0f, 0.0f, 4.0f);
    AVector camDVector(0.0f, 0.0f, -1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

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
void VertexTextures::OnTerminate ()
{
    mScene = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VertexTextures::OnIdle ()
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
void VertexTextures::CreateScene ()
{
    mScene = new0 Node();

    // Create a flat mesh.  The heights are computed by the vertex shader.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* mesh = StandardMesh(vformat).Rectangle(32, 32, 1.0f, 1.0f);
    mScene->AttachChild(mesh);

    std::string heightName = Environment::GetPathR("HeightField.wmtf");
    Texture2D* heightTexture = Texture2D::LoadWMTF(heightName);
    DisplacementEffect* effect = new0 DisplacementEffect();
    mesh->SetEffectInstance(effect->CreateInstance(heightTexture));
}
//----------------------------------------------------------------------------
