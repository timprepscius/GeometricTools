// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
template <typename Real>
CpuPdeSolver1<Real>::CpuPdeSolver1 (int dimension,
    const Image1<Real>* initial, const Image1<unsigned char>* domain, Real dt,
    Real dx, bool& success)
    :
    mDimension(dimension),
    mNumTexels(dimension)
{
    // Default initialization of the remaining class members that are arrays.
    int i;
    for (i = 0; i < 3; ++i)
    {
        mCoeff[i] = (Real)0;
        mActive[i] = 0;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (mDimension <= 0 || dt <= (Real)0 || dx <= (Real)0)
    {
        return;
    }

    Real r = dt/(dx*dx);
    mCoeff[0] = ((Real)1)/((Real)1 + ((Real)2)*r);
    mCoeff[1] = mCoeff[0]*r;
    mCoeff[2] = mCoeff[0]*dt;

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
            mImage[i].Resize(mDimension);
            memset(mImage[i].GetPixels1D(), 0, mNumTexels*sizeof(Real));
        }
    }

    if (domain)
    {
        mMask = *domain;
    }
    else
    {
        mMask.Resize(mDimension);
        memset(mMask.GetPixels1D(), 0xFF, mNumTexels*sizeof(unsigned char));
        mMask(0) = 0;
        mMask(mDimension - 1) = 0;
    }

    success = true;
}
//----------------------------------------------------------------------------
template <typename Real>
CpuPdeSolver1<Real>::~CpuPdeSolver1 ()
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver1<Real>::Enable ()
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver1<Real>::Disable ()
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver1<Real>::Execute (uint64_t iteration, int numGaussSeidel)
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
        Image1<Real>& u0 = mImage[mActive[0]];
        Image1<Real>& u1 = mImage[mActive[1]];
        Image1<Real>& u2 = mImage[mActive[2]];

        for (int i0 = 0; i0 < mDimension; ++i0)
        {
            if (mMask(i0) != 0)
            {
                u2(i0) =
                    mCoeff[0]*u0(i0) +
                    mCoeff[1]*(u1(i0+1) + u1(i0-1)) +
                    Equation(i0, u0, u1);
            }
            else
            {
                u2(i0) = 0.0f;
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
Real CpuPdeSolver1<Real>::Equation (int, const Image1<Real>&,
    const Image1<Real>&)
{
    // Stub for derived classes.
    return (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver1<Real>::OnPreIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool CpuPdeSolver1<Real>::OnPostIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
