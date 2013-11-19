// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2011/03/23)

#include "IntersectTriangleCylinder.h"

WM5_WINDOW_APPLICATION(IntersectTriangleCylinder);

//----------------------------------------------------------------------------
IntersectTriangleCylinder::IntersectTriangleCylinder ()
    :
    WindowApplication3("SampleMathematics/IntersectTriangleCylinder", 0, 0,
        640, 480, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f),
    mTriangleMVertex0(0.0f, 0.0f, 3.0f),
    mTriangleMVertex1(-0.25f, 0.0f, 0.0f),
    mTriangleMVertex2(1.0f, 0.0f, -1.0f),
    mCylinderMCenter(0.0f, 0.0f, 0.0f),
    mCylinderMDirection(0.0f, 0.0f, 1.0f),
    mCylinderRadius(1.0f),
    mCylinderHeight(0.5f)
{
}
//----------------------------------------------------------------------------
bool IntersectTriangleCylinder::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(6.0f, 0.0f, 0.0f);
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

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectTriangleCylinder::OnTerminate ()
{
    mScene = 0;
    mTMesh = 0;
    mCMesh = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectTriangleCylinder::OnIdle ()
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
        TestIntersection();
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        //DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool IntersectTriangleCylinder::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        mMotionObject = mScene;
        return true;
    case '1':
        mMotionObject = mTMesh;
        return true;
    case '2':
        mMotionObject = mCMesh;
        return true;
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void IntersectTriangleCylinder::CreateScene ()
{
    mScene = new0 Node();
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(3, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = (const Vector3f&)mTriangleMVertex0;
    vba.Color<Float3>(0, 0) = Float3(0.0f, 0.0f, 1.0f);
    vba.Position<Vector3f>(1) = (const Vector3f&)mTriangleMVertex1;
    vba.Color<Float3>(0, 1) = Float3(0.0f, 0.0f, 1.0f);
    vba.Position<Vector3f>(2) = (const Vector3f&)mTriangleMVertex2;
    vba.Color<Float3>(0, 2) = Float3(0.0f, 0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(3, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    mTMesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mTMesh->SetEffectInstance(
        VertexColor3Effect::CreateUniqueInstance());
    mTMesh->LocalTransform.SetTranslate(APoint(0.0f, 1.125f, 0.0f));

    mCMesh = StandardMesh(vformat).Cylinder(8, 16, mCylinderRadius,
        mCylinderHeight, false);
    vba.ApplyTo(mCMesh);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = Float3(1.0f, 0.0f, 0.0f);
    }
    mCMesh->SetEffectInstance(
        VertexColor3Effect::CreateUniqueInstance());

    mScene->AttachChild(mTMesh);
    mScene->AttachChild(mCMesh);
}
//----------------------------------------------------------------------------
void IntersectTriangleCylinder::TestIntersection ()
{
    Triangle3f triangle;
    triangle.V[0] =
        (const Vector3f&)(mTMesh->WorldTransform*mTriangleMVertex0);
    triangle.V[1] =
        (const Vector3f&)(mTMesh->WorldTransform*mTriangleMVertex1);
    triangle.V[2] =
        (const Vector3f&)(mTMesh->WorldTransform*mTriangleMVertex2);

    Cylinder3f cylinder;
    cylinder.Axis.Origin =
        (const Vector3f&)(mCMesh->WorldTransform*APoint::ORIGIN);
    cylinder.Axis.Direction =
        (const Vector3f&)(mCMesh->WorldTransform*AVector::UNIT_Z);
    cylinder.Radius = mCylinderRadius;
    cylinder.Height = mCylinderHeight;

    VertexBufferAccessor vba(mTMesh);
    if (IntrTriangle3Cylinder3f(triangle, cylinder).Test())
    {
        vba.Color<Float3>(0, 0) = Float3(0.0f, 1.0f, 0.0f);
        vba.Color<Float3>(0, 1) = Float3(0.0f, 1.0f, 0.0f);
        vba.Color<Float3>(0, 2) = Float3(0.0f, 1.0f, 0.0f);
    }
    else
    {
        vba.Color<Float3>(0, 0) = Float3(0.0f, 0.0f, 1.0f);
        vba.Color<Float3>(0, 1) = Float3(0.0f, 0.0f, 1.0f);
        vba.Color<Float3>(0, 2) = Float3(0.0f, 0.0f, 1.0f);
    }
    mRenderer->Update(mTMesh->GetVertexBuffer());
}
//----------------------------------------------------------------------------
