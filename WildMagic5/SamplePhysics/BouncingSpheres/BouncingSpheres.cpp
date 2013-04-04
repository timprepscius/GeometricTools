// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BouncingSpheres.h"

WM5_WINDOW_APPLICATION(BouncingSpheres);

//----------------------------------------------------------------------------
BouncingSpheres::BouncingSpheres ()
    :
    WindowApplication3("SamplePhysics/BouncingSpheres", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mSimTime = 0.0f;
    mSimDeltaTime = 1.0f/10.0f;
}
//----------------------------------------------------------------------------
bool BouncingSpheres::OnInitialize ()
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

    // Initialize balls with correct transformations.
    PhysicsTick();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BouncingSpheres::OnTerminate()
{
    for (int i = 0; i < NUM_BALLS; ++i)
    {
        delete0(mBalls[i]);
        mBallNodes[i] = 0;
    }

    mScene = 0;
    mWireState = 0;
    mFloor = 0;
    mSideWall1 = 0;
    mSideWall2 = 0;
    mBackWall = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BouncingSpheres::OnIdle ()
{
    MeasureTime();
    PhysicsTick();
    GraphicsTick();
    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BouncingSpheres::OnKeyDown (unsigned char key, int x, int y)
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
    }

    return false;
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateScene ()
{
    CreateBalls();
    CreateFloor();
    CreateBackWall();
    CreateSideWall1();
    CreateSideWall2();

    // ** layout of scene graph **
    // scene
    //     room
    //         backwall
    //         floor
    //         sidewall1
    //         sidewall2
    //     balls

    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    Node* room = new0 Node();
    room->AttachChild(mFloor);
    room->AttachChild(mSideWall1);
    room->AttachChild(mSideWall2);
    room->AttachChild(mBackWall);
    mScene->AttachChild(room);

    Node* ballRoot = new0 Node();
    int i;
    for (i = 0; i < NUM_BALLS; ++i)
    {
        ballRoot->AttachChild(mBallNodes[i]);
    }
    mScene->AttachChild(ballRoot);

    // The balls are constrained to bounce around in a rectangular solid
    // region.  The six defining planes are defined to be immovable rigid
    // bodies.  The boundaries are parallel to coordinate axes and pass
    // through the points indicated by the value other than +-100.  That is,
    // the back wall is at x = 1, the left wall is at y = 2, the floor is at
    // z = 1, the right wall is at y = 15, the ceiling is at z = 17, and the
    // front wall is at x = 9.  The ceiling and front wall are invisible
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
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateBalls ()
{
    // TODO.  Adjust this so that the physics simulation runs with the
    // real clock.
    mSimDeltaTime = 0.001f;

    Vector3f position, linearMomentum;
    float mass = 2.0f;

    for (int i = 0; i < NUM_BALLS; ++i)
    {
        mBalls[i] = new0 RigidBall(Mathf::IntervalRandom(0.25f, 1.0f));
        RigidBall& ball = *mBalls[i];
        mBallNodes[i] = new0 Node();
        mBallNodes[i]->AttachChild(ball.Mesh());
        mass += 1.2f;
        if (i > 3)
        {
            position = Vector3f(5.0f, 4.0f, 13.0f) -
                1.0f*(i - 4.0f)*Vector3f::UNIT_Z +
                2.0f*(i - 4.0f)*Vector3f::UNIT_Y;
        }
        else
        {
            position = Vector3f(3.0f, 4.0f, 10.0f) -
                2.0f*i*Vector3f::UNIT_Z + 2.0f*i*Vector3f::UNIT_Y;
        }

        linearMomentum = Vector3f(2.0f, 2.0f, -1.2f) +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_X +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_Y +
            50.0f*(Mathf::SymmetricRandom())*Vector3f::UNIT_Z ;

        ball.SetMass(mass);
        ball.SetPosition(position);
        ball.SetLinearMomentum(linearMomentum);
        ball.mForce = Force;
        ball.mTorque = Torque;
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateFloor ()
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
    vba.Color<Float3>(0, 0) = floorColor;
    vba.Color<Float3>(0, 1) = floorColor;
    vba.Color<Float3>(0, 2) = floorColor;
    vba.Color<Float3>(0, 3) = floorColor;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mFloor = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFloor->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateSideWall1 ()
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
void BouncingSpheres::CreateSideWall2 ()
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
    vba.Color<Float3>(0,0) = sideWall2Color;
    vba.Color<Float3>(0,1) = sideWall2Color;
    vba.Color<Float3>(0,2) = sideWall2Color;
    vba.Color<Float3>(0,3) = sideWall2Color;

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mSideWall2 = new0 TriMesh(vformat, vbuffer, ibuffer);
    mSideWall2->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
}
//----------------------------------------------------------------------------
void BouncingSpheres::CreateBackWall ()
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
void BouncingSpheres::PhysicsTick ()
{
    DoCollisionDetection();
    DoCollisionResponse();

    for (int i = 0; i < NUM_BALLS; ++i)
    {
        mBallNodes[i]->LocalTransform.SetTranslate(mBalls[i]->GetPosition());
    }

    mScene->Update();

    mSimTime += mSimDeltaTime;
}
//----------------------------------------------------------------------------
void BouncingSpheres::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[256];
        sprintf(message, "Time = %5.2f", mSimTime);
        mRenderer->Draw(90, GetHeight()-8, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoCollisionDetection ()
{
    mBoundaryContacts.clear();

    // Collisions with boundaries.
    Contact contact;
    int i;
    for (i = 0; i < NUM_BALLS; ++i)
    {
        Vector3f position = mBalls[i]->GetPosition();
        float radius = mBalls[i]->GetRadius();
        mBalls[i]->Moved = false;
        mBlocked[i].clear();

        // These checks are done in pairs under the assumption that the ball 
        // radii are smaller than the separation of opposite boundaries, hence
        // only one of each opposite pair of boundaries may be touched at any
        // time.

        // rear[0] and front[5] boundaries
        if (position.X() < mBoundaryLocations[0].X() + radius)
        {
            SetBoundaryContact(i, 0, position, radius, contact);
        }
        else if (position.X() > mBoundaryLocations[5].X() - radius)
        {
            SetBoundaryContact(i, 5, position, radius, contact);
        }

        // left[1] and right[3] boundaries
        if (position.Y() < mBoundaryLocations[1].Y() + radius)
        {
            SetBoundaryContact(i, 1, position, radius, contact);
        }
        else if (position.Y() > mBoundaryLocations[3].Y() - radius)
        {
            SetBoundaryContact(i, 3, position, radius, contact);
        }

        // bottom[2] and top[4] boundaries
        if (position.Z() < mBoundaryLocations[2].Z() + radius)
        {
            SetBoundaryContact(i, 2, position, radius, contact);
        }
        else if (position.Z() > mBoundaryLocations[4].Z() - radius)
        {
            SetBoundaryContact(i, 4, position, radius, contact);
        }
    }

    // Collisions between balls.
    for (i = 0; i < NUM_BALLS-1; ++i)
    {
        for (int j = i + 1; j < NUM_BALLS; ++j)
        {
            Vector3f diff =
                mBalls[j]->GetPosition() - mBalls[i]->GetPosition();
            float diffLen = diff.Length();
            float radiusI = mBalls[i]->GetRadius();
            float radiusJ = mBalls[j]->GetRadius();
            float magnitude = diffLen - radiusI - radiusJ;
            if (magnitude < 0.0f)
            {
                contact.A = mBalls[i];
                contact.B = mBalls[j];
                contact.N = diff/diffLen;
                Vector3f deltaPos = magnitude*contact.N;

                if (mBalls[i]->Moved && !mBalls[j]->Moved)
                {
                    // Ball i moved but ball j did not.
                    mBalls[j]->Position() -= deltaPos;
                }
                else if (!mBalls[i]->Moved && mBalls[j]->Moved)
                {
                    // Ball j moved but ball i did not.
                    mBalls[i]->Position() += deltaPos;
                }
                else
                {
                    // Neither ball moved or both balls moved already.
                    deltaPos *= 0.5f;
                    mBalls[j]->Position() -= deltaPos;
                    mBalls[i]->Position() += deltaPos;
                }

                contact.P = mBalls[i]->Position() + radiusI*contact.N; 
                mBoundaryContacts.push_back(contact);
            }
        }
    }

    mNumContacts = (int)mBoundaryContacts.size();
}
//----------------------------------------------------------------------------
void BouncingSpheres::SetBoundaryContact (int i, int bIndex, 
    const Vector3f& position, float radius, Contact& contact)
{
    contact.B = mBalls[i];
    contact.A = &mBoundaries[bIndex];
    contact.N = mBoundaryNormals[bIndex];
    contact.P = position;
    mBlocked[i].push_back(-contact.N);
    mBoundaryContacts.push_back(contact);

    Vector3f bPosition = mBoundaries[bIndex].GetPosition();
    switch (bIndex)
    {
    case 0:
        contact.B->Position().X() = bPosition.X() + radius;
        break;
    case 1:
        contact.B->Position().Y() = bPosition.Y() + radius;
        break;
    case 2:
        contact.B->Position().Z() = bPosition.Z() + radius;
        break;
    case 3:
        contact.B->Position().Y() = bPosition.Y() - radius;
        break;
    case 4:
        contact.B->Position().Z() = bPosition.Z() - radius;
        break;
    case 5:
        contact.B->Position().X() = bPosition.X() - radius;
        break;
    default:
        assertion(false, "Should not get here.\n");
        break;
    }
    mBalls[i]->Moved = true;
}
//----------------------------------------------------------------------------
void BouncingSpheres::DoCollisionResponse ()
{
    if (mNumContacts > 0)
    {
        float* preRelVelocities = new1<float>(mNumContacts);
        float* impulseMagnitudes = new1<float>(mNumContacts);

        ComputePreimpulseVelocity(preRelVelocities);
        ComputeImpulseMagnitude(preRelVelocities, impulseMagnitudes);

        for (int i = 0; i < mNumContacts; ++i)
        {
            Contact& contact = mBoundaryContacts[i];
            RigidBodyf& bodyA = *contact.A;
            RigidBodyf& bodyB = *contact.B;

            Vector3f impulse = impulseMagnitudes[i]*contact.N;
            bodyA.SetLinearMomentum(bodyA.GetLinearMomentum() + impulse);
            bodyB.SetLinearMomentum(bodyB.GetLinearMomentum() - impulse);
        }

        delete1(preRelVelocities);
        delete1(impulseMagnitudes);
    }

    for (int i = 0; i < NUM_BALLS; ++i)
    {
        mBalls[i]->Update(mSimTime, mSimDeltaTime);
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::ComputeImpulseMagnitude (float* preRelVelocities,
    float* impulseMagnitudes)
{
    // The coefficient of restitution.
    const float restitution = 0.8f;

    Vector3f linVelDiff, relA, relB;
    Vector3f AxN, BxN, JInvAxN, JInvBxN;

    for (int i = 0; i < mNumContacts; ++i)
    {
        const Contact& contact = mBoundaryContacts[i];
        const RigidBodyf& bodyA = *contact.A;
        const RigidBodyf& bodyB = *contact.B;

        if (preRelVelocities[i] < 0.0f)
        {
            linVelDiff =
                bodyA.GetLinearVelocity() - bodyB.GetLinearVelocity();
            relA = contact.P - bodyA.GetPosition();
            relB = contact.P - bodyB.GetPosition();
            AxN = relA.Cross(contact.N);
            BxN = relB.Cross(contact.N);
            JInvAxN = bodyA.GetWorldInverseInertia()*AxN;
            JInvBxN = bodyB.GetWorldInverseInertia()*BxN;

            float numer = -(1.0f + restitution)*(contact.N.Dot(linVelDiff)
                + bodyA.GetAngularVelocity().Dot(AxN)
                - bodyB.GetAngularVelocity().Dot(BxN));

            float denom = bodyA.GetInverseMass() + bodyB.GetInverseMass()
                + AxN.Dot(JInvAxN)
                + BxN.Dot(JInvBxN);

            impulseMagnitudes[i] = numer/denom;
        }
        else
        {
            impulseMagnitudes[i] = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
void BouncingSpheres::ComputePreimpulseVelocity (float* preRelVelocities)
{
    for (int i = 0; i < mNumContacts; ++i)
    {
        const Contact& contact = mBoundaryContacts[i];
        const RigidBodyf& bodyA = *contact.A;
        const RigidBodyf& bodyB = *contact.B;

        Vector3f relA = contact.P - bodyA.GetPosition();
        Vector3f relB = contact.P - bodyB.GetPosition();
        Vector3f velA = bodyA.GetLinearVelocity() +
            bodyA.GetAngularVelocity().Cross(relA);
        Vector3f velB = bodyB.GetLinearVelocity() +
            bodyB.GetAngularVelocity().Cross(relB);
        preRelVelocities[i] = contact.N.Dot(velB - velA);
    }
}
//----------------------------------------------------------------------------
Vector3f BouncingSpheres::Force (float, float mass, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    const float gravityConstant = 9.81f;   // m/sec/sec
    const Vector3f gravityDirection = Vector3f(0.0f, 0.0f, -1.0f);
    return (mass*gravityConstant)*gravityDirection;
}
//----------------------------------------------------------------------------
Vector3f BouncingSpheres::Torque (float, float, const Vector3f&,
    const Quaternionf&, const Vector3f&, const Vector3f&, const Matrix3f&,
    const Vector3f&, const Vector3f&)
{
    return Vector3f::ZERO;
}
//----------------------------------------------------------------------------
