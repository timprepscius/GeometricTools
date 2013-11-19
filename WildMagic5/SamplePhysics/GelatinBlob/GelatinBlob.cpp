// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GelatinBlob.h"
#include "Wm5EdgeKey.h"

WM5_WINDOW_APPLICATION(GelatinBlob);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
GelatinBlob::GelatinBlob ()
    :
    WindowApplication3("SamplePhysics/GelatinBlob", 0, 0, 640, 480,
        Float4(0.713725f, 0.807843f, 0.929411f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mModule = 0;
}
//----------------------------------------------------------------------------
bool GelatinBlob::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

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
void GelatinBlob::OnTerminate ()
{
    delete0(mModule);

    mScene = 0;
    mTrnNode = 0;
    mSegments = 0;
    mWireState = 0;
    mIcosahedron = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GelatinBlob::OnIdle ()
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
bool GelatinBlob::OnKeyDown (unsigned char key, int x, int y)
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
void GelatinBlob::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateIcosahedron();
    CreateSprings();
    CreateSegments();

    // Segments are opaque. Draw them first (the icosahedron is transparent).
    mTrnNode->AttachChild(mSegments);
    mTrnNode->AttachChild(mIcosahedron);
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateIcosahedron ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mIcosahedron = StandardMesh(vformat).Icosahedron();

    // Texture for the water objects.
    std::string path = Environment::GetPathR("WaterWithAlpha.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT);
    mIcosahedron->SetEffectInstance(instance);

    // The texture has an alpha channel of 1/2 for all pixels.
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSprings ()
{
    // The icosahedron has 12 vertices and 30 edges.  Each vertex is a
    // particle in the system.  Each edge represents a spring.  To keep the
    // icosahedron from collapsing, 12 immovable particles are added, each
    // outside the icosahedron in the normal direction above a vertex.  The
    // immovable particles are connected to their corresponding vertices with
    // springs.
    int numParticles = 24, numSprings = 42;

    // Viscous forces applied.  If you set viscosity to zero, the cuboid
    // wiggles indefinitely since there is no dissipation of energy.  If
    // the viscosity is set to a positive value, the oscillations eventually
    // stop.  The length of time to steady state is inversely proportional
    // to the viscosity.
#ifdef _DEBUG
    float step = 0.1f;
#else
    float step = 0.01f;  // simulation needs to run slower in release mode
#endif
    float viscosity = 0.01f;
    mModule = new0 PhysicsModule(numParticles, numSprings, step, viscosity);

    // Set positions and velocities.  The first 12 positions are the vertices
    // of the icosahedron.  The last 12 are the extra particles added to
    // stabilize the system.
    VertexBufferAccessor vba(mIcosahedron);
    int i;
    for (i = 0; i < 12; ++i)
    {
        mModule->SetMass(i, 1.0f);
        mModule->Position(i) = vba.Position<Vector3f>(i);
        mModule->Velocity(i) = 0.1f*Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(), Mathf::SymmetricRandom());
    }
    for (i = 12; i < 24; ++i)
    {
        mModule->SetMass(i, Mathf::MAX_REAL);
        mModule->Position(i) = 2.0f*vba.Position<Vector3f>(i - 12);
        mModule->Velocity(i) = Vector3f::ZERO;
    }

    // Get unique set of edges for icosahedron.
    std::set<EdgeKey> edgeSet;
    int numTriangles = mIcosahedron->GetIndexBuffer()->GetNumElements()/3;
    int* indices = (int*)mIcosahedron->GetIndexBuffer()->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        int v0 = *indices++;
        int v1 = *indices++;
        int v2 = *indices++;
        edgeSet.insert(EdgeKey(v0, v1));
        edgeSet.insert(EdgeKey(v1, v2));
        edgeSet.insert(EdgeKey(v2, v0));
    }

    // Springs are at rest in the initial configuration.
    const float constant = 10.0f;
    Vector3f diff;
    int spring = 0;
    std::set<EdgeKey>::iterator iter = edgeSet.begin();
    std::set<EdgeKey>::iterator end = edgeSet.end();
    for (/**/; iter != end; ++iter)
    {
        const EdgeKey& key = *iter;
        int v0 = key.V[0], v1 = key.V[1];
        diff = mModule->Position(v1) - mModule->Position(v0);
        mModule->SetSpring(spring, v0, v1, constant, diff.Length());
        ++spring;
    }
    for (i = 0; i < 12; ++i)
    {
        diff = mModule->Position(i + 12) - mModule->Position(i);
        mModule->SetSpring(spring, i, i + 12, constant, diff.Length());
        ++spring;
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::CreateSegments ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexColor3Effect* effect = new0 VertexColor3Effect();
    VertexBufferAccessor vba;

    int numSprings = mModule->GetNumSprings();
    mSegments = new0 Node();
    for (int i = 0; i < numSprings; ++i)
    {
        int v0, v1;
        float constant, length;
        mModule->GetSpring(i, v0, v1, constant, length);

        VertexBuffer* vbuffer = new0 VertexBuffer(2, vstride);
        vba.ApplyTo(vformat, vbuffer);

        vba.Position<Vector3f>(0) = mModule->Position(v0);
        vba.Position<Vector3f>(1) = mModule->Position(v1);
        vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 1.0f);
        vba.Color<Float3>(0, 1) = Float3(1.0f, 1.0f, 1.0f);
        Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
        segment->SetEffectInstance(effect->CreateInstance());

        mSegments->AttachChild(segment);
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::PhysicsTick ()
{
    mModule->Update((float)GetTimeInSeconds());

    // Update icosahedrone.  The particle system and icosahedron maintain
    // their own copy of the vertices, so this update is necessary.
    VertexBufferAccessor vba(mIcosahedron);
    int i;
    for (i = 0; i < 12; ++i)
    {
        vba.Position<Vector3f>(i) = mModule->Position(i);
    }

    mIcosahedron->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    mRenderer->Update(mIcosahedron->GetVertexBuffer());

    // Update the segments representing the springs.
    int numSprings = mModule->GetNumSprings();
    for (i = 0; i < numSprings; ++i)
    {
        int v0, v1;
        float constant, length;
        mModule->GetSpring(i, v0, v1, constant, length);

        Polysegment* segment =
            StaticCast<Polysegment>(mSegments->GetChild(i));
        vba.ApplyTo(segment);
        vba.Position<Vector3f>(0) = mModule->Position(v0);
        vba.Position<Vector3f>(1) = mModule->Position(v1);
        mRenderer->Update(segment->GetVertexBuffer());
    }
}
//----------------------------------------------------------------------------
void GelatinBlob::GraphicsTick ()
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
