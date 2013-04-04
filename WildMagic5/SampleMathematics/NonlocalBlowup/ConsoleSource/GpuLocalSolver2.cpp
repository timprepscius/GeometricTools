// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "GpuLocalSolver2.h"
#include "FpuSupport.h"

extern void SaveGraph (const std::string&, int, float, int, const float*);

const GLchar* GpuLocalSolver2::msDeclarations = "";
const GLchar* GpuLocalSolver2::msEquation = "result += coeff.w*exp(uZZ);";

//----------------------------------------------------------------------------
GpuLocalSolver2::GpuLocalSolver2 (int dimension0, int dimension1,
    const Image2<float>* initial, const Image2<unsigned char>* domain,
    float dt, float dx0, float dx1, const std::string& folder, bool& success)
    :
    GpuPdeSolver2(msDeclarations, msEquation, dimension0, dimension1, initial,
        domain, dt, dx0, dx1, success),
    mReadBack(0),
    mSlice(0),
    mFolder(folder)
{
    if (success)
    {
        mReadBack = new1<float>(mNumTexels);
        if (initial)
        {
            memcpy(mReadBack, initial->GetPixels1D(),
                mNumTexels*sizeof(float));
        }
        else
        {
            memset(mReadBack, 0, mNumTexels*sizeof(float));
        }

        mSlice = new1<float>(mDimension[0]);
        memset(mSlice, 0, mDimension[0]*sizeof(float));
    }
}
//----------------------------------------------------------------------------
GpuLocalSolver2::~GpuLocalSolver2 ()
{
    delete1(mSlice);
    delete1(mReadBack);
}
//----------------------------------------------------------------------------
bool GpuLocalSolver2::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int j0 = mDimension[0]*(mDimension[1]/2);
    for (int i0 = 0; i0 < mDimension[0]; ++i0, ++j0)
    {
        mSlice[i0] = mReadBack[j0];
    }

    int frame = (int)iteration;
    SaveGraph(mFolder, frame, 100.0f, mDimension[0], mSlice);

    float umax = mSlice[mDimension[0]/2];
    std::cout << "frame = " << frame << " : umax = " << umax << std::endl;
#else
    WM5_UNUSED(iteration);
#endif
    return true;
}
//----------------------------------------------------------------------------
bool GpuLocalSolver2::OnPostIteration (uint64_t)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mDimension[0], mDimension[1], GL_RED, GL_FLOAT,
        mReadBack);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

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
