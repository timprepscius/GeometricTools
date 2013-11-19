// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "InverseKinematics.h"

WM5_WINDOW_APPLICATION(InverseKinematics);

//----------------------------------------------------------------------------
InverseKinematics::InverseKinematics ()
    :
    WindowApplication3("SampleGraphics/InverseKinematics", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool InverseKinematics::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, -2.0f, 0.5f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();
    UpdateRod();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.1f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void InverseKinematics::OnTerminate ()
{
    mScene = 0;
    mIKSystem = 0;
    mGoal = 0;
    mJoint0 = 0;
    mJoint1 = 0;
    mWireState = 0;
    mRod = 0;
    mVCEffect = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void InverseKinematics::OnIdle ()
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
bool InverseKinematics::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    if (Transform(key))
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
bool InverseKinematics::OnMotion (int button, int x, int y,
    unsigned int modifiers)
{
    bool moved = WindowApplication3::OnMotion(button, x, y, modifiers);
    if (moved)
    {
        UpdateRod();
    }
    return moved;
}
//----------------------------------------------------------------------------
void InverseKinematics::CreateScene ()
{
    // Scene
    //     GroundMesh
    //     IKSystem
    //         Goal
    //             GoalCube
    //         Joint0
    //             OriginCube
    //             Rod
    //             Joint1
    //                 EndCube

    // Create the scene root.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // The effect shared by multiple objects.
    mVCEffect = new0 VertexColor3Effect();

    // Create the IK objects.
    mIKSystem = new0 Node();
    mGoal = new0 Node();
    mJoint0 = new0 Node();
    mJoint1 = new0 Node();
    mRod = CreateRod();
    mGoal->LocalTransform.SetTranslate(APoint(0.0f, 2.0f, 0.0f));
    mJoint1->LocalTransform.SetTranslate(APoint(1.0f, 0.0f, 0.0f));

    // Set the parent-child links.
    mScene->AttachChild(CreateGround());
    mScene->AttachChild(mIKSystem);
    mIKSystem->AttachChild(mGoal);
    mGoal->AttachChild(CreateCube());
    mIKSystem->AttachChild(mJoint0);
    mJoint0->AttachChild(CreateCube());
    mJoint0->AttachChild(mRod);
    mJoint0->AttachChild(mJoint1);
    mJoint1->AttachChild(CreateCube());

    // Create the goal.
    IKGoalPtr* goals = new1<IKGoalPtr>(1);
    goals[0] = new0 IKGoal(mGoal, mJoint1, 1.0f);

    // Create the joints.
    IKJointPtr* joints = new1<IKJointPtr>(2);

    IKGoalPtr* jointGoal0 = new1<IKGoalPtr>(1);
    jointGoal0[0] = goals[0];
    joints[0] = new0 IKJoint(mJoint0, 1, jointGoal0);
    joints[0]->AllowRotation[2] = true;

    IKGoalPtr* jointGoal1 = new1<IKGoalPtr>(1);
    jointGoal1[0] = goals[0];
    joints[1] = new0 IKJoint(mJoint1, 1, jointGoal1);
    joints[1]->AllowTranslation[2] = true;

    // Create the IK controller.
    IKController* controller = new0 IKController(2, joints, 1, goals);
    controller->Iterations = 1;
    mJoint0->AttachController(controller);
}
//----------------------------------------------------------------------------
TriMesh* InverseKinematics::CreateCube ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(8, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float size = 0.1f;
    vba.Position<Float3>(0) = Float3(-size, -size, -size);
    vba.Position<Float3>(1) = Float3(+size, -size, -size);
    vba.Position<Float3>(2) = Float3(+size, +size, -size);
    vba.Position<Float3>(3) = Float3(-size, +size, -size);
    vba.Position<Float3>(4) = Float3(-size, -size, +size);
    vba.Position<Float3>(5) = Float3(+size, -size, +size);
    vba.Position<Float3>(6) = Float3(+size, +size, +size);
    vba.Position<Float3>(7) = Float3(-size, +size, +size);
    vba.Color<Float3>(0,0) = Float3(0.0f, 0.0f, 1.0f);
    vba.Color<Float3>(0,1) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0,2) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0,3) = Float3(0.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0,4) = Float3(0.0f, 0.0f, 1.0f);
    vba.Color<Float3>(0,5) = Float3(1.0f, 0.0f, 1.0f);
    vba.Color<Float3>(0,6) = Float3(1.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0,7) = Float3(1.0f, 1.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(36, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0; indices[ 1] = 2; indices[ 2] = 1;
    indices[ 3] = 0; indices[ 4] = 3; indices[ 5] = 2;
    indices[ 6] = 4; indices[ 7] = 5; indices[ 8] = 6;
    indices[ 9] = 4; indices[10] = 6; indices[11] = 7;
    indices[12] = 1; indices[13] = 6; indices[14] = 5;
    indices[15] = 1; indices[16] = 2; indices[17] = 6;
    indices[18] = 0; indices[19] = 7; indices[20] = 3;
    indices[21] = 0; indices[22] = 4; indices[23] = 7;
    indices[24] = 0; indices[25] = 1; indices[26] = 5;
    indices[27] = 0; indices[28] = 5; indices[29] = 4;
    indices[30] = 3; indices[31] = 6; indices[32] = 2;
    indices[33] = 3; indices[34] = 7; indices[35] = 6;

    TriMesh* cube = new0 TriMesh(vformat, vbuffer, ibuffer);
    cube->SetEffectInstance(mVCEffect->CreateInstance());

    return cube;
}
//----------------------------------------------------------------------------
Polysegment* InverseKinematics::CreateRod ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(2, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 1.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 1.0f, 1.0f);

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(mVCEffect->CreateInstance());

    return segment;
}
//----------------------------------------------------------------------------
TriMesh* InverseKinematics::CreateGround ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float size = 16.0f;
    vba.Position<Float3>(0) = Float3(-size, -size, -0.1f);
    vba.Position<Float3>(1) = Float3(+size, -size, -0.1f);
    vba.Position<Float3>(2) = Float3(+size, +size, -0.1f);
    vba.Position<Float3>(3) = Float3(-size, +size, -0.1f);
    vba.Color<Float3>(0, 0) = Float3(0.5f, 0.5f, 0.70f);
    vba.Color<Float3>(0, 1) = Float3(0.5f, 0.5f, 0.80f);
    vba.Color<Float3>(0, 2) = Float3(0.5f, 0.5f, 0.90f);
    vba.Color<Float3>(0, 3) = Float3(0.5f, 0.5f, 1.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    TriMesh* plane = new0 TriMesh(vformat, vbuffer, ibuffer);
    plane->SetEffectInstance(mVCEffect->CreateInstance());

    return plane;
}
//----------------------------------------------------------------------------
void InverseKinematics::UpdateRod ()
{
    // The vertex[0] never moves.  The rod mesh is in the coordinate system
    // of joint0, so use the local translation of joint1 for the rod mesh's
    // moving end point.
    VertexBufferAccessor vba(mRod);
    vba.Position<Float3>(1) = mJoint1->LocalTransform.GetTranslate();
    mRenderer->Update(mRod->GetVertexBuffer());
    mRod->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    mRod->Update();
}
//----------------------------------------------------------------------------
bool InverseKinematics::Transform (unsigned char key)
{
    Matrix3f kRot, kIncr;
    APoint trn;

    switch (key)
    {
    case 'x':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[0] -= mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    case 'X':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[0] += mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    case 'y':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[1] -= mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    case 'Y':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[1] += mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    case 'z':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[2] -= mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    case 'Z':
        trn = mGoal->LocalTransform.GetTranslate();
        trn[2] += mTrnSpeed;
        mGoal->LocalTransform.SetTranslate(trn);
        break;
    default:
        return false;
    }

    mIKSystem->Update();
    UpdateRod();
    return true;
}
//----------------------------------------------------------------------------
