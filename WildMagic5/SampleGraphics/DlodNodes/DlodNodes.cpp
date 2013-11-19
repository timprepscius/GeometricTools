// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "DlodNodes.h"

WM5_WINDOW_APPLICATION(DlodNodes);

//----------------------------------------------------------------------------
DlodNodes::DlodNodes ()
    :
    WindowApplication3("SampleGraphics/DlodNodes", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool DlodNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    APoint camPosition(0.0f, -4.0f, 0.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
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
void DlodNodes::OnTerminate ()
{
    mScene = 0;
    mDlod = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void DlodNodes::OnIdle ()
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
bool DlodNodes::OnKeyDown (unsigned char key, int x, int y)
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
    }

    return false;
}
//----------------------------------------------------------------------------
void DlodNodes::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    const int numLevelsOfDetail = 6; // the number of children of mDlod
    mDlod = new0 DlodNode(numLevelsOfDetail);
    mScene->AttachChild(mDlod);

    // A lighting effect to be shared by all objects.
    Light* light = new0 Light(Light::LT_DIRECTIONAL);
    light->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    light->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    light->Specular = Float4(0.0f, 0.0f, 0.0f, 0.0f);
    light->DVector = AVector::UNIT_Y;
    Material* material = new0 Material();
    material->Diffuse = Float4(0.5f, 0.0f, 0.5f, 1.0f);
    LightDirPerVerEffect* effect = new0 LightDirPerVerEffect();

    // Create the children of the DLOD node.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0);

    StandardMesh sm(vformat);

    // Create a sphere mesh (child 0).
    TriMesh* mesh = sm.Sphere(32, 16, 1.0f);
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Create an icosahedron (child 1).
    mesh = sm.Icosahedron();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Create a dodecahedron (child 2).
    mesh = sm.Dodecahedron();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Create an octahedron (child 3).
    mesh = sm.Octahedron();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Create a hexahedron (child 4).
    mesh = sm.Hexahedron();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Create a tetrahedron (child 5).
    mesh = sm.Tetrahedron();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
    mDlod->AttachChild(mesh);

    // Set the distance intervals for switching the active child.
    mDlod->SetModelDistance(0,  0.0f,  10.0f);
    mDlod->SetModelDistance(1, 10.0f,  20.0f);
    mDlod->SetModelDistance(2, 20.0f,  30.0f);
    mDlod->SetModelDistance(3, 30.0f,  40.0f);
    mDlod->SetModelDistance(4, 40.0f,  50.0f);
    mDlod->SetModelDistance(5, 50.0f, 100.0f);

    // Set the model level-of-detail center.
    mDlod->ModelCenter() = APoint::ORIGIN;
}
//----------------------------------------------------------------------------
