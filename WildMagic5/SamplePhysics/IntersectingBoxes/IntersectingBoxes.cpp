// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#include "IntersectingBoxes.h"

WM5_WINDOW_APPLICATION(IntersectingBoxes);

// #define SINGLE_STEP

//----------------------------------------------------------------------------
IntersectingBoxes::IntersectingBoxes ()
    :
    WindowApplication3("SamplePhysics/IntersectingBoxes", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f))
{
    mManager = 0;
    mDoSimulation = true;
    mLastIdle = 0.0f;
    mSize = 256.0f;
}
//----------------------------------------------------------------------------
bool IntersectingBoxes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, 0.0f, -mSize);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.5f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectingBoxes::OnTerminate ()
{
    delete0(mManager);

    mScene = 0;
    mWireState = 0;
    mNoIntersectEffect = 0;
    mIntersectEffect = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectingBoxes::OnIdle ()
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
bool IntersectingBoxes::OnKeyDown (unsigned char key, int x, int y)
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
    case 's':  // toggle simulation
    case 'S':
        mDoSimulation = !mDoSimulation;
        return true;
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyBoxes();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void IntersectingBoxes::CreateScene ()
{
    // Create some axis-aligned boxes for intersection testing.
    const int imax = 16;
    int i;
    for (i = 0; i < imax; ++i)
    {
        float xMin = 0.5f*mSize*Mathf::SymmetricRandom();
        float xMax = xMin + Mathf::IntervalRandom(8.0f, 32.0f);
        float yMin = 0.5f*mSize*Mathf::SymmetricRandom();
        float yMax = yMin + Mathf::IntervalRandom(8.0f, 32.0f);
        float zMin = 0.5f*mSize*Mathf::SymmetricRandom();
        float zMax = zMin + Mathf::IntervalRandom(8.0f, 32.0f);
        mBoxes.push_back(
            AxisAlignedBox3f(xMin, xMax, yMin, yMax, zMin, zMax));
    }
    mManager = new0 BoxManagerf(mBoxes);

    // Scene graph for the visual representation of the boxes.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Effects for boxes, blue for nonintersecting and red for intersecting.
    Float4 black(0.0f, 0.0f, 0.0f, 1.0f);
    Float4 white(1.0f, 1.0f, 1.0f, 1.0f);
    Material* blueMaterial = new0 Material();
    blueMaterial->Emissive = black;
    blueMaterial->Ambient = Float4(0.25f, 0.25f, 0.25f, 1.0f);
    blueMaterial->Diffuse = Float4(0.0f, 0.0f, 1.0f, 1.0f);
    blueMaterial->Specular = black;

    Material* redMaterial = new0 Material();
    redMaterial->Emissive = black;
    redMaterial->Ambient = Float4(0.25f, 0.25f, 0.25f, 1.0f);
    redMaterial->Diffuse = Float4(1.0f, 0.0f, 0.0f, 1.0f);
    redMaterial->Specular = black;

    // A light for the effects.
    Light* light = new0 Light(Light::LT_DIRECTIONAL);
    light->Ambient = white;
    light->Diffuse = white;
    light->Specular = black;
    light->SetDirection(AVector::UNIT_Z);

    LightDirPerVerEffect* effect = new0 LightDirPerVerEffect();
    mNoIntersectEffect = effect->CreateInstance(light, blueMaterial);
    mIntersectEffect = effect->CreateInstance(light, redMaterial);

    // Create visual representations of the boxes.  The normals are duplicated
    // to texture coordinates to avoid the AMD lighting problems due to use of
    // pre-OpenGL2.x extensions.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 1);

    for (i = 0; i < imax; ++i)
    {
        APoint center(
            0.5f*(mBoxes[i].Min[0] + mBoxes[i].Max[0]),
            0.5f*(mBoxes[i].Min[1] + mBoxes[i].Max[1]),
            0.5f*(mBoxes[i].Min[2] + mBoxes[i].Max[2]));

        Transform transform;
        transform.SetTranslate(center);

        float xExtent = 0.5f*(mBoxes[i].Max[0] - mBoxes[i].Min[0]);
        float yExtent = 0.5f*(mBoxes[i].Max[1] - mBoxes[i].Min[1]);
        float zExtent = 0.5f*(mBoxes[i].Max[2] - mBoxes[i].Min[2]);

        StandardMesh sm(vformat, true, false, &transform);
        TriMesh* mesh = sm.Box(xExtent, yExtent, zExtent);

        mesh->SetEffectInstance(mNoIntersectEffect);
        mScene->AttachChild(mesh);
    }
}
//----------------------------------------------------------------------------
void IntersectingBoxes::ModifyBoxes ()
{
    const int numBoxes = (int)mBoxes.size();
    int i;
    for (i = 0; i < numBoxes; ++i)
    {
        AxisAlignedBox3f box = mBoxes[i];

        float dx = Mathf::IntervalRandom(-4.0f, 4.0f);
        if (-mSize <= box.Min[0] + dx && box.Max[0] + dx <= mSize)
        {
            box.Min[0] += dx;
            box.Max[0] += dx;
        }

        float dy = Mathf::IntervalRandom(-4.0f, 4.0f);
        if (-mSize <= box.Min[1] + dy && box.Max[1] + dy <= mSize)
        {
            box.Min[1] += dy;
            box.Max[1] += dy;
        }

        float dz = Mathf::IntervalRandom(-4.0f, 4.0f);
        if (-mSize <= box.Min[2] + dz && box.Max[2] + dz <= mSize)
        {
            box.Min[2] += dz;
            box.Max[2] += dz;
        }

        mManager->SetBox(i, box);
        ModifyMesh(i);
    }

    mManager->Update();
    mScene->Update();

    // Switch material to red for any box that overlaps another.
    TriMesh* mesh;
    for (i = 0; i < numBoxes; ++i)
    {
        // Reset all boxes to blue.
        mesh = StaticCast<TriMesh>(mScene->GetChild(i));
        mesh->SetEffectInstance(mNoIntersectEffect);
    }

    const std::set<EdgeKey>& overlap = mManager->GetOverlap();
    std::set<EdgeKey>::const_iterator iter = overlap.begin();
    std::set<EdgeKey>::const_iterator end = overlap.end();
    for (/**/; iter != end; ++iter)
    {
        // Set intersecting boxes to red.
        i = iter->V[0];
        mesh = StaticCast<TriMesh>(mScene->GetChild(i));
        mesh->SetEffectInstance(mIntersectEffect);
        i = iter->V[1];
        mesh = StaticCast<TriMesh>(mScene->GetChild(i));
        mesh->SetEffectInstance(mIntersectEffect);
    }
}
//----------------------------------------------------------------------------
void IntersectingBoxes::ModifyMesh (int i)
{
    Vector3f center(
        0.5f*(mBoxes[i].Min[0] + mBoxes[i].Max[0]),
        0.5f*(mBoxes[i].Min[1] + mBoxes[i].Max[1]),
        0.5f*(mBoxes[i].Min[2] + mBoxes[i].Max[2]));

    float xExtent = 0.5f*(mBoxes[i].Max[0] - mBoxes[i].Min[0]);
    float yExtent = 0.5f*(mBoxes[i].Max[1] - mBoxes[i].Min[1]);
    float zExtent = 0.5f*(mBoxes[i].Max[2] - mBoxes[i].Min[2]);

    Vector3f xTerm = xExtent*Vector3f::UNIT_X;
    Vector3f yTerm = yExtent*Vector3f::UNIT_Y;
    Vector3f zTerm = zExtent*Vector3f::UNIT_Z;

    TriMesh* mesh = StaticCast<TriMesh>(mScene->GetChild(i));
    VertexBufferAccessor vba(mesh);

    vba.Position<Vector3f>(0) = center - xTerm - yTerm - zTerm;
    vba.Position<Vector3f>(1) = center + xTerm - yTerm - zTerm;
    vba.Position<Vector3f>(2) = center + xTerm + yTerm - zTerm;
    vba.Position<Vector3f>(3) = center - xTerm + yTerm - zTerm;
    vba.Position<Vector3f>(4) = center - xTerm - yTerm + zTerm;
    vba.Position<Vector3f>(5) = center + xTerm - yTerm + zTerm;
    vba.Position<Vector3f>(6) = center + xTerm + yTerm + zTerm;
    vba.Position<Vector3f>(7) = center - xTerm + yTerm + zTerm;

    mesh->UpdateModelSpace(Visual::GU_NORMALS);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }

    mRenderer->Update(mesh->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void IntersectingBoxes::PhysicsTick ()
{
    if (mDoSimulation)
    {
        float currIdle = (float)GetTimeInSeconds();
        float diff = currIdle - mLastIdle;
        if (diff >= 1.0f/30.0f)
        {
            ModifyBoxes();
            mLastIdle = currIdle;
        }
    }
}
//----------------------------------------------------------------------------
void IntersectingBoxes::GraphicsTick ()
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
