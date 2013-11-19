// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "GpuLocalSolver3.h"
#include "FpuSupport.h"

extern void SaveGraph (const std::string&, int, float, int, const float*);

const GLchar* GpuLocalSolver3::msDeclarations = "";
const GLchar* GpuLocalSolver3::msEquation = "result += coeff1*exp(uZZZ);";

//----------------------------------------------------------------------------
GpuLocalSolver3::GpuLocalSolver3 (int dimension0, int dimension1,
    int dimension2, const Image3<float>* initial,
    const Image3<unsigned char>* domain, float dt, float dx0, float dx1,
    float dx2, const std::string& folder, bool& success)
    :
    GpuPdeSolver3(msDeclarations, msEquation, dimension0, dimension1,
        dimension2, initial, domain, dt, dx0, dx1, dx2, success),
    mReadBack(0),
    mSlice(0),
    mFolder(folder)
{
    if (success)
    {
        mReadBack = new1<float>(mNumTexels);
        if (initial)
        {
            for (int v = 0; v < mBound[1]; ++v)
            {
                for (int u = 0; u < mBound[0]; ++u)
                {
                    int i0, i1, i2;
                    Map2Dto3D(u, v, i0, i1, i2);
                    mReadBack[u + mBound[0]*v] = (*initial)(i0, i1, i2);
                }
            }
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
GpuLocalSolver3::~GpuLocalSolver3 ()
{
    delete1(mSlice);
    delete1(mReadBack);
}
//----------------------------------------------------------------------------
bool GpuLocalSolver3::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    int u, v;
    for (int i0 = 0; i0 < mDimension[0]; ++i0)
    {
        Map3Dto2D(i0, mDimension[1]/2, mDimension[2]/2, u, v);
        mSlice[i0] = mReadBack[u + mBound[0]*v];
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
bool GpuLocalSolver3::OnPostIteration (uint64_t)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mBound[0], mBound[1], GL_RED, GL_FLOAT, mReadBack);
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
