// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
template <typename Real>
CpuPdeSolver2<Real>::CpuPdeSolver2 (int dimension0, int dimension1,
    const Image2<Real>* initial, const Image2<unsigned char>* domain, Real dt,
    Real dx0, Real dx1, bool& success)
    :
    mDimension0(dimension0),
    mDimension1(dimension1),
    mNumTexels(dimension0*dimension1)
{
    // Default initialization of the remaining class members that are arrays.
    int i;
    for (i = 0; i < 4; ++i)
    {
        mCoeff[i] = (Real)0;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (mDimension0 <= 0 || mDimension1 <= 0 || dt <= (Real)0
    ||  dx0 <= (Real)0 || dx1 <= (Real)0)
    {
        return;
    }

    Real r0 = dt/(dx0*dx0), r1 = dt/(dx1*dx1);
    mCoeff[0] = ((Real)1)/((Real)1 + ((Real)2)*(r0 + r1));
    mCoeff[1] = mCoeff[0]*r0;
    mCoeff[2] = mCoeff[0]*r1;
    mCoeff[3] = mCoeff[0]*dt;

    if (initial)
    {
        for (i = 0; i < 3; ++i)
        {
            mImage[i] = *initial;
        }
    }
    else
    {
        for (i = 0; i < 3; ++i)
        {
            mImage[i].Resize(mDimension0, mDimension1);
            memset(mImage[i].GetPixels1D(), 0, mNumTexels*sizeof(Real));
        }
    }

    if (domain)
    {
        mMask = *domain;
    }
    else
    {
        mMask.Resize(mDimension0, mDimension1);
        memset(mMask.GetPixels1D(), 0xFF, mNumTexels*sizeof(unsigned char));
        for (int i0 = 0; i0 < mDimension0; ++i0)
        {
            mMask(i0, 0) = 0;
            mMask(i0, mDimension1 - 1) = 0;
        }
        for (int i1 = 0; i1 < mDimension1; ++i1)
        {
            mMask(0, i1) = 0;
            mMask(mDimension0 - 1, i1) = 0;
        }
    }

    success = true;
}
//----------------------------------------------------------------------------
template <typename Real>
CpuPdeSolver2<Real>::~CpuPdeSolver2 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver2<Real>::Enable ()
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver2<Real>::Disable ()
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver2<Real>::Execute (uint64_t iteration, int numGaussSeidel)
{
    if (iteration == 0)
    {
        mActive[0] = 0;
    }

    mActive[1] = 1 - mActive[0];
    mActive[2] = 2;

    if (!OnPreIteration(iteration))
    {
        return false;
    }

    for (int j = 0; j < numGaussSeidel; ++j)
    {
        Image2<Real>& u0 = mImage[mActive[0]];
        Image2<Real>& u1 = mImage[mActive[1]];
        Image2<Real>& u2 = mImage[mActive[2]];

        for (int i1 = 0; i1 < mDimension1; ++i1)
        {
            for (int i0 = 0; i0 < mDimension0; ++i0)
            {
                if (mMask(i0,i1) != 0)
                {
                    u2(i0,i1) =
                        mCoeff[0]*u0(i0,i1) +
                        mCoeff[1]*(u1(i0+1,i1) + u1(i0-1,i1)) +
                        mCoeff[2]*(u1(i0,i1+1) + u1(i0,i1-1)) +
                        Equation(i0, i1, u0, u1);
                }
                else
                {
                    u2(i0,i1) = 0.0f;
                }
            }
        }

        int save = mActive[1];
        mActive[1] = mActive[2];
        mActive[2] = save;
    }

    if (!OnPostIteration(iteration))
    {
        return false;
    }

    mActive[0] = 1 - mActive[0];
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
Real CpuPdeSolver2<Real>::Equation (int, int, const Image2<Real>&,
    const Image2<Real>&)
{
    // Stub for derived classes.
    return (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver2<Real>::OnPreIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver2<Real>::OnPostIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
