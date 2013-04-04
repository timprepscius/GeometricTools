// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "GpuLocalSolver1.h"
#include "FpuSupport.h"

extern void SaveGraph (const std::string&, int, float, int, const float*);

const GLchar* GpuLocalSolver1::msDeclarations = "";
const GLchar* GpuLocalSolver1::msEquation = "result += coeff.z*exp(uZ);";

//----------------------------------------------------------------------------
GpuLocalSolver1::GpuLocalSolver1 (int dimension,
    const Image1<float>* initial, const Image1<unsigned char>* domain,
    float dt, float dx, const std::string& folder, bool& success)
    :
    GpuPdeSolver1(msDeclarations, msEquation, dimension, initial, domain,
        dt, dx, success),
    mReadBack(0),
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
    }
}
//----------------------------------------------------------------------------
GpuLocalSolver1::~GpuLocalSolver1 ()
{
    delete1(mReadBack);
}
//----------------------------------------------------------------------------
bool GpuLocalSolver1::OnPreIteration (uint64_t iteration)
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
bool GpuLocalSolver1::OnPostIteration (uint64_t)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, mDimension, 1, GL_RED, GL_FLOAT, mReadBack);
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
