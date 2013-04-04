// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "CpuNonlocalSolver2.h"
#include "FpuSupport.h"
#include "Wm5BitHacks.h"
#include "Wm5Memory.h"
using namespace Wm5;

extern void SaveGraph (const std::string&, int, float, int, const float*);

//----------------------------------------------------------------------------
CpuNonlocalSolver2::CpuNonlocalSolver2 (int dimension0, int dimension1,
    const Image2<float>* initial, const Image2<unsigned char>* domain,
    float dt, float dx0, float dx1, float p, const std::string& folder,
    bool& success)
    :
    CpuPdeSolver2<float>(dimension0, dimension1, initial, domain, dt, dx0,
        dx1, success),
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
CpuNonlocalSolver2::~CpuNonlocalSolver2 ()
{
    delete1(mSlice);
}
//----------------------------------------------------------------------------
void CpuNonlocalSolver2::GetIntegral (float& umax, float& integral)
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

    Image2<float> temp(mDimension0, mDimension1);
    float* tcurrent = temp.GetPixels1D();
    ucurrent = mReadBack;
    for (j = 0; j < mNumTexels; ++j)
    {
        *tcurrent++ = exp(*ucurrent++ - umax);
    }

    // Use a mipmap approach to averaging.
    int logDim0 = Log2OfPowerOfTwo(mDimension0);
    int logDim1 = Log2OfPowerOfTwo(mDimension1);
    int i0, i1, i0max, i1max;
    for (j = 0, i0max = mDimension0/2; j < logDim0; ++j, i0max /= 2)
    {
        for (i1 = 0; i1 < mDimension1; ++i1)
        {
            for (i0 = 0; i0 < i0max; ++i0)
            {
                temp(i0, i1) = 0.5f*(temp(2*i0, i1) + temp(2*i0+1, i1));
            }
        }
    }
    for (j = 0, i1max = mDimension1/2; j < logDim1; ++j, i1max /= 2)
    {
        for (i1 = 0; i1 < i1max; ++i1)
        {
            temp(0, i1) = 0.5f*(temp(0, 2*i1) + temp(0, 2*i1+1));
        }
    }

    // Define m = max_{i0,i1}[u(i0,i1)] and f(i0,i1) = exp(u(i0,i1)-m); the
    // average is
    //   A = (1/(n0*n1))*sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1} f(i0,i1)
    float average = temp(0, 0);

    // The integral is approximated by the trapezoidal rule.  Define
    // dx0 = 2/(n0-1) and dx1 = 2/(n1-1); then
    //   I = int_{-1}^{1} int_{-1}{1} exp(u(x,y)-m) dx dy
    //     = (dx0*dx1/4)*[
    //       f(0,0) + f(n0-1,0) + f(0,n1-1) + f(n0-1,n1-1) +
    //       2*sum_{i0=1}^{n0-2} [f(i0,0) + f(i0,n1-1)] +
    //       2*sum_{i1=1}^{n1-2} [f(0,i1) + f(n0-1,i1)] +
    //       4*sum_{i0=1}^{n0-2} sum_{i1=1}^{n1-2} f(i0,i1)]
    //     = dx0*dx1*[
    //       (5-n0-n1)*exp(-m) + 
    //       sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1} f(i0,i1)]
    //     = k0*A - k1*exp(-m)
    // where
    //   k0 = 4*n0*n1/((n0-1)*(n1-1))
    //   k1 = 4*(n0+n1-5)/((n0-1)(n1-1))
    float ratio = 4.0f/((mDimension0 - 1.0f)*(mDimension1 - 1.0f));
    float k0 = ratio*mDimension0*mDimension1;
    float k1 = ratio*(mDimension0 + mDimension1 - 5.0f);
    integral = pow(k0*average - k1*exp(-umax), mPower);
}
//----------------------------------------------------------------------------
float CpuNonlocalSolver2::Equation (int i0, int i1, const Image2<float>& u0,
    const Image2<float>&)
{
    return mNonlinear0*exp(u0(i0,i1) - mNonlinear1);
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver2::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int j0 = mDimension0*(mDimension1/2);
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
    mNonlinear0 = mCoeff[3]/integral;
    mNonlinear1 = mPower*u0max;
    return true;
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver2::OnPostIteration (uint64_t)
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
