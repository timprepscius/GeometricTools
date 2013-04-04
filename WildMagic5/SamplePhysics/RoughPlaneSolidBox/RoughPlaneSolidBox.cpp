// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RoughPlaneSolidBox.h"

WM5_WINDOW_APPLICATION(RoughPlaneSolidBox);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneSolidBox::RoughPlaneSolidBox ()
    :
    WindowApplication3("SamplePhysics/RoughPlaneSolidBox", 0, 0, 640, 480,
        Float4(0.839215f, 0.894117f, 0.972549f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mDoUpdate = true;
}
//----------------------------------------------------------------------------
bool RoughPlaneSolidBox::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    APoint camPosition(17.695415f, 0.0f, 6.4494629f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    InitializeModule();
    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::OnTerminate ()
{
    mScene = 0;
    mBox = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

#ifndef SINGLE_STEP
    PhysicsTick();
#endif
    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool RoughPlaneSolidBox::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    case 'r':  // restart
    case 'R':
        InitializeModule();
        MoveBox();
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        PhysicsTick();
        return true;
#endif
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mScene->AttachChild(CreateGround());
    mScene->AttachChild(CreateRamp());
    mScene->AttachChild(CreateBox());

    mScene->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, 0.661917f));
}
//----------------------------------------------------------------------------
Node* RoughPlaneSolidBox::CreateBox ()
{
    mBox = new0 Node();

    float xExtent = (float)mModule.XLocExt;
    float yExtent = (float)mModule.YLocExt;
    float zExtent = (float)mModule.ZLocExt;

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    StandardMesh sm(vformat);
    VertexColor3Effect* effect = new0 VertexColor3Effect();
    VertexBufferAccessor vba;
    Transform transform;
    TriMesh* face;
    int i;

    // +z face
    Float3 red(1.0f, 0.0f, 0.0f);
    transform.SetTranslate(APoint(0.0f, 0.0f, zExtent));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, xExtent, yExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = red;
        vba.Color<Float3>(0, 1) = red;
        vba.Color<Float3>(0, 2) = red;
        vba.Color<Float3>(0, 3) = red;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    // -z face
    Float3 darkRed(0.5f, 0.0f, 0.0f);
    transform.SetTranslate(APoint(0.0f, 0.0f, -zExtent));
    transform.SetRotate(HMatrix(AVector::UNIT_Y, AVector::UNIT_X,
        -AVector::UNIT_Z, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, yExtent, xExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = darkRed;
        vba.Color<Float3>(0, 1) = darkRed;
        vba.Color<Float3>(0, 2) = darkRed;
        vba.Color<Float3>(0, 3) = darkRed;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    // +y face
    Float3 green(0.0f, 1.0f, 0.0f);
    transform.SetTranslate(APoint(0.0f, yExtent, 0.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_Z, AVector::UNIT_X,
        AVector::UNIT_Y, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, zExtent, xExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = green;
        vba.Color<Float3>(0, 1) = green;
        vba.Color<Float3>(0, 2) = green;
        vba.Color<Float3>(0, 3) = green;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    // -y face
    Float3 darkGreen(0.0f, 1.0f, 0.0f);
    transform.SetTranslate(APoint(0.0f, -yExtent, 0.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_X, AVector::UNIT_Z,
        -AVector::UNIT_Y, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, xExtent, zExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = darkGreen;
        vba.Color<Float3>(0, 1) = darkGreen;
        vba.Color<Float3>(0, 2) = darkGreen;
        vba.Color<Float3>(0, 3) = darkGreen;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    // +x face
    Float3 blue(0.0f, 0.0f, 1.0f);
    transform.SetTranslate(APoint(xExtent, 0.0f, 0.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_Y, AVector::UNIT_Z,
        AVector::UNIT_X, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, yExtent, zExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = blue;
        vba.Color<Float3>(0, 1) = blue;
        vba.Color<Float3>(0, 2) = blue;
        vba.Color<Float3>(0, 3) = blue;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    // -x face
    Float3 darkBlue(0.0f, 0.0f, 1.0f);
    transform.SetTranslate(APoint(-xExtent, 0.0f, 0.0f));
    transform.SetRotate(HMatrix(AVector::UNIT_Z, AVector::UNIT_Y,
        -AVector::UNIT_X, APoint::ORIGIN, true));
    sm.SetTransform(transform);
    face = sm.Rectangle(2, 2, zExtent, yExtent);
    vba.ApplyTo(face);
    for (i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, 0) = darkBlue;
        vba.Color<Float3>(0, 1) = darkBlue;
        vba.Color<Float3>(0, 2) = darkBlue;
        vba.Color<Float3>(0, 3) = darkBlue;
    }
    face->SetEffectInstance(effect->CreateInstance());
    mBox->AttachChild(face);

    MoveBox();
    return mBox;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateRamp ()
{
    float x = 8.0f;
    float y = 8.0f;
    float z = y*Mathf::Tan((float)mModule.Angle);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(6, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Float3>(0) = Float3(-x, 0.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(+x, 0.0f, 0.0f);
    vba.Position<Float3>(2) = Float3(-x, y, 0.0f);
    vba.Position<Float3>(3) = Float3(+x, y, 0.0f);
    vba.Position<Float3>(4) = Float3(-x, y, z);
    vba.Position<Float3>(5) = Float3(+x, y, z);
    vba.TCoord<Float2>(0, 0) = Float2(0.25f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(0.75f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 4) = Float2(0.25f, 1.0f);
    vba.TCoord<Float2>(0, 5) = Float2(0.75f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(18, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 1;  indices[ 2] = 4;
    indices[ 3] = 1;  indices[ 4] = 5;  indices[ 5] = 4;
    indices[ 6] = 0;  indices[ 7] = 4;  indices[ 8] = 2;
    indices[ 9] = 1;  indices[10] = 3;  indices[11] = 5;
    indices[12] = 3;  indices[13] = 2;  indices[14] = 4;
    indices[15] = 3;  indices[16] = 4;  indices[17] = 5;

    TriMesh* ramp = new0 TriMesh(vformat, vbuffer, ibuffer);

    std::string path = Environment::GetPathR("Metal.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    ramp->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return ramp;
}
//----------------------------------------------------------------------------
TriMesh* RoughPlaneSolidBox::CreateGround ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* ground = StandardMesh(vformat).Rectangle(2, 2, 32.0f, 32.0f);

    // Change the texture repeat.
    VertexBufferAccessor vba(ground);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 8.0f;
        tcoord[1] *= 8.0f;
    }

    std::string path = Environment::GetPathR("Gravel.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    ground->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return ground;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::InitializeModule ()
{
    // Set up the physics module.
    mModule.Mu = 0.01;
    mModule.Gravity = 10.0;
    mModule.Angle = 0.125*Mathd::PI;
    mModule.XLocExt = 0.8;
    mModule.YLocExt = 0.4;
    mModule.ZLocExt = 0.2;

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 0.0005;
    double x = -6.0;
    double w = 1.0;
    double xDot = 4.0;
    double wDot = 6.0;
    double theta = 0.25*Mathd::PI;
    double thetaDot = 4.0;
    mModule.Initialize(time, deltaTime, x, w, theta, xDot, wDot, thetaDot);

    mDoUpdate = true;
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::MoveBox ()
{
    float x = (float)mModule.GetX();
    float w = (float)mModule.GetW();
    float xExt = (float)mModule.XLocExt;
    float yExt = (float)mModule.YLocExt;
    float zExt = (float)mModule.ZLocExt;
    float sinPhi = (float)mModule.SinAngle;
    float cosPhi = (float)mModule.CosAngle;
    float theta = (float)mModule.GetTheta();
    float sinTheta = Mathf::Sin(theta);
    float cosTheta = Mathf::Cos(theta);

    // Compute the box center.
    APoint center(x, w*cosPhi - zExt*sinPhi, w*sinPhi + zExt*cosPhi);

    // Compute the box orientation.
    AVector axis0(cosTheta, -sinTheta*cosPhi, -sinTheta*sinPhi);
    AVector axis1(sinTheta, +cosTheta*cosPhi, +cosTheta*sinPhi);
    AVector axis2(0.0f, -sinPhi, cosPhi);

    // Keep the box from sliding below the ground.
    float zRadius =
        xExt*Mathf::FAbs(axis0.Z()) +
        yExt*Mathf::FAbs(axis1.Z()) +
        zExt*Mathf::FAbs(axis2.Z());

    if (center.Z() >= zRadius)
    {
        // Update the box.
        mBox->LocalTransform.SetTranslate(center);
        mBox->LocalTransform.SetRotate(HMatrix(axis0, axis1, axis2,
            APoint::ORIGIN, true));
        mBox->Update();
    }
    else
    {
        mDoUpdate = false;
    }
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::PhysicsTick ()
{
    if (mDoUpdate)
    {
        mModule.Update();
        MoveBox();
    }
}
//----------------------------------------------------------------------------
void RoughPlaneSolidBox::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
