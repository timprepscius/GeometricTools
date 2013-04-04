// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "CpuNonlocalSolver1.h"
#include "FpuSupport.h"
#include "Wm5BitHacks.h"
#include "Wm5Memory.h"
using namespace Wm5;

extern void SaveGraph (const std::string&, int, float, int, const float*);

//----------------------------------------------------------------------------
CpuNonlocalSolver1::CpuNonlocalSolver1 (int dimension,
    const Image1<float>* initial, const Image1<unsigned char>* domain,
    float dt, float dx, float p, const std::string& folder, bool& success)
    :
    CpuPdeSolver1<float>(dimension, initial, domain, dt, dx, success),
    mPower(p),
    mNonlinear0(0.0f),
    mNonlinear1(0.0f),
    mReadBack(0),
    mFolder(folder)
{
    if (success)
    {
        mReadBack = mImage[0].GetPixels1D();
    }
}
//----------------------------------------------------------------------------
CpuNonlocalSolver1::~CpuNonlocalSolver1 ()
{
}
//----------------------------------------------------------------------------
void CpuNonlocalSolver1::GetIntegral (float& umax, float& integral)
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

    Image1<float> temp(mDimension);
    float* tcurrent = temp.GetPixels1D();
    ucurrent = mReadBack;
    for (j = 0; j < mNumTexels; ++j)
    {
        *tcurrent++ = exp(*ucurrent++ - umax);
    }

    // Use a mipmap approach to averaging.
    int logDim0 = Log2OfPowerOfTwo(mDimension);
    int i0, i0max;
    for (j = 0, i0max = mDimension/2; j < logDim0; ++j, i0max /= 2)
    {
        for (i0 = 0; i0 < i0max; ++i0)
        {
            temp(i0) = 0.5f*(temp(2*i0) + temp(2*i0+1));
        }
    }

    // Define m = max_{i0}[u(i0)] and f(i0) = exp(u(i0)-m); the
    // average is
    //   A = (1/(n0))*sum_{i0=0}^{n0-1} f(i0)
    float average = temp(0);

    // The integral is approximated by the trapezoidal rule.  Define
    // dx0 = 2/(n0-1); then
    //   I = int_{-1}^{1} exp(u(x)-m) dx
    //     = (dx0/2)*[f(0) + f(n0-1) + 2*sum_{i0=1}^{n0-2} f(i0)]
    //     = dx0*[sum_{i0=0}^{n0-1} f(i0) - exp(-m)]
    //     = k0*A - k1*exp(-m)
    // where
    //   k0 = 2*n0/(n0-1)
    //   k1 = 2/(n0-1)
    float k1 = 2.0f/(mDimension - 1.0f);
    float k0 = k1*mDimension;
    integral = pow(k0*average - k1*exp(-umax), mPower);
}
//----------------------------------------------------------------------------
float CpuNonlocalSolver1::Equation (int i0, const Image1<float>& u0,
    const Image1<float>&)
{
    return mNonlinear0*exp(u0(i0) - mNonlinear1);
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver1::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int frame = (int)iteration;
    SaveGraph(mFolder, frame, 100.0f, mDimension, mReadBack);

    float umax = mReadBack[mDimension/2];
    std::cout << "frame = " << frame << " : umax = " << umax << std::endl;
#else
    WM5_UNUSED(iteration);
#endif

    float u0max, integral;
    GetIntegral(u0max, integral);
    mNonlinear0 = mCoeff[2]/integral;
    mNonlinear1 = mPower*u0max;
    return true;
}
//----------------------------------------------------------------------------
bool CpuNonlocalSolver1::OnPostIteration (uint64_t)
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
