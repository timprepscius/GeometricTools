// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Memory.h"
#include "Wm5OdeRungeKutta4.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    Gravity(0.0),
    A1(0.0),
    A2(0.0),
    A3(0.0),
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
void PhysicsModule::Initialize (double time, double deltaTime, double y1,
    double y2, double y1Dot, double y2Dot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = y1;     // y1(0)
    mState[1] = y1Dot;  // y1'(0)
    mState[2] = y2;     // y2(0)
    mState[3] = y2Dot;  // y2'(0)

    // auxiliary variables
    mAux[0] = A1*A1;   // a1^2
    mAux[1] = A2*A2;   // a2^2
    mAux[2] = Gravity; // g

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(4, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::GetData (APoint& center, HMatrix& incrRot) const
{
    // Position is a point exactly on the hill.
    APoint position;
    position[0] = (float)(A1*mState[0]);
    position[1] = (float)(A2*mState[2]);
    position[2] = (float)(A3 - mState[0]*mState[0] - mState[2]*mState[2]);

    // Lift this point off the hill in the normal direction by the radius of
    // the ball so that the ball just touches the hill.  The hill is
    // implicitly specified by F(x,y,z) = z - [a3 - (x/a1)^2 - (y/a2)^2]
    // where (x,y,z) is the position on the hill.  The gradient of F is a
    // normal vector, Grad(F) = (2*x/a1^2,2*y/a2^2,1).
    AVector normal;
    normal[0] = 2.0f*position[0]/(float)mAux[0];
    normal[1] = 2.0f*position[1]/(float)mAux[1];
    normal[2] = 1.0f;
    normal.Normalize();

    center = position + ((float)Radius)*normal;

    // Let the ball rotate as it rolls down hill.  The axis of rotation is
    // the perpendicular to hill normal and ball velocity.  The angle of
    // rotation from the last position is A = speed*deltaTime/radius.
    AVector velocity;
    velocity[0] = (float)(A1*mState[1]);
    velocity[1] = (float)(A1*mState[3]);
    velocity[2] = -2.0f*(velocity[0]*(float)mState[0] +
        velocity[1]*(float)mState[2]);

    float speed = velocity.Normalize();
    float angle = speed*((float)mDeltaTime)/((float)Radius);
    AVector axis = normal.UnitCross(velocity);
    incrRot = HMatrix(axis, angle);
}
//----------------------------------------------------------------------------
float PhysicsModule::GetHeight (float x, float y) const
{
    double xScaled = ((double)x)/A1;
    double yScaled = ((double)y)/A2;
    return (float)(A3 - xScaled*xScaled - yScaled*yScaled);
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
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    double mat00 = aux[0] + 4.0*state[0]*state[0];
    double mat01 = 4.0*state[0]*state[2];
    double mat11 = aux[1] + 4.0*state[2]*state[2];
    double invDet = 1.0/(mat00*mat11 - mat01*mat01);
    double sqrLen = state[1]*state[1] + state[3]*state[3];
    double rhs0 = 2.0*state[0]*(aux[2] - 2.0*state[0]*sqrLen);
    double rhs1 = 2.0*state[2]*(aux[2] - 2.0*state[2]*sqrLen);
    double y1DotFunction = (mat11*rhs0 - mat01*rhs1)*invDet;
    double y2DotFunction = (mat00*rhs1 - mat01*rhs0)*invDet;

    // Y1 function
    output[0] = state[1];

    // dot(Y1) function
    output[1] = y1DotFunction;

    // Y2 function
    output[2] = state[3];

    // dot(Y2) function
    output[3] = y2DotFunction;
}
//----------------------------------------------------------------------------
