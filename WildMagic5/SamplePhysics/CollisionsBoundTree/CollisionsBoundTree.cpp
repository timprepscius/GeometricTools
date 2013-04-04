// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "CollisionsBoundTree.h"

WM5_WINDOW_APPLICATION(CollisionsBoundTree);

//----------------------------------------------------------------------------
CollisionsBoundTree::CollisionsBoundTree ()
    :
    WindowApplication3("SamplePhysics/CollisionsBoundTree", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mBlueUV(0.25f, 0.25f),
        mRedUV(0.25f, 0.75f),
        mCyanUV(0.75f, 0.25f),
        mYellowUV(0.75f, 0.75f),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mGroup = 0;
}
//----------------------------------------------------------------------------
bool CollisionsBoundTree::OnInitialize ()
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
void CollisionsBoundTree::OnTerminate ()
{
    delete0(mGroup);

    mScene = 0;
    mCylinder0 = 0;
    mCylinder1 = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CollisionsBoundTree::OnIdle ()
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
bool CollisionsBoundTree::OnKeyDown (unsigned char key, int x, int y)
{
    // Move the tall/thin cylinder and activate the collision system.
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

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void CollisionsBoundTree::CreateScene ()
{
    // The root of the scene will have two cylinders as children.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    // Create a texture image to be used by both cylinders.
    Texture2D* texture = new0 Texture2D(Texture::TF_A8R8G8B8, 2, 2, 1);
    unsigned int* data = (unsigned int*)texture->GetData(0);
    data[0] = Color::MakeR8G8B8(0,     0, 255);  // blue
    data[1] = Color::MakeR8G8B8(0,   255, 255);  // cyan
    data[2] = Color::MakeR8G8B8(255,   0,   0);  // red
    data[3] = Color::MakeR8G8B8(255, 255,   0);  // yellow

    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);

    // Create two cylinders, one short and thick, one tall and thin.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);
    VertexBufferAccessor vba;
    int i;

    mCylinder0 = sm.Cylinder(8, 16, 1.0f, 2.0f, false);
    vba.ApplyTo(mCylinder0);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float2>(0, i) = mBlueUV;
    }
    mCylinder0->SetEffectInstance(effect->CreateInstance(texture));
    mScene->AttachChild(mCylinder0);

    mCylinder1 = sm.Cylinder(16,8,0.25,4.0,false);
    vba.ApplyTo(mCylinder1);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float2>(0, i) = mRedUV;
    }
    mCylinder1->SetEffectInstance(effect->CreateInstance(texture));
    mScene->AttachChild(mCylinder1);

    mScene->Update();

    // Set up the collision system.  Record0 handles the collision response.
    // Record1 is not given a callback so that 'double processing' of the
    // events does not occur.
    CTree* tree0 = new0 CTree(mCylinder0, 1, false);
    CRecord* record0 = new0 CRecord(tree0, 0, Response, this);
    CTree* tree1 = new0 CTree(mCylinder1, 1, false);
    CRecord* record1 = new0 CRecord(tree1, 0, 0, 0);
    mGroup = new0 CGroup();
    mGroup->Add(record0);
    mGroup->Add(record1);

    ResetColors();
    mGroup->TestIntersection();
}
//----------------------------------------------------------------------------
bool CollisionsBoundTree::Transform (unsigned char key)
{
    // Move the tall/thin cylinder.  After each motion, reset the texture
    // coordinates to the "no intersection" state, then let the collision
    // system test for intersection.  Any intersecting triangles have their
    // texture coordinates changed to the "intersection" state.

    float trnSpeed = 0.1f;
    float rotSpeed = 0.1f;

    HMatrix rot, incr;
    APoint trn;

    switch (key)
    {
        case 'x':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[0] -= trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'X':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[0] += trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'y':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[1] -= trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'Y':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[1] += trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'z':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[2] -= trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'Z':
            trn = mCylinder1->LocalTransform.GetTranslate();
            trn[2] += trnSpeed;
            mCylinder1->LocalTransform.SetTranslate(trn);
            break;
        case 'r':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_X, rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        case 'R':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_X, -rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        case 'a':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Y, rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        case 'A':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Y, -rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        case 'p':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        case 'P':
            rot = mCylinder1->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, -rotSpeed);
            mCylinder1->LocalTransform.SetRotate(incr*rot);
            break;
        default:
            return false;
    }

    // Activate the collision system.
    mCylinder1->Update();
    ResetColors();
    mGroup->TestIntersection();
    return true;
}
//----------------------------------------------------------------------------
void CollisionsBoundTree::ResetColors ()
{
    VertexBufferAccessor vba(mCylinder0);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float2>(0, i) = mBlueUV;
    }
    mRenderer->Update(mCylinder0->GetVertexBuffer());

    vba.ApplyTo(mCylinder1);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float2>(0, i) = mRedUV;
    }
    mRenderer->Update(mCylinder1->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void CollisionsBoundTree::Response (CRecord& record0, int t0,
    CRecord& record1, int t1, Intersector<float,Vector3f>*)
{
    CollisionsBoundTree* app = (CollisionsBoundTree*)TheApplication;

    // Mesh0 triangles that are intersecting change from blue to cyan.
    TriMesh* mesh = record0.GetMesh();
    VertexBufferAccessor vba(mesh);
    const int* indices = (int*)mesh->GetIndexBuffer()->GetData();
    int i0 = indices[3*t0];
    int i1 = indices[3*t0 + 1];
    int i2 = indices[3*t0 + 2];
    vba.TCoord<Float2>(0, i0) = app->mCyanUV;
    vba.TCoord<Float2>(0, i1) = app->mCyanUV;
    vba.TCoord<Float2>(0, i2) = app->mCyanUV;
    app->mRenderer->Update(mesh->GetVertexBuffer());

    // Mesh1 triangles that are intersecting change from red to yellow.
    mesh = record1.GetMesh();
    vba.ApplyTo(mesh);
    indices = (int*)mesh->GetIndexBuffer()->GetData();
    i0 = indices[3*t1];
    i1 = indices[3*t1 + 1];
    i2 = indices[3*t1 + 2];
    vba.TCoord<Float2>(0 ,i0) = app->mYellowUV;
    vba.TCoord<Float2>(0, i1) = app->mYellowUV;
    vba.TCoord<Float2>(0, i2) = app->mYellowUV;
    app->mRenderer->Update(mesh->GetVertexBuffer());

    // NOTE: See the comments in Wm5CollisionGroup.h about information that
    // is available from the Intersector<float,Vector3f> object.
}
//----------------------------------------------------------------------------
