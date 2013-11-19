// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2011/07/27)

#include "CollisionsMovingSpheres.h"

WM5_WINDOW_APPLICATION(CollisionsMovingSpheres);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
CollisionsMovingSpheres::CollisionsMovingSpheres ()
    :
    WindowApplication3("SamplePhysics/CollisionsMovingSpheres", 0, 0, 640,
        480, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
    mColliders(mSphere0, mSphere1),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
#ifdef SINGLE_STEP
    mSimTime = 0.01f;
    mSimDelta = 0.01f;
#else
    mSimTime = 0.001f;
    mSimDelta = 0.001f;
#endif
    mBoundingSphere.Center = Vector3f::ZERO;
    mBoundingSphere.Radius = 1.0f;
}
//----------------------------------------------------------------------------
bool CollisionsMovingSpheres::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(3.0f, 0.0f, 0.0f);
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
void CollisionsMovingSpheres::OnTerminate ()
{
    mScene = 0;
    mMesh0 = 0;
    mMesh1 = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CollisionsMovingSpheres::OnIdle ()
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

#ifndef SINGLE_STEP
    UpdateSpheres();
#endif

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
bool CollisionsMovingSpheres::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

#ifdef SINGLE_STEP
    switch (key)
    {
    case 'g':
    case 'G':
        UpdateSpheres();
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
void CollisionsMovingSpheres::CreateScene ()
{
    mScene = new0 Node();

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    StandardMesh sm(vformat);

    mSphere0.Radius = 0.1f;
    mSphere1.Radius = 0.2f;
    mVelocity0 = Vector3f(0.0f, -1.0f, 0.0f);
    mVelocity1 = Vector3f(0.0f, 0.0f, 1.0f);
    mMesh0 = sm.Sphere(16, 16, mSphere0.Radius);
    mMesh1 = sm.Sphere(16, 16, mSphere1.Radius);

    VertexBufferAccessor vba0(mMesh0), vba1(mMesh1);
    for (int i = 0; i < vba0.GetNumVertices(); ++i)
    {
        vba0.Color<Float3>(0, i) = Float3(Mathf::UnitRandom(), 0.0f, 0.0f);
        vba1.Color<Float3>(0, i) = Float3(0.0f, 0.0f, Mathf::UnitRandom());
    }

    VertexColor3Effect* effect = new0 VertexColor3Effect();
    mMesh0->SetEffectInstance(effect->CreateInstance());
    mMesh1->SetEffectInstance(effect->CreateInstance());

    mSphere0.Center = Vector3f(0.0f, 0.75f, 0.0f);
    mSphere1.Center = Vector3f(0.0f, -0.75f, 0.0f);
    mMesh0->LocalTransform.SetTranslate(mSphere0.Center);
    mMesh1->LocalTransform.SetTranslate(mSphere1.Center);

    mScene->AttachChild(mMesh0);
    mScene->AttachChild(mMesh1);
}
//----------------------------------------------------------------------------
void CollisionsMovingSpheres::UpdateSpheres ()
{
    float contactTime;
    Colliders::CollisionType eType = mColliders.Find(mSimTime, mVelocity0,
        mVelocity1, contactTime);
    WM5_UNUSED(eType);

    if (contactTime < 0.0f)
    {
        contactTime = 0.0f;
    }

    if (contactTime <= mSimTime)
    {
        // Move the spheres through the time to make contact.
        mSphere0.Center += contactTime*mVelocity0;
        mSphere1.Center += contactTime*mVelocity1;
        Vector3f contactPoint = mColliders.GetContactPoint();

        // Compute the unit-length normals at the contact point.
        Vector3f normal0 = contactPoint - mSphere0.Center;
        normal0.Normalize();
        Vector3f normal1 = contactPoint - mSphere1.Center;
        normal1.Normalize();

        // Reflect the velocities through the normals.
        mVelocity0 -= 2.0f*mVelocity0.Dot(normal1)*normal1;
        mVelocity1 -= 2.0f*mVelocity1.Dot(normal0)*normal0;
        mSimTime -= contactTime;
    }
    else
    {
        mSphere0.Center += mSimDelta*mVelocity0;
        mSphere1.Center += mSimDelta*mVelocity1;
        mSimTime = mSimDelta;
    }

    // Keep the spheres inside the world sphere.
    Vector3f diff = mSphere0.Center - mBoundingSphere.Center;
    float length = diff.Normalize();
    if (length >= mBoundingSphere.Radius)
    {
        mSphere0.Center = mBoundingSphere.Radius*diff;
        mVelocity0 -= 2.0f*mVelocity0.Dot(diff)*diff;
    }
    diff = mSphere1.Center - mBoundingSphere.Center;
    length = diff.Normalize();
    if (length >= mBoundingSphere.Radius)
    {
        mSphere1.Center = mBoundingSphere.Radius*diff;
        mVelocity1 -= 2.0f*mVelocity1.Dot(diff)*diff;
    }

    mMesh0->LocalTransform.SetTranslate(mSphere0.Center);
    mMesh1->LocalTransform.SetTranslate(mSphere1.Center);
    mScene->Update();
}
//----------------------------------------------------------------------------
