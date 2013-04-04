// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMOKE2D_H
#define SMOKE2D_H

#include "Wm5Fluid2Da.h"
#include "Wm5Fluid2Db.h"
using namespace Wm5;

// Set to "0" for Fluid2Da or to "1" for Fluids2Db.
#if 0
#define FLUIDBASE Fluid2Da<Real>
#else
#define FLUIDBASE Fluid2Db<Real>
#endif

template <typename Real>
class Smoke2D : public FLUIDBASE
{
public:
    // Construction and destruction.
    Smoke2D (Real x0, Real y0, Real x1, Real y1, Real dt, Real denViscosity,
        Real velViscosity, int imax, int jmax, int numGaussSeidelIterations,
        bool densityDirichlet, int numVortices);

    virtual ~Smoke2D ();

    virtual Real InitialDensity (Real x, Real y,
        int i, int j);

    virtual Vector2<Real> InitialVelocity (Real x, Real y,
        int i, int j);

    virtual Real SourceDensity (Real t, Real x, Real y,
        int i, int j);

    virtual Vector2<Real> SourceVelocity (Real t, Real x, Real y,
        int i, int j);

    // Member access.
    inline int GetNumVortices () const;
    inline Vector2<Real> GetVortexCenter (int i) const;
    inline Real GetVortexVariance (int i) const;
    inline Real GetVortexAmplitude (int i) const;

    // Dynamic adjustment of vortices.
    inline int GetNumActiveVortices () const;
    void SetNumActiveVortices (int numActive);

    // Dynamic adjustment of gravity.
    inline Real GetGravity () const;
    void SetGravity (Real gravity);

private:
    using FLUIDBASE::mIMax;
    using FLUIDBASE::mJMax;
    using FLUIDBASE::mIMaxP1;
    using FLUIDBASE::mJMaxP1;
    using FLUIDBASE::mX;
    using FLUIDBASE::mY;

    int mNumVortices;
    int mNumActive;
    Vector2<Real>* mVortexCenter;
    Real* mVortexVariance;
    Real* mVortexAmplitude;

    // Time-invariant portion of density source.
    Real** mTimelessDensity;

    // Time-invariant portion of velocity source.
    Vector2<Real>*** mTimelessVortex;
    Vector2<Real>** mTimelessWind;

    // Gravitational force in y-direction.
    Real mGravity;
};

#include "Smoke2D.inl"

#endif
