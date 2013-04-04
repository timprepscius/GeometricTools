// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PointSystems.h"
#include "RandomController.h"

WM5_WINDOW_APPLICATION(PointSystems);

//----------------------------------------------------------------------------
PointSystems::PointSystems ()
    :
    WindowApplication3("SampleGraphics/PointSystems", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool PointSystems::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(4.0f, 0.0f, 0.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
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
void PointSystems::OnTerminate ()
{
    mScene = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PointSystems::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();
    mScene->Update(GetTimeInSeconds());
    mCuller.ComputeVisibleSet(mScene);

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
void PointSystems::CreateScene ()
{
    mScene = new0 Node();

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(1024, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Position<Float3>(i) = Float3(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(), Mathf::SymmetricRandom());
        vba.Color<Float3>(0, i) = Float3(Mathf::UnitRandom(),
            Mathf::UnitRandom(), Mathf::UnitRandom());
    }

    Polypoint* points = new0 Polypoint(vformat, vbuffer);
    points->AttachController(new0 RandomController());
    points->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    mScene->AttachChild(points);
}
//----------------------------------------------------------------------------
