// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef CUBICINTERPOLATOR_H
#define CUBICINTERPOLATOR_H

#include "Wm5Memory.h"
#include "Wm5Tuple.h"
using namespace Wm5;

template <int N, typename Real>
class CubicInterpolator
{
public:
    // Construction and destruction.  A bool input is set to 'true' whenever
    // the corresponding array was dynamically allocated and you want the
    // class to own the array and deallocate it during class destruction.
    // The inputs must be a strictly increasing sequence of numbers.
    CubicInterpolator (int numSamples, Real* inputs, Tuple<N,Real>* outputs,
        Tuple<N,Real>* tangents0, Tuple<N,Real>* tangents1, bool ownerInputs,
        bool ownerOutputs, bool ownerTangents0, bool ownerTangents1);

    CubicInterpolator (const std::string& filename);

    ~CubicInterpolator ();

    // Member access.
    inline int GetNumSamples () const;
    inline const Real* GetInputs () const;
    inline const Real* GetOutputs () const;
    inline const Real* GetTangents0 () const;
    inline const Real* GetTangents1 () const;
    inline const Real GetMinInput () const;
    inline const Real GetMaxInput () const;

    // Evaluate the interpolator.  The input is clamped to [min,max], where
    // min = inputs[0] and max = inputs[numSamples-1].
    Tuple<N,Real> operator() (Real input) const;

private:
    // Support for constructors.
    void Initialize ();

    // Lookup on bounding keys.
    void GetKeyInfo (Real input, Real& normInput, int& key) const;

    // Constructor inputs.
    int mNumSamples;
    Real* mInputs;
    Tuple<N,Real>* mOutputs;
    Tuple<N,Real>* mTangents0;
    Tuple<N,Real>* mTangents1;
    bool mOwnerInputs, mOwnerOutputs, mOwnerTangents0, mOwnerTangents1;

    // Support for key lookup and evaluation.
    int mNumSamplesM1;
    Real* mInvDeltas;
    Tuple<N,Real>* mC0;
    Tuple<N,Real>* mC1;
    Tuple<N,Real>* mC2;
    Tuple<N,Real>* mC3;

    // For O(1) lookup on bounding keys.
    mutable int mLastIndex;
};

#include "CubicInterpolator.inl"

#endif
