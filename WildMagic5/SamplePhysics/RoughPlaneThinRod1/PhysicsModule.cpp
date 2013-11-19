// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Integrate1.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
#include "Wm5OdeRungeKutta4.h"

int PhysicsModule::msOrder = 16;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    Length(0.0),
    MassDensity(0.0),
    Friction(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mState[3] = 0.0;
    mState[4] = 0.0;
    mState[5] = 0.0;
    mAux[0] = 0.0;
    mAux[1] = 0.0;
    mAux[2] = 0.0;
    mAux[3] = 0.0;
    mAux[4] = 0.0;
    mAux[5] = 0.0;
    mAux[6] = 0.0;
    mAux[7] = 0.0;
    mAux[8] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double x,
    double y, double theta, double xDot, double yDot, double thetaDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = x;
    mState[1] = xDot;
    mState[2] = y;
    mState[3] = yDot;
    mState[4] = theta;
    mState[5] = thetaDot;

    // auxiliary variable
    double mu0 = MassDensity*Length;
    double mu2 = MassDensity*Length*Length*Length/12.0;
    mHalfLength = 0.5*Length;
    mAux[0] = mHalfLength;
    mAux[1] = -Friction/mu0;
    mAux[2] = -Friction/mu2;
    mAux[3] = mState[1];  // need dot(x) for integration
    mAux[4] = mState[3];  // need dot(y) for integration
    mAux[5] = mState[4];  // need theta for integration
    mAux[6] = mState[5];  // need dot(theta) for integration

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(6, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Get (double& x1, double& y1, double& x2, double& y2) const
{
    double cs = Mathd::Cos(mState[4]);
    double sn = Mathd::Sin(mState[4]);
    x1 = mState[0] + mHalfLength*cs;
    y1 = mState[2] + mHalfLength*sn;
    x2 = mState[0] - mHalfLength*cs;
    y2 = mState[2] - mHalfLength*sn;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (mSolver)
    {
        // Apply a single step of the ODE solver.
        mSolver->Update(mTime, mState, mTime, mState);

        mAux[3] = mState[1];
        mAux[4] = mState[3];
        mAux[5] = mState[4];
        mAux[6] = mState[5];
    }
}
//----------------------------------------------------------------------------
double PhysicsModule::FXIntegrand (double ell, void* data)
{
    // Compute all integrands here.  Return one value, store the others for
    // later access.
    double* aux = (double*)data;

    double cs = Mathd::Cos(aux[2]);
    double sn = Mathd::Sin(aux[2]);

    double tmp1 = aux[3] - ell*aux[6]*sn;
    double tmp2 = aux[4] + ell*aux[6]*cs;
    double length = Mathd::Sqrt(tmp1*tmp1 + tmp2*tmp2);
    if (length > Mathd::ZERO_TOLERANCE)
    {
        double invLength = 1.0/length;

        // FY integrand
        aux[7] = aux[1]*tmp2*invLength;

        // FTheta integrand
        double tmp3 = ell*(ell*aux[6] - aux[3]*sn + aux[4]*cs);
        aux[8] = aux[2]*tmp3*invLength;

        // FX integrand
        return aux[1]*tmp1*invLength;
    }

    // FY integrand
    aux[7] = 0.0;

    // FTheta integrand
    aux[8] = 0.0;

    // FX integrand
    return 0.0;
}
//----------------------------------------------------------------------------
double PhysicsModule::FYIntegrand (double, void* data)
{
    double* aux = (double*)data;
    return aux[7];
}
//----------------------------------------------------------------------------
double PhysicsModule::FThetaIntegrand (double, void* data)
{
    double* aux = (double*)data;
    return aux[8];
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    // x function
    output[0] = state[1];

    // dot(x) function
    output[1] = Integrate1d::RombergIntegral(msOrder, -aux[4], aux[4],
        FXIntegrand, aux);

    // y function
    output[2] = state[3];

    // dot(y) function
    output[3] = Integrate1d::RombergIntegral(msOrder, -aux[4], aux[4],
        FYIntegrand, aux);

    // theta function
    output[4] = state[5];

    // dot(theta) function
    output[5] = Integrate1d::RombergIntegral(msOrder, -aux[4], aux[4],
        FThetaIntegrand, aux);
}
//----------------------------------------------------------------------------
