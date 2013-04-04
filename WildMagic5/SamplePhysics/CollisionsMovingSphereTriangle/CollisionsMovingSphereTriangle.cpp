// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#include "CollisionsMovingSphereTriangle.h"

WM5_WINDOW_APPLICATION(CollisionsMovingSphereTriangle);

//----------------------------------------------------------------------------
CollisionsMovingSphereTriangle::CollisionsMovingSphereTriangle ()
    :
    WindowApplication3("SamplePhysics/CollisionsMovingSphereTriangle", 0, 0,
        640, 480, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mSimTime = 0.001f;
    mSimDelta = 0.001f;

    mMTri[0] = Vector3f(0.0f, 0.0f, 0.0f);
    mMTri[1] = Vector3f(0.0f, 1.0f, 0.0f);
    mMTri[2] = Vector3f(-0.6f, 0.7f, 0.8f);
    Vector3f average = (mMTri[0] + mMTri[1] + mMTri[2])/3.0f;
    mMTri[0] -= average;
    mMTri[1] -= average;
    mMTri[2] -= average;
    mTriangle.P[0] = mMTri[0];
    mTriangle.P[1] = mMTri[1];
    mTriangle.P[2] = mMTri[2];
    mTriangle.ComputeDerived();
    mTriangleVelocity = Vector3f(0.0f, 0.0f, 0.0f);

    mSphere.C = Vector3f(0.0f, 0.0f, 2.0f);
    mSphere.R = 0.3f;
    mSphere.ComputeDerived();
    mSphereVelocity = Vector3f(0.0f, 0.0f, -1.0f);

    mUseInitialCenter = true;
}
//----------------------------------------------------------------------------
bool CollisionsMovingSphereTriangle::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(8.0f, 0.0f, 0.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of scene.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void CollisionsMovingSphereTriangle::OnTerminate ()
{
    mScene = 0;
    mSphereMesh = 0;
    mTriangleMesh = 0;
    mCenters = 0;
    mContactMesh = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CollisionsMovingSphereTriangle::OnIdle ()
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

    Update();

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
bool CollisionsMovingSphereTriangle::OnKeyDown (unsigned char key, int x,
    int y)
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
    case '0':
        mMotionObject = mScene;
        return true;
    case '1':
        mMotionObject = mSphereMesh;
        return true;
    case '2':
        mMotionObject = mTriangleMesh;
        return true;
    case ' ':
        // Toggle sphere mesh (not the actual sphere) between initial center
        // and center when in contact with triangle.
        mUseInitialCenter = !mUseInitialCenter;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void CollisionsMovingSphereTriangle::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    StandardMesh sm(vformat);
    mSphereMesh = sm.Sphere(16, 16, mSphere.R);

    VertexBufferAccessor vba(mSphereMesh);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = Float3(Mathf::UnitRandom(), 0.0f, 0.0f);
    }
    VertexColor3Effect* effect = new0 VertexColor3Effect();
    mSphereMesh->SetEffectInstance(effect->CreateInstance());
    mSphereMesh->LocalTransform.SetTranslate(mSphere.C);

    // Orient the sphere so that the first column of the local rotation
    // matrix is the sphere velocity direction.
    mSphereMesh->LocalTransform.SetRotate(Matrix3f(-AVector::UNIT_Z,
        AVector::UNIT_Y, AVector::UNIT_X, true));

    mScene->AttachChild(mSphereMesh);

    VertexBuffer* vbuffer = new0 VertexBuffer(3, vstride);
    vba.ApplyTo(vformat, vbuffer);
    for (i = 0; i < 3; ++i)
    {
        vba.Position<Vector3f>(i) = mTriangle.P[i];
        vba.Color<Float3>(0, i)  = Float3(0.0f, 0.0f, Mathf::UnitRandom());
    }
    IndexBuffer* ibuffer = new0 IndexBuffer(3, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;

    mTriangleMesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    VertexColor3Effect* noCullEffect = new0 VertexColor3Effect();
    noCullEffect->GetCullState(0, 0)->Enabled = false;
    mTriangleMesh->SetEffectInstance(noCullEffect->CreateInstance());
    mScene->AttachChild(mTriangleMesh);

    vbuffer = new0 VertexBuffer(2, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Vector3f>(0) = mSphere.C;
    vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 0.0f);
    vba.Position<Vector3f>(1) = mSphere.C + 100.0f*mSphereVelocity;
    vba.Color<Float3>(0, 1) = Float3(1.0f, 1.0f, 0.0f);
    mCenters = new0 Polysegment(vformat, vbuffer, false);
    mCenters->SetEffectInstance(effect->CreateInstance());
    mScene->AttachChild(mCenters);

    mContactMesh = sm.Sphere(16, 16, 0.05f);
    vba.ApplyTo(mContactMesh);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = Float3(0.0f, 1.0f, 0.0f);
    }
    mContactMesh->SetEffectInstance(effect->CreateInstance());
    mScene->AttachChild(mContactMesh);

    Update();
}
//----------------------------------------------------------------------------
void CollisionsMovingSphereTriangle::Update ()
{
    // Update the sphere and triangle based on how they were moved by the
    // user (during an OnMotion operation).
    Matrix3f rot = mTriangleMesh->LocalTransform.GetRotate();
    mTriangle.P[0] = rot*mMTri[0];
    mTriangle.P[1] = rot*mMTri[1];
    mTriangle.P[2] = rot*mMTri[2];
    mTriangle.ComputeDerived();

    rot = mSphereMesh->LocalTransform.GetRotate();
    mSphereVelocity = rot.GetColumn(0);

    VertexBufferAccessor vba(mCenters);
    vba.Position<Vector3f>(1) = mSphere.C + 100.0f*mSphereVelocity;
    mRenderer->Update(mCenters->GetVertexBuffer());

    ContactType type = Collide(mSphere, mSphereVelocity, mTriangle,
        mTriangleVelocity, 100.0f, mContactTime, mContactPoint);
    if (type != CONTACT)
    {
        // "Hide" the contact by moving it far away.
        mContactPoint = Vector3f(1000.0f, 1000.0f, 1000.0f);
    }

    mContactMesh->LocalTransform.SetTranslate(mContactPoint);

    if (mUseInitialCenter)
    {
        mSphereMesh->LocalTransform.SetTranslate(mSphere.C);
    }
    else
    {
        mSphereMesh->LocalTransform.SetTranslate(mSphere.C +
            mContactTime*mSphereVelocity);
    }
    mSphereMesh->Update();
}
//----------------------------------------------------------------------------
