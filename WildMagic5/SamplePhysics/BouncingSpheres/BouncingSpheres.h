// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BOUNCINGSPHERES_H
#define BOUNCINGSPHERES_H

#include "Wm5WindowApplication3.h"
#include "RigidBall.h"
using namespace Wm5;

class BouncingSpheres : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BouncingSpheres ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    class Contact
    {
    public:
        RigidBodyf* A;  // ball containing face
        RigidBodyf* B;  // ball containing vertex
        Vector3f P;     // contact point
        Vector3f N;     // outward unit-length normal to face
    };

    void CreateScene ();
    void CreateBalls ();
    void CreateFloor ();
    void CreateBackWall ();
    void CreateSideWall1 ();
    void CreateSideWall2 ();

    void PhysicsTick ();
    void GraphicsTick ();

    void DoCollisionDetection ();
    void DoCollisionResponse ();
    void ComputePreimpulseVelocity (float* preRelVelocities);
    void ComputeImpulseMagnitude (float* preRelVelocities,
        float* impulseMagnitudes);
    void SetBoundaryContact (int i, int bIndex, const Vector3f& position,
        float radius, Contact& contact);

    // External forces and torques for this application.
    static Vector3f Force (float time, float mass, const Vector3f& position,
        const Quaternionf& quatOrient, const Vector3f& linearMomentum,
        const Vector3f& angularMomentum, const Matrix3f& rotOrient,
        const Vector3f& linearVelocity, const Vector3f& angularVelocity);

    static Vector3f Torque (float time, float mass, const Vector3f& position,
        const Quaternionf& quatOrient, const Vector3f& linearMomentum,
        const Vector3f& angularMomentum, const Matrix3f& rotOrient,
        const Vector3f& linearVelocity, const Vector3f& angularVelocity);

    // Rigid spheres are the rigid bodies.
    enum { NUM_BALLS = 16 };
    RigidBall* mBalls[NUM_BALLS];
 
    // Representation of boundaries (floor, ceiling, walls).
    RigidBodyf mBoundaries[6];
    Vector3f mBoundaryLocations[6];
    Vector3f mBoundaryNormals[6];

    // Contact points during one pass of the physical simulation.
    int mNumContacts;
    std::vector<Contact> mBoundaryContacts;

    // Blocked directions.
    std::vector<Vector3f> mBlocked[NUM_BALLS];

    // Simulated clock.
    float mSimTime, mSimDeltaTime;

    // The scene graph.
    NodePtr mScene;
    WireStatePtr mWireState;
    NodePtr mBallNodes[NUM_BALLS];
    TriMeshPtr mFloor, mBackWall, mSideWall1, mSideWall2;
    PolysegmentPtr mSegment;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BouncingSpheres);
WM5_REGISTER_TERMINATE(BouncingSpheres);

#endif
