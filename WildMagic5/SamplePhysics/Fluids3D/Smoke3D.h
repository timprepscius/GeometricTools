// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMOKE3D_H
#define SMOKE3D_H

#include "Wm5Fluid3Da.h"
#include "Wm5Fluid3Db.h"
using namespace Wm5;

// Set to "0" for Fluid3Da or to "1" for Fluids3Db.
#if 0
#define FLUIDBASE Fluid3Da<Real>
#else
#define FLUIDBASE Fluid3Db<Real>
#endif

template <typename Real>
class Smoke3D : public FLUIDBASE
{
public:
    // Construction and destruction.
    Smoke3D (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1, Real dt,
        Real denViscosity, Real velViscosity, int imax, int jmax, int kmax,
        int numGaussSeidelIterations, bool densityDirichlet, int numVortices);

    virtual ~Smoke3D ();

    virtual Real InitialDensity (Real x, Real y, Real z,
        int i, int j, int k);

    virtual Vector3<Real> InitialVelocity (Real x, Real y, Real z,
        int i, int j, int k);

    virtual Real SourceDensity (Real t, Real x, Real y, Real z,
        int i, int j, int k);

    virtual Vector3<Real> SourceVelocity (Real t, Real x, Real y, Real z,
        int i, int j, int k);

    // Member access.
    inline int GetNumVortices () const;
    inline Vector3<Real> GetVortexCenter (int i) const;
    inline Vector3<Real> GetVortexNormal (int i) const;
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
    using FLUIDBASE::mKMax;
    using FLUIDBASE::mIMaxP1;
    using FLUIDBASE::mJMaxP1;
    using FLUIDBASE::mKMaxP1;
    using FLUIDBASE::mX0;
    using FLUIDBASE::mY0;
    using FLUIDBASE::mZ0;
    using FLUIDBASE::mX1;
    using FLUIDBASE::mY1;
    using FLUIDBASE::mZ1;
    using FLUIDBASE::mX;
    using FLUIDBASE::mY;
    using FLUIDBASE::mZ;

    int mNumVortices;
    int mNumActive;
    Vector3<Real>* mVortexCenter;
    Vector3<Real>* mVortexNormal;
    Real* mVortexVariance;
    Real* mVortexAmplitude;

    // Time-invariant portion of velocity source.
    Vector3<Real>**** mTimelessVortex;
    Vector3<Real>*** mTimelessWind;

    // Gravitational force in z-direction.
    Real mGravity;
};

#include "Smoke3D.inl"

#endif
