// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SwitchNodes.h"

WM5_WINDOW_APPLICATION(SwitchNodes);

//----------------------------------------------------------------------------
SwitchNodes::SwitchNodes ()
    :
    WindowApplication3("SampleGraphics/SwitchNodes", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool SwitchNodes::OnInitialize ()
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
void SwitchNodes::OnTerminate ()
{
    mScene = 0;
    mSwitch = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SwitchNodes::OnIdle ()
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
bool SwitchNodes::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'c':
    case 'C':
    {
        int child = mSwitch->GetActiveChild();
        if (++child == mSwitch->GetNumChildren())
        {
            child = 0;
        }
        mSwitch->SetActiveChild(child);
        mCuller.ComputeVisibleSet(mScene);
        return true;
    }
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SwitchNodes::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mSwitch = new0 SwitchNode();
    mScene->AttachChild(mSwitch);

    // Texture effect to be shared by all objects.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    std::string path = Environment::GetPathR("Flower.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);

    // Create the children of the switch node.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);

    // Create a rectangle mesh (child 0).
    TriMesh* mesh = sm.Rectangle(4, 4, 1.0f, 1.0f);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a disk mesh (child 1).
    mesh = sm.Disk(8,16,1.0f);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a box mesh (child 2).
    mesh = sm.Box(1.0f,0.5f,0.25f);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a closed cylinder mesh (child 3).
    mesh = sm.Cylinder(8,16,1.0f,2.0f,false);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a sphere mesh (child 4).
    mesh = sm.Sphere(32,16,1.0f);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a torus mesh (child 5).
    mesh = sm.Torus(16,16,1.0f,0.25f);
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a tetrahedron (child 6).
    mesh = sm.Tetrahedron();
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a hexahedron (child 7).
    mesh = sm.Hexahedron();
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create an octahedron (child 8).
    mesh = sm.Octahedron();
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create a dodecahedron (child 9).
    mesh = sm.Dodecahedron();
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Create an icosahedron (child 10).
    mesh = sm.Icosahedron();
    mesh->SetEffectInstance(effect->CreateInstance(texture));
    mSwitch->AttachChild(mesh);

    // Set the active child (otherwise it is invalid).
    mSwitch->SetActiveChild(0);
}
//----------------------------------------------------------------------------
