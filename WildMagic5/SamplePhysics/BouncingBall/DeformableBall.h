// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef DEFORMABLEBALL_H
#define DEFORMABLEBALL_H

#include "Wm5TriMesh.h"
using namespace Wm5;

class DeformableBall
{
public:
    // Construction and destruction.
    DeformableBall (float dration, float period);
    ~DeformableBall ();

    // Member access.
    void Set (float duration, float period);
    inline TriMesh* GetMesh () const;
    inline float GetDuration () const;
    inline float GetPeriod () const;
    inline float GetMinActive () const;
    inline float GetMaxActive () const;
    inline float GetAmplitude (float time);
    inline bool& DoAffine ();

    // Deform the ball.
    bool DoSimulationStep (float realTime);

private:
    void CreateBall ();

    // Support for the mesh smoother of mMesh.
    void CreateSmoother ();
    void Update (float time);

    // Influence function:  parameters (X,t)
    //   input:  X = point on surface
    //           t = current time
    // The return value is 'true' if and only if the point is within the
    // region of influence of the deformation *and* if the deformation
    // function is active at time t.  This allows the mesh smoother to avoid
    // evolving the surface in regions where no deformation is occuring.
    bool VertexInfluenced (int i, float time, const AVector& position);

    float GetTangentWeight (int i, float time, const AVector& position);
    float GetNormalWeight (int i, float time, const AVector& position);

    // Level surface function:  parameters (X,t,F,Grad(F))
    //   input:  X = point in space
    //           t = time of deformation
    //   output: F(X,t) = scalar function at position and time
    //           Grad(F)(X,t) = gradient at level surface through X at time
    void ComputeFunction (const AVector& position, float time,
        float& function, AVector& gradient);

    TriMeshPtr mMesh;
    float mDuration, mPeriod, mDeformMult;
    float mMinActive, mMaxActive, mInvActiveRange;
    bool mDeforming, mDoAffine;

    // Parameters for Newton's method in ComputeFunction.
    int mMaxIterations;
    float mErrorTolerance;

    // Mesh smoother data.
    AVector* mNormal;
    AVector* mMean;
    int* mNeighborCount;
};

#include "DeformableBall.inl"

#endif
