// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
#include "Wm5OdeRungeKutta4.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    Gravity(0.0),
    Mass(0.0),
    Length(0.0),
    Inertia1(0.0),
    Inertia3(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mAux[0] = 0.0;
    mAux[1] = 0.0;
    mAux[2] = 0.0;
    mAux[3] = 0.0;
    mAux[4] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double theta,
    double phi, double psi, double angVel1, double angVel2, double angVel3)
{
    mTime = time;
    mDeltaTime = deltaTime;

    double cosPhi = Mathd::Cos(phi), sinPhi = Mathd::Sin(phi);
    double cosPsi = Mathd::Cos(psi), sinPsi = Mathd::Sin(psi);

    // state variables
    mState[0] = theta;
    mState[1] = phi;
    mState[2] = psi;

    // auxiliary variables
    mAux[0] = Mass*Gravity*Length/Inertia1;  // alpha
    mAux[1] = angVel1*angVel1 + angVel2*angVel2 + 2.0*cosPhi*mAux[0];  // beta
    mAux[2] = angVel3*Inertia3/Inertia1;  // epsilon
    mAux[3] = sinPhi*(angVel1*sinPsi + angVel2*cosPsi) +
        cosPhi*mAux[2];  // delta
    mAux[4] = angVel3;

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(3, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    // Apply a single step of the ODE solver.
    if (mSolver)
    {
        mSolver->Update(mTime, mState, mTime, mState);
    }
}
//----------------------------------------------------------------------------
HMatrix PhysicsModule::GetBodyAxes () const
{
    float cosTheta = (float)Mathd::Cos(mState[0]);
    float sinTheta = (float)Mathd::Sin(mState[0]);
    float cosPhi = (float)Mathd::Cos(mState[1]);
    float sinPhi = (float)Mathd::Sin(mState[1]);
    float cosPsi = (float)Mathd::Cos(mState[2]);
    float sinPsi = (float)Mathd::Sin(mState[2]);

    AVector N(cosTheta, sinTheta, 0.0f);
    AVector axis3(sinTheta*sinPhi, -cosTheta*sinPhi, cosPhi);
    AVector axis3xN = axis3.Cross(N);
    AVector axis1 = cosPsi*N + sinPsi*axis3xN;
    AVector axis2 = cosPsi*axis3xN - sinPsi*N;

    return HMatrix(axis1, axis2, axis3, APoint::ORIGIN, true);
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    double cs = Mathd::Cos(state[1]);
    double invSin = 1.0/Mathd::Sin(state[1]);
    double numer = aux[3] - aux[2]*cs;
    double fraction = numer*invSin;
    double arg = aux[1] - 2.0*aux[0]*cs - fraction*fraction;
    double thetaDotFunction = fraction*invSin;
    double phiDotFunction = Mathd::Sqrt(Mathd::FAbs(arg));
    double psiDotFunction = aux[4] - cs*thetaDotFunction;

    // dot(theta) function
    output[0] = thetaDotFunction;

    // dot(phi) function
    output[1] = phiDotFunction;

    // dot(psi) function
    output[2] = psiDotFunction;
}
//----------------------------------------------------------------------------
