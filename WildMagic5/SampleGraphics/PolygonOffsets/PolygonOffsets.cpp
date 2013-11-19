// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PolygonOffsets.h"

WM5_WINDOW_APPLICATION(PolygonOffsets);

//----------------------------------------------------------------------------
PolygonOffsets::PolygonOffsets ()
    :
    WindowApplication3("SampleGraphics/PolygonOffsets", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool PolygonOffsets::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, 0.0f, -16.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
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
void PolygonOffsets::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PolygonOffsets::OnIdle ()
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
        mRenderer->Draw(8, 16, mTextColor,
            "Press UP/DOWN arrows. "
            "The left box flickers, the right does not");
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool PolygonOffsets::OnKeyDown (unsigned char key, int x, int y)
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
void PolygonOffsets::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    // Vertices to be shared by rectangles 1 and 3.
    VertexBuffer* vbuffer0 = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer0);
    vba.Position<Float3>(0) = Float3(-1.0f, 0.0f, -1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, 0.0f, +1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, 0.0f, +1.0f);
    vba.Position<Float3>(3) = Float3(+1.0f, 0.0f, -1.0f);
    vba.Color<Float3>(0, 0) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 2) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 3) = Float3(1.0f, 0.0f, 0.0f);

    // Vertices to be shared by rectangles 2 and 4.
    VertexBuffer* vbuffer1 = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer1);
    vba.Position<Float3>(0) = Float3(-1.0f, 0.0f, -1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, 0.0f, +1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, 0.0f, +1.0f);
    vba.Position<Float3>(3) = Float3(+1.0f, 0.0f, -1.0f);
    vba.Color<Float3>(0, 0) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0, 1) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0, 2) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0, 3) = Float3(0.0f, 1.0f, 0.0f);

    // Indices to be shared by all rectangles.
    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 3;
    indices[3] = 3;  indices[4] = 1;  indices[5] = 2;

    // Effect to be shared by the first three rectangles.
    VertexColor3Effect* effect = new0 VertexColor3Effect();

    // rectangle 1
    TriMesh* mesh = new0 TriMesh(vformat, vbuffer0, ibuffer);
    mesh->SetEffectInstance(effect->CreateInstance());
    mesh->LocalTransform.SetTranslate(APoint(+2.0f, -4.0f, 0.0f));
    mScene->AttachChild(mesh);

    // rectangle 2
    mesh = new0 TriMesh(vformat, vbuffer1, ibuffer);
    mesh->SetEffectInstance(effect->CreateInstance());
    mesh->LocalTransform.SetTranslate(APoint(+2.0f, -4.0f, 0.0f));
    mesh->LocalTransform.SetUniformScale(0.5f);
    mScene->AttachChild(mesh);

    // rectangle 3
    mesh = new0 TriMesh(vformat, vbuffer0, ibuffer);
    mesh->SetEffectInstance(effect->CreateInstance());
    mesh->LocalTransform.SetTranslate(APoint(-2.0f, -4.0f, 0.0f));
    mScene->AttachChild(mesh);

    // rectangle 4
    mesh = new0 TriMesh(vformat, vbuffer1, ibuffer);
    mesh->LocalTransform.SetTranslate(APoint(-2.0f, -4.0f, 0.0f));
    mesh->LocalTransform.SetUniformScale(0.5f);
    mScene->AttachChild(mesh);

    // Set up the polygon offset for rectangle 4.
    effect = new0 VertexColor3Effect();
    OffsetState* ostate = effect->GetOffsetState(0, 0);
    ostate->FillEnabled = true;
    ostate->Scale = -1.0f;
    ostate->Bias = -2.0f;
    mesh->SetEffectInstance(effect->CreateInstance());
}
//----------------------------------------------------------------------------
