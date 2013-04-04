// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef CPUPDESOLVER1_H
#define CPUPDESOLVER1_H

#include "Image1.h"

template <typename Real>
class CpuPdeSolver1
{
protected:
    // Abstract base class.  The return 'success' is 'true' iff the solver is
    // successfully created.
    CpuPdeSolver1 (int dimension, const Image1<Real>* initial,
        const Image1<unsigned char>* domain, Real dt, Real dx,
        bool& success);

public:
    virtual ~CpuPdeSolver1 ();

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
    virtual Real Equation (int i0, const Image1<Real>& u0,
        const Image1<Real>& u1);

    // Overrides to be specialized by derived classes.  Return 'true' to
    // continue the solver, 'false' to terminate the solver.
    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    int mDimension, mNumTexels;
    Image1<Real> mImage[3];
    Image1<unsigned char> mMask;
    Real mCoeff[3];
    int mActive[3];
};

#include "CpuPdeSolver1.inl"

#endif
