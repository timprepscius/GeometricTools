// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SortFaces.h"
#include "SortedCube.h"

WM5_WINDOW_APPLICATION(SortFaces);

//----------------------------------------------------------------------------
SortFaces::SortFaces ()
    :
    WindowApplication3("SampleGraphics/SortFaces", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool SortFaces::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    APoint camPosition(0.0f, 0.0f, 8.0f);
    AVector camDVector(0.0f,0.0f,-1.0f);
    AVector camUVector(0.0f,1.0f,0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Create the scene.
    mScene = new0 SortedCube(mCamera, "SortFaceXP.wmtf", "SortFaceXM.wmtf",
        "SortFaceYP.wmtf", "SortFaceYM.wmtf", "SortFaceZP.wmtf",
        "SortFaceZM.wmtf");

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
void SortFaces::OnTerminate ()
{
    mScene = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SortFaces::OnIdle ()
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
