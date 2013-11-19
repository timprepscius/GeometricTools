// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "CpuLocalSolver3.h"
#include "FpuSupport.h"
#include "Wm5Memory.h"
using namespace Wm5;

extern void SaveGraph (const std::string&, int, float, int, const float*);

//----------------------------------------------------------------------------
CpuLocalSolver3::CpuLocalSolver3 (int dimension0, int dimension1,
    int dimension2, const Image3<float>* initial,
    const Image3<unsigned char>* domain, float dt, float dx0, float dx1,
    float dx2, const std::string& folder, bool& success)
    :
    CpuPdeSolver3<float>(dimension0, dimension1, dimension2, initial, domain,
        dt, dx0, dx1, dx2, success),
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
CpuLocalSolver3::~CpuLocalSolver3 ()
{
    delete1(mSlice);
}
//----------------------------------------------------------------------------
float CpuLocalSolver3::Equation (int i0, int i1, int i2,
    const Image3<float>& u0, const Image3<float>&)
{
    return mCoeff1*exp(u0(i0,i1,i2));
}
//----------------------------------------------------------------------------
bool CpuLocalSolver3::OnPreIteration (uint64_t iteration)
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
    return true;
}
//----------------------------------------------------------------------------
bool CpuLocalSolver3::OnPostIteration (uint64_t)
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
