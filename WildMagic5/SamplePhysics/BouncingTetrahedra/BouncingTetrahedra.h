// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BOUNCINGTETRAHEDRA_H
#define BOUNCINGTETRAHEDRA_H

#include "Wm5WindowApplication3.h"
#include "RigidTetra.h"
using namespace Wm5;

class BouncingTetrahedra : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BouncingTetrahedra ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    struct Contact
    {
        RigidBodyf* A;     // tetrahedron containing face
        RigidBodyf* B;     // tetrahedron containing vertex
        Vector3f PA;       // contact point for tetrahedron A
        Vector3f PB;       // contact point for tetrahedron B
        Vector3f N;        // outward unit-length normal to face
        Vector3f EA;       // edge from A
        Vector3f EB;       // edge from B
        bool IsVFContact;  // true if vertex-face, false if edge-edge
    };

    void CreateScene ();
    void CreateTetra ();
    void CreateFloor ();
    void CreateBackWall ();
    void CreateSideWall1 ();
    void CreateSideWall2 ();

    void PhysicsTick ();
    void GraphicsTick ();

    // Collision detection functions.
    void DoCollisionDetection ();
    bool FarFromBoundary (int i);
    bool FarApart (int t0, int t1);
    bool TetraBoundaryIntersection (int tetra, int boundary, float* distances,
        Contact& contact);
    void BuildContactMoveTetra (int tetra, int boundary, int hitIndex, 
        float depthMax, Contact& contact);
    void Reposition (int t0, int t1, Contact& contact);
    bool IsVertex (const Vector3f* vertices, const Vector3f& closest);
    void CalculateNormal (const Vector3f* vertices, const Vector3f& closest,
        Contact& contact);
    Vector3f ClosestEdge (const Vector3f* vertices, const Vector3f& closest,
        Vector3f& otherVertex);

    // Collision response functions.
    void DoCollisionResponse ();
    void ComputePreimpulseVelocity (float* preRelVelocities);
    void ComputeImpulseMagnitude (float* preRelVelocities,
        float* impulseMagnitudes);
    void DoImpulse (float* impulseMagnitudes);

    // External forces and torques for this application.
    static Vector3f Force (float time, float mass, const Vector3f& position,
        const Quaternionf& quatOrient, const Vector3f& linearMomentum,
        const Vector3f& angularMomentum, const Matrix3f& rotOrient,
        const Vector3f& linearVelocity, const Vector3f& angularVelocity);

    static Vector3f Torque (float time, float mass, const Vector3f& position,
        const Quaternionf& quatOrient, const Vector3f& linearMomentum,
        const Vector3f& angularMomentum, const Matrix3f& rotOrient,
        const Vector3f& linearVelocity, const Vector3f& angularVelocity);

    // Rigid tetrahedra are the rigid bodies.
    enum { NUM_TETRA = 4 };
    RigidTetra* mTetras[NUM_TETRA];

    // Representation of boundaries (floor, ceiling, walls).
    RigidBodyf mBoundaries[6];
    Vector3f mBoundaryLocations[6];
    Vector3f mBoundaryNormals[6];

    // Contact points during one pass of the physical simulation.
    int mNumContacts;
    std::vector<Contact> mContacts;

    // Simulated clock.
    float mSimTime, mSimDeltaTime;

    // Number of times the LCP solver was called during a simulation step.
    int mLCPCount;

    // The connectivity of a tetrahedron, used by the LCP solver.
    Tuple<3,int> mFaces[4];

    // The error tolerance used for interpenetration calculations.
    float mTolerance;

    // The kinetic energy of the system.
    float mTotalKE;

    // The scene graph.
    NodePtr mScene;
    WireStatePtr mWireState;
    NodePtr mTetraNodes[NUM_TETRA];
    TriMeshPtr mFloor, mBackWall, mSideWall1, mSideWall2;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BouncingTetrahedra);
WM5_REGISTER_TERMINATE(BouncingTetrahedra);

#endif
