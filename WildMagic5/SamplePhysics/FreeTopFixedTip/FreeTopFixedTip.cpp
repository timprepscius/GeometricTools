// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "FreeTopFixedTip.h"

WM5_WINDOW_APPLICATION(FreeTopFixedTip);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FreeTopFixedTip::FreeTopFixedTip ()
    :
    WindowApplication3("SamplePhysics/FreeTopFixedTip", 0, 0, 640, 480,
        Float4(0.839215f, 0.894117f, 0.972549f, 1.0f))
{
    mLastIdle = 0.0f;
}
//----------------------------------------------------------------------------
bool FreeTopFixedTip::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    APoint camPosition(4.0f, 0.0f, 2.0f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    InitializeModule();
    CreateScene();
    mTopRoot->LocalTransform.SetRotate(mModule.GetBodyAxes());

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mTopRoot = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

    float currIdle = (float)GetTimeInSeconds();
    float diff = currIdle - mLastIdle;
    if (diff >= 1.0f/30.0f)
    {
        mLastIdle = currIdle;
        PhysicsTick();
        GraphicsTick();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool FreeTopFixedTip::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::InitializeModule ()
{
    mModule.Gravity = 10.0;
    mModule.Mass = 1.0;
    mModule.Length = 8.0;
    mModule.Inertia1 = 1.0;
    mModule.Inertia3 = 2.0;

    double time = 0.0;
    double deltaTime = 0.01;
    double theta = 0.0;
    double phi = 0.001;
    double psi = 0.0;
    double angVel1 = 1.0;
    double angVel2 = 0.0;
    double angVel3 = 10.0;
    mModule.Initialize(time, deltaTime, theta, phi, psi, angVel1, angVel2,
        angVel3);

    mMaxPhi = Mathf::HALF_PI - Mathf::ATan(2.0f/3.0f);
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::CreateScene ()
{
    // scene -+--- floor
    //        |
    //        +--- vertical axis
    //        |
    //        +--- top root ---+--- top
    //                         |
    //                         +--- top axis

    mScene = new0 Node();
    mTopRoot = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mScene->AttachChild(CreateFloor());
    mScene->AttachChild(CreateAxisVertical());
    mScene->AttachChild(mTopRoot);
    mTopRoot->AttachChild(CreateTop());
    mTopRoot->AttachChild(CreateAxisTop());
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateFloor ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* floor = StandardMesh(vformat).Rectangle(2, 2, 32.0f, 32.0f);
    std::string path = Environment::GetPathR("Wood.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    floor->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return floor;
}
//----------------------------------------------------------------------------
TriMesh* FreeTopFixedTip::CreateTop ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* top = StandardMesh(vformat).Cylinder(32, 32, 1.0f, 2.0f, true);

    // Adjust the shape.
    VertexBufferAccessor vba(top);
    int numVertices = vba.GetNumVertices();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        Float3& pos = vba.Position<Float3>(i);
        float z = pos[2] + 1.0f;
        float r = 0.75f*(z >= 1.5f ? 4.0f - 2.0f*z : z/1.5f);
        float invLength = Mathf::InvSqrt(pos[0]*pos[0] + pos[1]*pos[1]);
        pos[0] *= r*invLength;
        pos[1] *= r*invLength;
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 4.0f;
        tcoord[1] *= 4.0f;
    }
    top->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    top->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 1.0f));

    std::string path = Environment::GetPathR("TopTexture.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    top->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    return top;
}
//----------------------------------------------------------------------------
Polysegment* FreeTopFixedTip::CreateAxisTop ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(2, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(0.0f, 0.0f, 4.0f);
    vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 1.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 1.0f, 1.0f);

    Polysegment* axisTop = new0 Polysegment(vformat, vbuffer, true);
    axisTop->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return axisTop;
}
//----------------------------------------------------------------------------
Polysegment* FreeTopFixedTip::CreateAxisVertical ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(2, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(0.0f, 0.0f, 4.0f);
    vba.Color<Float3>(0,0) = Float3(0.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0,1) = Float3(0.0f, 0.0f, 0.0f);

    Polysegment* axisVertical = new0 Polysegment(vformat, vbuffer, true);
    axisVertical->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return axisVertical;
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::PhysicsTick ()
{
    // Stop the simulation when the top edge reaches the ground.
    if (mModule.GetPhi() >= mMaxPhi)
    {
        // EXERCISE.  Instead of stopping the top, maintain its phi value at
        // mMaxPhi so that the top continues to roll on the ground.  In
        // addition, arrange for the top to slow down while rolling on the
        // ground, eventually coming to a stop.
        return;
    }

    // Move the top
    mModule.Update();
    mTopRoot->LocalTransform.SetRotate(mModule.GetBodyAxes());
    mTopRoot->Update();
}
//----------------------------------------------------------------------------
void FreeTopFixedTip::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
