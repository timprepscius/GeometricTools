// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/08/04)

#include "BouncingTetrahedra.h"

WM5_WINDOW_APPLICATION(BouncingTetrahedra);

//----------------------------------------------------------------------------
BouncingTetrahedra::BouncingTetrahedra ()
    :
    WindowApplication3("SamplePhysics/BouncingTetrahedra", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mTolerance = 1e-12f;
    mTotalKE = 0.0f;
    mSimTime = 0.0f;
    mSimDeltaTime = 0.01f;
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    float angle = 0.02f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    APoint camPosition(27.5f, 8.0f, 8.9f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initialize the simulation.
    PhysicsTick();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::OnTerminate()
{
    for (int i = 0; i < NUM_TETRA; ++i)
    {
        delete0(mTetras[i]);
        mTetraNodes[i] = 0;
    }

    mScene = 0;
    mWireState = 0;
    mFloor = 0;
    mBackWall = 0;
    mSideWall1 = 0;
    mSideWall2 = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::OnIdle ()
{
    MeasureTime();
    PhysicsTick();
    GraphicsTick();
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
        mWireState->Enabled = !mWireState->Enabled;
        return true;

#ifdef SINGLE_STEP
    case 'g':
        mSimTime += mSimDeltaTime;
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateScene ()
{
    CreateTetra();
    CreateFloor();
    CreateBackWall();
    CreateSideWall1();
    CreateSideWall2();

    // ** layout of scene graph **
    // scene
    //     room
    //         floor
    //         back wall
    //         side wall 1
    //         side wall 2
    //     tetra

    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    Node* room = new0 Node();
    room->AttachChild(mFloor);
    room->AttachChild(mBackWall);
    room->AttachChild(mSideWall1);
    room->AttachChild(mSideWall2);
    mScene->AttachChild(room);

    Node* tetraRoot = new0 Node();
    int i;
    for (i = 0; i < NUM_TETRA; ++i)
    {
        tetraRoot->AttachChild(mTetraNodes[i]);
    }
    mScene->AttachChild(tetraRoot);

    // The tetrahedra are constrained to bounce around in a rectangular solid
    // region.  The six defining planes are defined to be immovable rigid
    // bodies.  The boundaries are parallel to coordinate axes and pass
    // through the points indicated by the value other than +-100.  That is,
    // the back wall is at x = 1, the left wall is at y = 2, the floor is at
    // z = 1, the right wall is at y = 15, the ceiling is at z = 17, and the
    // front wall is at x = 8.  The ceiling and front wall are invisible
    // objects (not rendered), but you will see balls bouncing against it
    // and reflecting away from it towards the back wall.
    mBoundaryLocations[0] = Vector3f(1.0f, -100.0f, -100.0f);
    mBoundaryNormals[0] = Vector3f(1.0f, 0.0f, 0.0f);
    mBoundaryLocations[1] = Vector3f(-100.0f, 2.0f, -100.0f);
    mBoundaryNormals[1] = Vector3f(0.0f, 1.0f, 0.0f);
    mBoundaryLocations[2] = Vector3f(-100.0f, -100.0f, 1.0f);
    mBoundaryNormals[2] = Vector3f(0.0f, 0.0f, 1.0f);
    mBoundaryLocations[3] = Vector3f(100.0f, 15.0f, 100.0f);
    mBoundaryNormals[3] = Vector3f(0.0f, -1.0f, 0.0f);
    mBoundaryLocations[4] = Vector3f(100.0f, 100.0f, 17.0f);
    mBoundaryNormals[4] = Vector3f(0.0f, 0.0f, -1.0f);
    mBoundaryLocations[5] = Vector3f(8.0f, 100.0f, 100.0f);
    mBoundaryNormals[5] = Vector3f(-1.0f, 0.0f, 0.0f);
    for (i = 0; i < 6; ++i)
    {
        mBoundaries[i].SetMass(0.0f);
        mBoundaries[i].SetPosition(mBoundaryLocations[i]);
    }

    // The face connectivity is the same for all the tetrahedra.  This
    // information is required by the LCP solver.
    mFaces[0][0] = 0;  mFaces[0][1] = 2;  mFaces[0][2] = 1;
    mFaces[1][0] = 0;  mFaces[1][1] = 3;  mFaces[1][2] = 2;
    mFaces[2][0] = 0;  mFaces[2][1] = 1;  mFaces[2][2] = 3;
    mFaces[3][0] = 1;  mFaces[3][1] = 2;  mFaces[3][2] = 3;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateTetra ()
{
    for (int i = 0; i < NUM_TETRA; ++i)
    {
        float size = 3.0f - 0.5f*i;
        float mass = 8.0f*size;
        Vector3f position = Vector3f(3.0f, 3.0f + 3.0f*i, 14.0f - 3.0f*i);
        Vector3f linearMomentum = 0.01f*Vector3f(
            2.0f + 100.0f*Mathf::SymmetricRandom(),
            2.0f + 100.0f*Mathf::SymmetricRandom(),
            -1.2f + 100.0f*Mathf::SymmetricRandom());
        Vector3f angularMomentum = 0.01f*Vector3f(
            1.0f + 100.0f*Mathf::SymmetricRandom(),
            2.0f + 100.0f*Mathf::SymmetricRandom(),
            3.0f + 100.0f*Mathf::SymmetricRandom());

        mTetras[i] = new0 RigidTetra(size, mass, position, linearMomentum,
            angularMomentum);

        mTetras[i]->mForce = Force;
        mTetras[i]->mTorque = Torque;

        mTetraNodes[i] = new0 Node();
        mTetraNodes[i]->AttachChild(mTetras[i]->Mesh());
    }
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateFloor ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    Float3 floorColor(155.0f/255.0f, 177.0f/255.0f, 164.0f/255.0f);

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3( 1.0f,  1.0f, 1.0f);
    vba.Position<Float3>(1) = Float3(17.0f,  1.0f, 1.0f);
    vba.Position<Float3>(2) = Float3(17.0f, 20.0f, 1.0f);
    vba.Position<Float3>(3) = Float3( 1.0f, 20.0f, 1.0f);
    vba.Color<Float3>(0 ,0) = floorColor;
    vba.Color<Float3>(0 ,1) = floorColor;
    vba.Color<Float3>(0 ,2) = floorColor;
    vba.Color<Float3>(0 ,3) = floorColor;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mFloor = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFloor->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateSideWall1 ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    Float3 sideWall1Color(170.0f/255.0f, 187.0f/255.0f, 219.0f/255.0f);

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3( 1.0f, 15.0f,  1.0f);
    vba.Position<Float3>(1) = Float3(17.0f, 15.0f,  1.0f);
    vba.Position<Float3>(2) = Float3(17.0f, 15.0f, 17.0f);
    vba.Position<Float3>(3) = Float3( 1.0f, 15.0f, 17.0f);
    vba.Color<Float3>(0, 0) = sideWall1Color;
    vba.Color<Float3>(0, 1) = sideWall1Color;
    vba.Color<Float3>(0, 2) = sideWall1Color;
    vba.Color<Float3>(0, 3) = sideWall1Color;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mSideWall1 = new0 TriMesh(vformat, vbuffer, ibuffer);
    mSideWall1->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateSideWall2 ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    Float3 sideWall2Color(170.0f/255.0f, 187.0f/255.0f, 219.0f/255.0f);

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(17.0f, 2.0f,  1.0f);
    vba.Position<Float3>(1) = Float3( 1.0f, 2.0f,  1.0f);
    vba.Position<Float3>(2) = Float3( 1.0f, 2.0f, 17.0f);
    vba.Position<Float3>(3) = Float3(17.0f, 2.0f, 17.0f);
    vba.Color<Float3>(0, 0) = sideWall2Color;
    vba.Color<Float3>(0, 1) = sideWall2Color;
    vba.Color<Float3>(0, 2) = sideWall2Color;
    vba.Color<Float3>(0, 3) = sideWall2Color;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mSideWall2 = new0 TriMesh(vformat, vbuffer, ibuffer);
    mSideWall2->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CreateBackWall ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    Float3 backWallColor(209.0f/255.0f, 204.0f/255.0f, 180.0f/255.0f);

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(1.0f,  1.0f,  1.0f);
    vba.Position<Float3>(1) = Float3(1.0f, 20.0f,  1.0f);
    vba.Position<Float3>(2) = Float3(1.0f, 20.0f, 17.0f);
    vba.Position<Float3>(3) = Float3(1.0f,  1.0f, 17.0f);
    vba.Color<Float3>(0, 0) = backWallColor;
    vba.Color<Float3>(0, 1) = backWallColor;
    vba.Color<Float3>(0, 2) = backWallColor;
    vba.Color<Float3>(0, 3) = backWallColor;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mBackWall = new0 TriMesh(vformat, vbuffer, ibuffer);
    mBackWall->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::PhysicsTick ()
{
    DoCollisionDetection();
    DoCollisionResponse();

    mTotalKE = 0.0f;
    for (int i = 0; i < NUM_TETRA; ++i)
    {
        const RigidTetra& tetra = *mTetras[i];
        float invMass = tetra.GetInverseMass();
        const Matrix3f& inertia = tetra.GetWorldInertia();
        const Vector3f& position = tetra.GetPosition();
        const Vector3f& linearMomentum = tetra.GetLinearMomentum();
        const Matrix3f& rotOrient = tetra.GetROrientation();
        const Vector3f& angularVelocity = tetra.GetAngularVelocity();

        mTetraNodes[i]->LocalTransform.SetTranslate(position);
        mTetraNodes[i]->LocalTransform.SetRotate(rotOrient);

        mTotalKE += invMass*linearMomentum.Dot(linearMomentum) +
            angularVelocity.Dot(inertia*angularVelocity);
    }
    mTotalKE *= 0.5f;

    mScene->Update();
    mSimTime += mSimDeltaTime;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[256];
        sprintf(message, "Time = %5.2f", mSimTime);
        mRenderer->Draw(95, GetHeight()-8, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::DoCollisionDetection ()
{
    int i, j;
    Contact contact;
    mContacts.clear();

    // Test for tetrahedron-boundary collisions.
    for (i = 0; i < NUM_TETRA; ++i)
    {
        mTetras[i]->Moved = false;
        if (FarFromBoundary(i))
        {
            continue;
        }

        // These checks are done in pairs under the assumption that the tetra 
        // have smaller diameters than the separation of opposite boundaries, 
        // hence only one of each opposite pair of boundaries may be touched 
        // at any one time.
        Vector3f vertices[4];
        float distances[4];
        mTetras[i]->GetVertices(vertices);
        float radius = mTetras[i]->GetRadius();
        Vector3f position = mTetras[i]->GetPosition();

        // rear[0] and front[5] boundaries
        if (position.X() - radius < mBoundaryLocations[0].X())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = vertices[j].X() - mBoundaryLocations[0].X();
            }
            TetraBoundaryIntersection(i, 0, distances, contact);
        }
        else if (position.X() + radius > mBoundaryLocations[5].X())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = mBoundaryLocations[5].X() - vertices[j].X();
            }
            TetraBoundaryIntersection(i, 5, distances, contact);
        }

        // left[1] and right[3] boundaries
        if (position.Y() - radius < mBoundaryLocations[1].Y())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = vertices[j].Y() - mBoundaryLocations[1].Y();
            }
            TetraBoundaryIntersection(i, 1, distances, contact);
        }
        else if (position.Y() + radius > mBoundaryLocations[3].Y())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = mBoundaryLocations[3].Y() - vertices[j].Y();
            }
            TetraBoundaryIntersection(i, 3, distances, contact);
        }

        // bottom[2] and top[4] boundaries
        if (position.Z() - radius < mBoundaryLocations[2].Z())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = vertices[j].Z() - mBoundaryLocations[2].Z();
            }
            TetraBoundaryIntersection(i, 2, distances, contact);
        }
        else if (position.Z() + radius > mBoundaryLocations[4].Z())
        {
            for (j = 0; j < 4; ++j)
            {
                distances[j] = mBoundaryLocations[4].Z() - vertices[j].Z();
            }
            TetraBoundaryIntersection(i, 4, distances, contact);
        }
    }    

    // Test for tetrahedron-tetrahedron collisions.
    mLCPCount = 0;
    for (i = 0; i < NUM_TETRA-1; ++i)
    {
        Vector3f vertices0[4];
        mTetras[i]->GetVertices(vertices0);

        for (j = i + 1; j < NUM_TETRA; ++j)
        {
            Vector3f vertices1[4];
            mTetras[j]->GetVertices(vertices1);

            if (!FarApart(i, j))
            {
                float dist = 1.0f;
                int statusCode = 0;
                Vector3f closest[2];
                LCPPolyDist3(4, vertices0, 4,mFaces, 4, vertices1, 4,
                    mFaces, statusCode, dist, closest);
                ++mLCPCount;
                if (dist <= mTolerance)
                {
                    // Collision with good LCPPolyDist results.
                    Reposition(i, j, contact);
                    mContacts.push_back(contact);
                }
            }
        }
    }

    mNumContacts = (int)mContacts.size();
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::FarFromBoundary (int i)
{
    // The tests are arranged so that the most likely to be encountered
    // (the floor) is tested first and the least likely to be encountered
    // (the ceiling) is tested last.

    Vector3f position = mTetras[i]->GetPosition();
    float radius = mTetras[i]->GetRadius();

    return position.Z() - radius >= mBoundaryLocations[2].Z()
        && position.X() - radius >= mBoundaryLocations[0].X()
        && position.X() + radius <= mBoundaryLocations[5].X()
        && position.Y() - radius >= mBoundaryLocations[1].Y()
        && position.Y() + radius <= mBoundaryLocations[3].Y()
        && position.Z() + radius <= mBoundaryLocations[4].Z();
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::FarApart (int t0, int t1)
{
    Vector3f position0 = mTetras[t0]->GetPosition();
    float radius0 = mTetras[t0]->GetRadius();
    Vector3f position1 = mTetras[t1]->GetPosition();
    float radius1 = mTetras[t1]->GetRadius();
    return (position0 - position1).Length() >= radius0 + radius1;
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::TetraBoundaryIntersection (int tetra, int boundary,
    float* distances, Contact& contact)
{
    int hitIndex = -1;
    float depthMax = 0.0f;
    for (int j = 0; j < 4; ++j)
    {
        float depth = distances[j];
        if (depth < depthMax)
        {
            depthMax = depth;
            hitIndex = j;
        }
    }
    if (hitIndex != -1)
    {
        BuildContactMoveTetra(tetra, boundary, hitIndex, depthMax, contact);
        mContacts.push_back(contact);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::BuildContactMoveTetra (int tetra, int boundary,
    int hitIndex, float depthMax, Contact& contact)
{
    contact.A = &mBoundaries[boundary];
    contact.B = mTetras[tetra];
    contact.IsVFContact = true;
    contact.N = mBoundaryNormals[boundary];
    contact.PA = Vector3f::ZERO;

    Vector3f vertices[4];
    mTetras[tetra]->GetVertices(vertices);
    contact.PB = vertices[hitIndex];
    
    // Move intersecting tetra to surface of boundary.
    Vector3f position = mTetras[tetra]->GetPosition();
    mTetras[tetra]->SetPosition(position - depthMax*contact.N);
    mTetras[tetra]->Moved = true;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::Reposition (int t0, int t1, Contact& contact)
{
    RigidTetra& tetra0 = *mTetras[t0];
    RigidTetra& tetra1 = *mTetras[t1];

    // Compute the centroids of the tetrahedra.
    Vector3f vertices0[4], vertices1[4];
    tetra0.GetVertices(vertices0);
    tetra1.GetVertices(vertices1);
    Vector3f centroid0 = Vector3f::ZERO;
    Vector3f centroid1 = Vector3f::ZERO;
    int i;
    for (i = 0; i < 4; ++i)
    {
        centroid0 += vertices0[i];
        centroid1 += vertices1[i];
    }
    centroid0 *= 0.25f;
    centroid1 *= 0.25f;

    // Randomly perturb the tetrahedra vertices by a small amount.  This is
    // done to help prevent the LCP solver from getting into cycles and
    // degenerate cases.
    const float reduction = 0.95f;
    float reduceI = reduction*Mathf::IntervalRandom(0.9999f, 1.0001f);
    float reduceJ = reduction*Mathf::IntervalRandom(0.9999f, 1.0001f);
    for (i = 0; i < 4; ++i)
    {
        vertices0[i] = centroid0 + (vertices0[i] - centroid0)*reduceI;
        vertices1[i] = centroid1 + (vertices1[i] - centroid1)*reduceJ;
    }

    // Compute the distance between the tetrahedra.
    float dist = 1.0f;
    int statusCode = 0;
    Vector3f closest[2];
    LCPPolyDist3(4, vertices0, 4, mFaces, 4, vertices1, 4, mFaces,
        statusCode, dist, closest);
    ++mLCPCount;

    // In theory, LCPPolyDist<3> should always find a valid distance, but just
    // in case numerical round-off errors cause problems, let us trap it.
    assertion(dist >= 0.0f, "LCP polyhedron distance calculator failed.\n");

    // Reposition the tetrahedra to the theoretical points of contact.
    closest[0] = centroid0 + (closest[0] - centroid0)/reduceI;
    closest[1] = centroid1 + (closest[1] - centroid1)/reduceJ;
    for (i = 0; i < 4; ++i)
    {
        vertices0[i] = centroid0 + (vertices0[i] - centroid0)/reduceI;
        vertices1[i] = centroid1 + (vertices1[i] - centroid1)/reduceJ;
    }

    // Numerical round-off errors can cause interpenetration.  Move the
    // tetrahedra to back out of this situation.  The length of diff
    // estimates the depth of penetration when dist > 0 was reported.
    Vector3f diff = closest[0] - closest[1];

    // Apply the separation distance along the line containing the centroids
    // of the tetrahedra.
    Vector3f diff2 = centroid1 - centroid0;
    diff = diff2/diff2.Length()*diff.Length();

    // Move each tetrahedron by half of kDiff when the distance was large,
    // but move each by twice kDiff when the distance is really small.
    float mult = (dist >= mTolerance ? 0.5f : 1.0f);
    Vector3f delta = mult*diff;

    // Undo the interpenetration.
    if (tetra0.Moved && !tetra1.Moved)
    {
        // Tetra t0 has moved but tetra t1 has not moved.
        tetra1.SetPosition(tetra1.GetPosition() + 2.0f*delta);
        tetra1.Moved = true;
    }
    else if (!tetra0.Moved && tetra1.Moved)
    {
        // Tetra t1 has moved but tetra t0 has not moved.
        tetra0.SetPosition(tetra0.GetPosition() - 2.0f*delta);
        tetra0.Moved = true;
    }
    else
    {
        // Both tetras moved or both tetras did not move.
        tetra0.SetPosition(tetra0.GetPosition() - delta);
        tetra0.Moved = true;
        tetra1.SetPosition(tetra1.GetPosition() + delta);
        tetra1.Moved = true;
    }

    // Test whether the two tetrahedra intersect in a vertex-face
    // configuration.
    contact.IsVFContact = IsVertex(vertices0, closest[0]);
    if (contact.IsVFContact)
    {
        contact.A = mTetras[t1];
        contact.B = mTetras[t0];
        CalculateNormal(vertices1, closest[1], contact);
    }
    else
    {
        contact.IsVFContact = IsVertex(vertices1, closest[1]);
        if (contact.IsVFContact)
        {
            contact.A = mTetras[t0];
            contact.B = mTetras[t1];
            CalculateNormal(vertices0, closest[0], contact);
        }
    }

    // Test whether the two tetrahedra intersect in an edge-edge
    // configuration.
    if (!contact.IsVFContact)
    {
        contact.A = mTetras[t0];
        contact.B = mTetras[t1];
        Vector3f otherVertexA = Vector3f::UNIT_X;
        Vector3f otherVertexB = Vector3f::ZERO;
        contact.EA = ClosestEdge(vertices0, closest[0], otherVertexA);
        contact.EB = ClosestEdge(vertices1, closest[1], otherVertexB);
        Vector3f normal = contact.EA.UnitCross(contact.EB);
        if (normal.Dot(otherVertexA - closest[0]) < 0.0f)
        {
            contact.N = normal;
        }
        else
        {
            contact.N = -normal;
        }
    }

    // Reposition results to correspond to relocaton of tetra.
    contact.PA = closest[0] - delta;
    contact.PB = closest[1] + delta;
}
//----------------------------------------------------------------------------
bool BouncingTetrahedra::IsVertex (const Vector3f* vertices,
    const Vector3f& closest)
{
    for (int i = 0; i < 4; ++i)
    {
        Vector3f diff = vertices[i] - closest;
        if (diff.Length() < mTolerance)
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::CalculateNormal (const Vector3f* vertices,
    const Vector3f& closest, Contact& contact)
{
    float diff = Mathf::MAX_REAL;
    for (int i = 0; i < 4; ++i)
    {
        Plane3f plane = Plane3f(vertices[mFaces[i][0]],
            vertices[mFaces[i][1]], vertices[mFaces[i][2]]);

        float temp = Mathf::FAbs(plane.DistanceTo(closest));
        if (temp < diff)
        {
            contact.N = plane.Normal;
            diff = temp;
        }
    }
}
//----------------------------------------------------------------------------
Vector3f BouncingTetrahedra::ClosestEdge (const Vector3f* vertices,
    const Vector3f& closest, Vector3f& otherVertex)
{
    // Find the edge of the tetrahedra nearest to the contact point.  If
    // otherVertexB is ZERO, then ClosestEdge skips the calculation of an
    // unneeded other-vertex for the tetrahedron B.

    Vector3f closestEdge;
    float minDist = Mathf::MAX_REAL;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = i + 1; j < 4; ++j)
        {
            Vector3f edge = vertices[j] - vertices[i];
            Vector3f diff = closest - vertices[i];
            float DdE = diff.Dot(edge);
            float edgeLength = edge.Length();
            float diffLength = diff.Length();
            float dist = Mathf::FAbs(DdE/(edgeLength*diffLength) - 1.0f);
            if (dist < minDist)
            {
                minDist = dist;
                closestEdge = edge;
                for (int k = 0; otherVertex != Vector3f::ZERO && k < 3; ++k)
                {
                    if (k != i && k != j)
                    {
                        otherVertex = vertices[k];
                        continue;
                    }
                }
            }
        }
    }
    return closestEdge;
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::DoCollisionResponse ()
{
    if (mNumContacts > 0)
    {
        float* preRelVelocities = new1<float>(mNumContacts);
        float* impulseMagnitudes = new1<float>(mNumContacts);

        ComputePreimpulseVelocity(preRelVelocities);
        ComputeImpulseMagnitude(preRelVelocities, impulseMagnitudes);
        DoImpulse(impulseMagnitudes);

        delete1(preRelVelocities);
        delete1(impulseMagnitudes);
    }

    for (int i = 0; i < NUM_TETRA; ++i)
    {
        mTetras[i]->Update(mSimTime, mSimDeltaTime);
    }
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::ComputePreimpulseVelocity (float* preRelVelocities)
{
    for (int i = 0; i < mNumContacts; ++i)
    {
        const Contact& contact = mContacts[i];
        const RigidBodyf& bodyA = *contact.A;
        const RigidBodyf& bodyB = *contact.B;

        Vector3f XA = bodyA.GetPosition();
        Vector3f XB = bodyB.GetPosition();
        Vector3f VA = bodyA.GetLinearVelocity();
        Vector3f VB = bodyB.GetLinearVelocity();
        Vector3f WA = bodyA.GetAngularVelocity();
        Vector3f WB = bodyB.GetAngularVelocity();

        Vector3f relA = contact.PA - XA;
        Vector3f relB = contact.PB - XB;
        Vector3f velA = VA + WA.Cross(relA);
        Vector3f velB = VB + WB.Cross(relB);
        preRelVelocities[i] = contact.N.Dot(velB - velA);
    }
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::ComputeImpulseMagnitude (float* preRelVelocities,
    float* impulseMagnitudes)
{
    // The coefficient of restitution.
    float restitution = 0.8f;
    float temp = 20.0f*NUM_TETRA;
    if (mTotalKE < temp)
    {
        restitution *= 0.5f*mTotalKE/temp;
    }
    float coeff = -(1.0f + restitution);

    for (int i = 0; i < mNumContacts; ++i)
    {
        if (preRelVelocities[i] < 0.0f)
        {
            const Contact& contact = mContacts[i];
            const RigidBodyf& bodyA = *contact.A;
            const RigidBodyf& bodyB = *contact.B;

            Vector3f velDiff = bodyA.GetLinearVelocity() -
                bodyB.GetLinearVelocity();
            Vector3f relA = contact.PA - bodyA.GetPosition();
            Vector3f relB = contact.PB - bodyB.GetPosition();
            Vector3f AxN = relA.Cross(contact.N);
            Vector3f BxN = relB.Cross(contact.N);
            Vector3f JInvAxN = bodyA.GetWorldInverseInertia()*AxN;
            Vector3f JInvBxN = bodyB.GetWorldInverseInertia()*BxN;

            float numer = coeff*(contact.N.Dot(velDiff)
                + bodyA.GetAngularVelocity().Dot(AxN)
                - bodyB.GetAngularVelocity().Dot(BxN));

            float denom = bodyA.GetInverseMass() + bodyB.GetInverseMass()
                + AxN.Dot(JInvAxN) + BxN.Dot(JInvBxN);

            impulseMagnitudes[i] = numer/denom;
        }
        else
        {
            impulseMagnitudes[i] = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
void BouncingTetrahedra::DoImpulse (float* impulseMagnitudes)
{
    for (int i = 0; i < mNumContacts; ++i)
    {
        Contact& contact = mContacts[i];
        RigidBodyf& bodyA = *contact.A;
        RigidBodyf& bodyB = *contact.B;

        Vector3f PA = bodyA.GetLinearMomentum();
        Vector3f PB = bodyB.GetLinearMomentum();
        Vector3f LA = bodyA.GetAngularMomentum();
        Vector3f LB = bodyB.GetAngularMomentum();

        Vector3f impulse = impulseMagnitudes[i]*contact.N;
        PA += impulse;
        PB -= impulse;
        Vector3f relA = contact.PA - bodyA.GetPosition();
        LA += relA.Cross(impulse);
        Vector3f relB = contact.PB - bodyB.GetPosition();
        LB -= relB.Cross(impulse);

        bodyA.SetLinearMomentum(PA);
        bodyB.SetLinearMomentum(PB);
        bodyA.SetAngularMomentum(LA);
        bodyB.SetAngularMomentum(LB);
    }
}
//----------------------------------------------------------------------------
Vector3f BouncingTetrahedra::Force (float, float mass, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    const float gravityConstant = 9.81f;
    const Vector3f gravityDirection = -Vector3f::UNIT_Z;
    return (gravityConstant*mass)*gravityDirection;
}
//----------------------------------------------------------------------------
Vector3f BouncingTetrahedra::Torque (float, float, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    return Vector3f::ZERO;
}
//----------------------------------------------------------------------------
