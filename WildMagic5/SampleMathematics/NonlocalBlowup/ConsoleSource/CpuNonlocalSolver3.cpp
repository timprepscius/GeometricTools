// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "CpuNonlocalSolver3.h"
#include "FpuSupport.h"
#include "Wm5BitHacks.h"
#include "Wm5Memory.h"
using namespace Wm5;

extern void SaveGraph (const std::string&, int, float, int, const float*);

//----------------------------------------------------------------------------
CpuNonlocalSolver3::CpuNonlocalSolver3 (int dimension0, int dimension1,
    int dimension2, const Image3<float>* initial,
    const Image3<unsigned char>* domain, float dt, float dx0, float dx1,
    float dx2, float p, const std::string& folder, bool& success)
    :
    CpuPdeSolver3<float>(dimension0, dimension1, dimension2, initial, domain,
        dt, dx0, dx1, dx2, success),
    mPower(p),
    mNonlinear0(0.0f),
    mNonlinear1(0.0f),
    mReadBack(0),
    mSlice(0),
    mFolder(folder)
{
    if (success)
    {
        mReadBack = mImage[0].GetPixels1D();
        mSlice = new1<float>(mDimension0);
        memset(mSlice, 0, mDimension0*sizeof(float));
    }
}
//----------------------------------------------------------------------------
CpuNonlocalSolver3::~CpuNonlocalSolver3 ()
{
    delete1(mSlice);
}
//----------------------------------------------------------------------------
void CpuNonlocalSolver3::GetIntegral (float& umax, float& integral)
{
    // Find the largest sample value.  To compute the integral robustly,
    // factor out the largest sample value.
    const float* ucurrent = mReadBack;
    umax = 0.0f;
    int j;
    for (j = 0; j < mNumTexels; ++j)
    {
        float u = *ucurrent++;
        if (u > umax)
        {
            umax = u;
        }
    }

    Image3<float> temp(mDimension0, mDimension1, mDimension2);
    float* tcurrent = temp.GetPixels1D();
    ucurrent = mReadBack;
    for (j = 0; j < mNumTexels; ++j)
    {
        *tcurrent++ = exp(*ucurrent++ - umax);
    }

    // Use a mipmap approach to averaging.
    int logDim0 = Log2OfPowerOfTwo(mDimension0);
    int logDim1 = Log2OfPowerOfTwo(mDimension1);
    int logDim2 = Log2OfPowerOfTwo(mDimension2);
    int i0, i1, i2, i0max, i1max, i2max;
    for (j = 0, i0max = mDimension0/2; j < logDim0; ++j, i0max /= 2)
    {
        for (i2 = 0; i2 < mDimension2; ++i2)
        {
            for (i1 = 0; i1 < mDimension1; ++i1)
            {
                for (i0 = 0; i0 < i0max; ++i0)
                {
                    temp(i0, i1, i2) = 0.5f*(temp(2*i0, i1, i2) +
                        temp(2*i0+1, i1, i2));
                }
            }
        }
    }
    for (j = 0, i1max = mDimension1/2; j < logDim1; ++j, i1max /= 2)
    {
        for (i2 = 0; i2 < mDimension2; ++i2)
        {
            for (i1 = 0; i1 < i1max; ++i1)
            {
                temp(0, i1, i2) = 0.5f*(temp(0, 2*i1, i2) +
                    temp(0, 2*i1+1, i2));
            }
        }
    }
    for (j = 0, i2max = mDimension2/2; j < logDim2; ++j, i2max /= 2)
    {
        for (i2 = 0; i2 < i2max; ++i2)
        {
            temp(0, 0, i2) = 0.5f*(temp(0, 0, 2*i2) +
                temp(0, 0, 2*i2+1));
        }
    }

    // Define m = max_{i0,i1}[u(i0,i1)] and f(i0,i1) = exp(u(i0,i1)-m); the
    // average is
    //   A = (1/(n0*n1))*sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1} f(i0,i1)
    float average = temp(0, 0, 0);

    // The integral is approximated by the trapezoidal rule.  Define
    // dx0 = 2/(n0-1), dx1 = 2/(n1-1), and dx2 = 2/(n2-2); then
    //   I = int_{-1}^{1} int_{-1}^{1} int_{-1}^{1} exp(u(x,y,z)-m) dx dy dz
    //     = (dx0*dx1*dx2/8)*[
    //       f(0,0,0) + f(n0-1,0,0) + f(0,n1-1,0) + f(n0-1,n1-1,0) +
    //       f(0,0,n2-1) + f(n0-1,0,n2-1) + f(0,n1-1,n2-1) +
    //         f(n0-1,n1-1,n2-1) +
    //       2*sum_{i0=1}^{n0-2} [f(i0,0,0) + f(i0,n1-1,0) + f(i0,0,n2-1) +
    //         f(i0,n1-1,n2-1)] +
    //       2*sum_{i1=1}^{n1-2} [f(0,i1,0) + f(0,i1,n2-1) + f(n0-1,i1,0) +
    //         f(n0-1,i1,n2-1)] +
    //       2*sum_{i2=1}^{n2-2} [f(0,0,i2) + f(0,n1-1,i2) + f(n0-1,0,i2) +
    //         f(n0-1,n1-1,i2)] +
    //       4*sum_{i0=1}^{n0-2} sum_{i1=1}^{n1-2} [f(i0,i1,0) +
    //         f(i0,i1,n2-1)] +
    //       4*sum_{i0=1}^{n0-2} sum_{i2=1}^{n2-2} [f(i0,0,i2) +
    //         f(i0,n1-1,i2)] +
    //       4*sum_{i1=1}^{n1-2} sum_{i2=1}^{n2-2} [f(0,i1,i2) +
    //         f(n0-1,i1,i2)] +
    //       8*sum_{i0=1}^{n0-2} sum_{i1=1}^{n1-2} sum_{i2=1}^{n2-2}
    //         f(i0,i1,i2)]
    //     = k0*A - k1*exp(-m)
    // where
    //   k0 = 8*n0*n1*n2/((n0-1)*(n1-1)*(n2-1))
    //   k1 = 8*(n0*n1+n0*n2+n1*n2-n0-n1-n2+1)/((n0-1)*(n1-1)*(n2-1))
    float dim0m1 = mDimension0 - 1.0f;
    float dim1m1 = mDimension1 - 1.0f;
    float dim2m1 = mDimension2 - 1.0f;
    float ratio = 8.0f/(dim0m1*dim1m1*dim2m1);
    float k0 = ratio*mDimension0*mDimension1*mDimension2;
    float k1 = ratio*(mDimension0*dim1m1 + mDimension1*dim2m1 +
        mDimension2*dim0m1 + 1.0f);
    integral = pow(k0*average - k1*exp(-umax), mPower);
}
//----------------------------------------------------------------------------
float CpuNonlocalSolver3::Equation (int i0, int i1, int i2,
    const Image3<float>& u0, const Image3<float>&)
{
    return mNonlinear0*exp(u0(i0,i1,i2) - mNonlinear1);
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver3::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int j0 = mDimension0*((mDimension1/2) + mDimension1*(mDimension2/2));
    for (int i0 = 0; i0 < mDimension0; ++i0, ++j0)
    {
        mSlice[i0] = mReadBack[j0];
    }

    int frame = (int)iteration;
    SaveGraph(mFolder, frame, 100.0f, mDimension0, mSlice);

    float umax = mSlice[mDimension0/2];
    std::cout << "frame = " << frame << " : umax = " << umax << std::endl;
#else
    WM5_UNUSED(iteration);
#endif

    float u0max, integral;
    GetIntegral(u0max, integral);
    mNonlinear0 = mCoeff1/integral;
    mNonlinear1 = mPower*u0max;
    return true;
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver3::OnPostIteration (uint64_t)
{
    mReadBack = mImage[mActive[1]].GetPixels1D();

    for (int i = 0; i < mNumTexels; ++i)
    {
        if (!IsFinite(mReadBack[i]))
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
