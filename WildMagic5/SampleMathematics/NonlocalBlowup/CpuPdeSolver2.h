// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef CPUPDESOLVER2_H
#define CPUPDESOLVER2_H

#include "Image2.h"

template <typename Real>
class CpuPdeSolver2
{
protected:
    // Abstract base class.  The return 'success' is 'true' iff the solver is
    // successfully created.
    CpuPdeSolver2 (int dimension0, int dimension1,
        const Image2<Real>* initial, const Image2<unsigned char>* domain,
        Real dt, Real dx0, Real dx1, bool& success);

public:
    virtual ~CpuPdeSolver2 ();

    // The functions Enable() and Disable() are stubs for derived classes.
    // Override these when additional behavior is required before and/or
    // after the Execute() call.
    virtual bool Enable ();
    virtual bool Disable ();

    // Compute one step of the solver.  This function must be called after
    // Enable() is called the first time.  The callback OnPreIteration(...) is
    // called before the solver step and the callback OnPostIteration(...) is
    // called after the solver step.  A derived class may override these if
    // additional behavior is needed before and after the solver step.  If the
    // input 'iteration' is zero, the solver assumes this is the initial step
    // and sets the flip-flop buffers accordingly.
    bool Execute (uint64_t iteration, int numGaussSeidel);

protected:
    // Overrides to be specialized by derived classes.
    virtual Real Equation (int i0, int i1, const Image2<Real>& u0,
        const Image2<Real>& u1);

    // Overrides to be specialized by derived classes.  Return 'true' to
    // continue the solver, 'false' to terminate the solver.
    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    int mDimension0, mDimension1, mNumTexels;
    Image2<Real> mImage[3];
    Image2<unsigned char> mMask;
    Real mCoeff[4];
    int mActive[3];
};

#include "CpuPdeSolver2.inl"

#endif
