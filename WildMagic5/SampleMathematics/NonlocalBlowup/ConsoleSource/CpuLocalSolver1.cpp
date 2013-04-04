// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "CpuLocalSolver1.h"
#include "FpuSupport.h"
#include "Wm5Memory.h"
using namespace Wm5;

extern void SaveGraph (const std::string&, int, float, int, const float*);

//----------------------------------------------------------------------------
CpuLocalSolver1::CpuLocalSolver1 (int dimension,
    const Image1<float>* initial, const Image1<unsigned char>* domain,
    float dt, float dx, const std::string& folder, bool& success)
    :
    CpuPdeSolver1<float>(dimension, initial, domain, dt, dx, success),
    mReadBack(0),
    mFolder(folder)
{
    if (success)
    {
        mReadBack = mImage[0].GetPixels1D();
    }
}
//----------------------------------------------------------------------------
CpuLocalSolver1::~CpuLocalSolver1 ()
{
}
//----------------------------------------------------------------------------
float CpuLocalSolver1::Equation (int i0, const Image1<float>& u0,
    const Image1<float>&)
{
    return mCoeff[2]*exp(u0(i0));
}
//----------------------------------------------------------------------------
bool CpuLocalSolver1::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int frame = (int)iteration;
    SaveGraph(mFolder, frame, 100.0f, mDimension, mReadBack);

    float umax = mReadBack[mDimension/2];
    std::cout << "frame = " << frame << " : umax = " << umax << std::endl;
#else
    WM5_UNUSED(iteration);
#endif
    return true;
}
//----------------------------------------------------------------------------
bool CpuLocalSolver1::OnPostIteration (uint64_t)
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
