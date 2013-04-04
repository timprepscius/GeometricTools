// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

class PhysicsModule
{
public:
    // Construction and destruction.
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the numerical solver.
    void Initialize (double time, double deltaTime, double y1, double dy1,
        double dy3);

    // Apply a single step of the solver.
    void Update ();

    // The gravitational constant.
    double Gravity;

    // The left mass in Figure 3.13.
    double Mass1;

    // The right mass in Figure 3.13.
    double Mass2;

    // The length of rigid wire connecting mass 1 to mass 2.
    double WireLength;

    // The pulley parameters.
    double Mass3, Radius, Inertia;

    // The spring parameters.
    double SpringLength, SpringConstant;

    // Member access.
    inline double GetInitialY1 () const;
    inline double GetCurrentY1 () const;
    inline double GetCurrentY2 () const;
    inline double GetCurrentY3 () const;
    inline double GetAngle () const;
    inline double GetCableFraction1 () const;
    inline double GetCableFraction2 () const;

private:
    // time information
    double mTime, mDeltaTime;

    // derived parameters
    double mAlpha, mBeta, mGamma, mDelta, mOmega, mGDivOmegaSqr;

    // initial conditions
    double mY1, mY2, mY3, mDY1, mDY2, mDY3;

    // solution parameters
    double mLPlusGDivOmegaSqr, mK1, mK2, mTCoeff, mTSqrCoeff;
    double mDeltaDivOmegaSqr, mY1Curr, mY2Curr, mY3Curr;
};

#include "PhysicsModule.inl"

#endif
