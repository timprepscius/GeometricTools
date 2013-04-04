// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.7.0 (2011/08/29)

#include "IntersectInfiniteCylinders.h"

WM5_WINDOW_APPLICATION(IntersectInfiniteCylinders);

//----------------------------------------------------------------------------
IntersectInfiniteCylinders::IntersectInfiniteCylinders ()
    :
    WindowApplication3("SampleMathematics/IntersectInfiniteCylinders", 0, 0,
        640, 480, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f),
    mRadius0(3.0f),
    mRadius1(2.0f),
    mHeight(100.0f),
    mC0(4.0f),
    mW1(3.0f/5.0f),
    mW2(4.0f/5.0f)
{
    mAngle = Mathf::ATan2(mW1, mW2);
}
//----------------------------------------------------------------------------
bool IntersectInfiniteCylinders::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, -16.0f, 0.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();
    mScene->Update();

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectInfiniteCylinders::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mCylinder0 = 0;
    mCylinder1 = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectInfiniteCylinders::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mVisible);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool IntersectInfiniteCylinders::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
    {
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void IntersectInfiniteCylinders::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    StandardMesh sm(vformat);
    VertexBufferAccessor vba;
    int i;

    // Create the canonical cylinder.
    mCylinder0 = sm.Cylinder(32, 128, mRadius0, mHeight, true);
    mScene->AttachChild(mCylinder0);
    mVisible.Insert(mCylinder0);
    vba.ApplyTo(mCylinder0);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = Float3(0.5f, 0.0f, 0.0f);
    }
    mCylinder0->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    // Create the other cylinder.
    mCylinder1 = sm.Cylinder(32, 128, mRadius1, mHeight, true);
    mScene->AttachChild(mCylinder1);
    mVisible.Insert(mCylinder1);
    vba.ApplyTo(mCylinder1);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = Float3(0.0f, 0.0f, 0.5f);
    }
    mCylinder1->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mCylinder1->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X, -mAngle));
    mCylinder1->LocalTransform.SetTranslate(APoint(mC0, 0.0f, 0.0f));

    // Create the intersection curve.
    const float minTheta = 2.0f*Mathf::PI/3.0f;
    const float maxTheta = 4.0f*Mathf::PI/3.0f;
    float theta, cs, sn, t, tmp, discr;
    VertexBuffer* vbuffer = new0 VertexBuffer(1024, vstride);
    mCurve0 = new0 Polysegment(vformat, vbuffer, true);
    mScene->AttachChild(mCurve0);
    vba.ApplyTo(mCurve0);
    int numPoints = vba.GetNumVertices();
    float multiplier = (maxTheta - minTheta)/(float)(numPoints - 1);
    for (i = 0; i < numPoints; ++i)
    {
        theta = minTheta + multiplier*i;
        cs = Mathf::Cos(theta);
        sn = Mathf::Sin(theta);
        tmp = mC0 + mRadius1*cs;
        discr = Mathf::FAbs(mRadius0*mRadius0 - tmp*tmp);
        t = (-mRadius1*mW2*sn - Mathf::Sqrt(discr))/mW1;

        Float3& position = vba.Position<Float3>(i);
        position[0] = mC0 + mRadius1*cs;
        position[1] = +mRadius1*sn*mW2 + t*mW1;
        position[2] = -mRadius1*sn*mW1 + t*mW2;
        vba.Color<Float3>(0, i) = Float3(0.0f, 0.5f, 0.0f);
    }
    mCurve0->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mVisible.Insert(mCurve0);

    vbuffer = new0 VertexBuffer(1024, vstride);
    mCurve1 = new0 Polysegment(vformat, vbuffer, true);
    mScene->AttachChild(mCurve1);
    vba.ApplyTo(mCurve1);
    numPoints = vba.GetNumVertices();
    multiplier = (maxTheta - minTheta)/(float)(numPoints - 1);
    for (i = 0; i < numPoints; ++i)
    {
        theta = minTheta + multiplier*i;
        cs = Mathf::Cos(theta);
        sn = Mathf::Sin(theta);
        tmp = mC0 + mRadius1*cs;
        discr = Mathf::FAbs(mRadius0*mRadius0 - tmp*tmp);
        t = (-mRadius1*mW2*sn + Mathf::Sqrt(discr))/mW1;

        Float3& position = vba.Position<Float3>(i);
        position[0] = mC0 + mRadius1*cs;
        position[1] = +mRadius1*sn*mW2 + t*mW1;
        position[2] = -mRadius1*sn*mW1 + t*mW2;
        vba.Color<Float3>(0, i) = Float3(0.0f, 0.5f, 0.0f);
    }
    mCurve1->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mVisible.Insert(mCurve1);
}
//----------------------------------------------------------------------------
