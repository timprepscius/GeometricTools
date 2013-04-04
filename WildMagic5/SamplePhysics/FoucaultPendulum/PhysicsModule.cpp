// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5OdeRungeKutta4.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    AngularSpeed(0.0),
    Latitude(0.0),
    GDivL(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mState[3] = 0.0;
    mAux[0] = 0.0;
    mAux[1] = 0.0;
    mAux[2] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double theta,
    double phi, double thetaDot, double phiDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = theta;
    mState[1] = thetaDot;
    mState[2] = phi;
    mState[3] = phiDot;

    // auxiliary variables
    mAux[0] = AngularSpeed*Mathd::Sin(Latitude);  // w*sin(lat)
    mAux[1] = AngularSpeed*Mathd::Cos(Latitude);  // w*cos(lat)
    mAux[2] = GDivL;

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(4, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
HMatrix PhysicsModule::GetOrientation () const
{
    float cosTheta = (float)Mathd::Cos(mState[0]);
    float sinTheta = (float)Mathd::Sin(mState[0]);
    float sinPhi = (float)Mathd::Sin(mState[2]);
    float cosPhi = (float)Mathd::Cos(mState[2]);
    float oneMinusCosPhi = 1.0f - cosPhi;

    HMatrix rot;
    rot[0][0] = 1.0f - oneMinusCosPhi*cosTheta*cosTheta;
    rot[0][1] = -oneMinusCosPhi*sinTheta*cosTheta;
    rot[0][2] = -sinPhi*cosTheta;
    rot[0][3] = 0.0f;
    rot[1][0] = rot[0][1];
    rot[1][1] = 1.0f - oneMinusCosPhi*sinTheta*sinTheta;
    rot[1][2] = -sinPhi*sinTheta;
    rot[1][3] = 0.0f;
    rot[2][0] = -rot[0][2];
    rot[2][1] = -rot[1][2];
    rot[2][2] = cosPhi;
    rot[2][3] = 0.0f;
    rot[3][0] = 0.0f;
    rot[3][1] = 0.0f;
    rot[3][2] = 0.0f;
    rot[3][3] = 1.0f;
    return rot;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    // Apply a single step to the ODE solver.
    if (mSolver)
    {
        mSolver->Update(mTime, mState, mTime, mState);
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state,
    void* data, double* output)
{
    double* aux = (double*)data;

    double sinTheta = Mathd::Sin(state[0]);
    double sinPhi = Mathd::Sin(state[2]);
    double cosPhi = Mathd::Cos(state[2]);

    // This function has a removable discontinuity at phi = 0.  When sin(phi)
    // is nearly zero, switch to the function that is defined at phi = 0. 
    double theta1DotFunction;
    if (Mathd::FAbs(sinPhi) < 1e-06)
    {
        theta1DotFunction = (2.0/3.0)*aux[1]*state[3]*sinTheta;
    }
    else
    {
        theta1DotFunction = -2.0*state[3]*(-aux[1]*sinTheta +
            cosPhi*(state[1]+aux[0])/sinPhi);
    }

    double theta2DotFunction = sinPhi*(state[1]*state[1]*cosPhi +
        2.0*state[1]*(aux[1]*sinTheta*sinPhi - aux[0]*cosPhi) - aux[2]);

    // theta function
    output[0] = state[1];

    // dot(theta) function
    output[1] = theta1DotFunction;

    // phi function
    output[2] = state[3];

    // dot(phi) function
    output[3] = theta2DotFunction;
}
//----------------------------------------------------------------------------
