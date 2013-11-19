// Geometric Tools, LLC
// Copyright (c) 1998-2013
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
    mTime(0.0),
    mDeltaTime(0.0),
    mEccentricity(0.0),
    mRho(0.0),
    mMajorAxis(0.0),
    mMinorAxis(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mState[3] = 0.0;
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
void PhysicsModule::Initialize (double time, double deltaTime, double radius,
    double theta, double radiusDot, double thetaDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = theta;
    mState[1] = thetaDot;
    mState[2] = radius;
    mState[3] = radiusDot;

    // Compute c0 and c1 in the potential energy function V(theta).
    double gm = Gravity*Mass;
    double gm2 = gm*Mass;
    double radiusSqr = radius*radius;
    double alpha = Mass*radiusSqr*thetaDot;
    double g2m4da2 = gm2*gm2/(alpha*alpha);
    double v0 = -gm/radius;
    double dv0 = gm2*radiusDot/alpha;
    double v0Plus = v0 + g2m4da2;
    double sinTheta0 = Mathd::Sin(theta);
    double cosTheta0 = Mathd::Cos(theta);
    double c0 = v0Plus*sinTheta0 + dv0*cosTheta0;
    double c1 = v0Plus*cosTheta0 - dv0*sinTheta0;

    // Auxiliary variables needed by function DTheta(...).
    mAux[0] = c0;
    mAux[1] = c1;
    mAux[2] = g2m4da2;
    mAux[3] = alpha/(gm*gm2);

    // ellipse parameters
    double gamma0 = radiusSqr*Mathd::FAbs(thetaDot);
    double tmp0 = radiusSqr*radius*thetaDot*thetaDot - gm;
    double tmp1 = radiusSqr*radiusDot*thetaDot;
    double gamma1 = Mathd::Sqrt(tmp0*tmp0 + tmp1*tmp1);
    mEccentricity = gamma1/gm;
    mRho = gamma0*gamma0/gamma1;
    double tmp2 = 1.0 - mEccentricity*mEccentricity;
    assertion(tmp2 > 0.0, "Invalid eccentricity.\n");
    mMajorAxis = mRho*mEccentricity/tmp2;
    mMinorAxis = mMajorAxis*Mathd::Sqrt(tmp2);

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(1, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
double PhysicsModule::GetPeriod () const
{
    double powValue = Mathd::Pow(mMajorAxis, 1.5);
    double sqrtValue = Mathd::Sqrt(Gravity*Mass);
    return Mathd::TWO_PI*powValue/sqrtValue;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (mSolver)
    {
        // Apply a single step of the ODE solver.
        mSolver->Update(mTime, mState, mTime, mState);

        // Compute dot(theta) for application access.
        double sn = Mathd::Sin(mState[0]);
        double cs = Mathd::Cos(mState[0]);
        double v = mAux[0]*sn + mAux[1]*cs - mAux[2];
        mState[1] = mAux[3]*v*v;

        // Compute radius for application access.
        mState[2] = mEccentricity*mRho/(1.0 + mEccentricity*cs);

        // Compute dot(radius) for application access.
        mState[3] = mState[2]*mState[2]*mState[1]*sn/mRho;
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    double sn = Mathd::Sin(state[2]);
    double cs = Mathd::Cos(state[2]);
    double v = aux[0]*sn + aux[1]*cs - aux[2];
    double thetaDotFunction = aux[3]*v*v;

    // dot(theta) function
    output[0] = thetaDotFunction;
}
//----------------------------------------------------------------------------
