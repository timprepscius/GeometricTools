// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "GpuNonlocalSolver3.h"
#include "FpuSupport.h"

#ifndef DO_ALL_GPU
#include "Mm1BitHacks.h"
#endif

extern void SaveGraph (const std::string&, int, float, int, const float*);

const GLchar* GpuNonlocalSolver3::msDeclarations =
    "uniform vec2 nonlinear;";

const GLchar* GpuNonlocalSolver3::msEquation =
    "result += nonlinear.x*exp(uZZZ - nonlinear.y);";

//----------------------------------------------------------------------------
GpuNonlocalSolver3::GpuNonlocalSolver3 (int dimension0, int dimension1,
    int dimension2, const Image3<float>* initial,
    const Image3<unsigned char>* domain, float dt, float dx0, float dx1,
    float dx2, float p, const std::string& folder, bool& success)
    :
    GpuPdeSolver3(msDeclarations, msEquation, dimension0, dimension1,
        dimension2, initial, domain, dt, dx0, dx1, dx2, success),
    mPower(p),
    mSlice(0),
    mNonlinear0(0.0f),
    mNonlinear1(0.0f),
    mNonlinearLocation(-1),
    mFolder(folder),
#ifdef DO_ALL_GPU
    mMaxPyramid(mBound[0], mBound[1], 0, 0, 0, 0, success),
    mEvaluator(mBound[0], mBound[1], success),
    mAvrPyramid(mBound[0], mBound[1], 0, 0, 0, 0, success)
#else
    mReadBack(0)
#endif
{
    if (success)
    {
        mNonlinearLocation = glGetUniformLocation(mProgram, "nonlinear");
        mSlice = new1<float>(mDimension[0]);
        memset(mSlice, 0, mDimension[0]*sizeof(float));

#ifndef DO_ALL_GPU
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
#endif
    }
}
//----------------------------------------------------------------------------
GpuNonlocalSolver3::~GpuNonlocalSolver3 ()
{
    delete1(mSlice);
#ifndef DO_ALL_GPU
    delete1(mReadBack);
#endif
}
//----------------------------------------------------------------------------
#ifdef DO_ALL_GPU
//----------------------------------------------------------------------------
bool GpuNonlocalSolver3::OnPreIteration (uint64_t iteration)
{
#ifdef PRE_GAUSSSEIDEL_SAVE
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[2]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, mBound[1]/2 + mDimension[1]/2, mDimension[0], 1, GL_RED,
        GL_FLOAT, mSlice);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    int frame = (int)iteration;
    SaveGraph(mFolder, frame, 100.0f, mDimension[0], mSlice);

    float smax = mSlice[mDimension[0]/2];
    std::cout << "frame = " << frame << " : umax = " << smax << std::endl;
#else
    WM5_UNUSED(iteration);
#endif

    // The output of the last pass is u2; copy to u0.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[2]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[0]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mBound[0], mBound[1],
        0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    // Get the maximum of u2.  Both u2 and u1 are overwritten.
    if (!mMaxPyramid.Use(mTexture[mActive[2]], mTexture[mActive[1]],
        mFrameBuffer[mActive[2]], mFrameBuffer[mActive[1]]))
    {
        return false;
    }
    if (!mMaxPyramid.Execute())
    {
        return false;
    }
    float umax = mMaxPyramid.GetMaximum();

    // Compute u2 = exp(u0-umax).
    mEvaluator.SetUMax(umax);
    mEvaluator.Evaluate(mTexture[mActive[0]], mFrameBuffer[mActive[2]]);

    // Get the average of u2.  Both u2 and u1 are overwritten.
    if (!mAvrPyramid.Use(mTexture[mActive[2]], mTexture[mActive[1]],
        mFrameBuffer[mActive[2]],  mFrameBuffer[mActive[1]]))
    {
        return false;
    }
    if (!mAvrPyramid.Execute())
    {
        return false;
    }

    // Define m = max_{i0,i1,i2}[u(i0,i1,i2)] and f(i0,i1,i2) =
    // exp(u(i0,i1,i2)-m); the average is
    //   A = (1/(n0*n1*n2))*sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1}
    //       sum_{i2=0}^{n2-1} f(i0,i1,i2)
    float average = mAvrPyramid.GetAverage();

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
    float dim0m1 = mDimension[0] - 1.0f;
    float dim1m1 = mDimension[1] - 1.0f;
    float dim2m1 = mDimension[2] - 1.0f;
    float ratio = 8.0f/(dim0m1*dim1m1*dim2m1);
    float k0 = ratio*mDimension[0]*mDimension[1]*mDimension[2];
    float k1 = ratio*(mDimension[0]*dim1m1 + mDimension[1]*dim2m1 +
        mDimension[2]*dim0m1 + 1.0f);
    float integral = pow(k0*average - k1*exp(-umax), mPower);
    mNonlinear0 = mCoeff1/integral;
    mNonlinear1 = mPower*umax;

    // Copy to u0 to u1, so now u0, u1 and u2 are equal for the next pass.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[0]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[1]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mBound[0], mBound[1],
        0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    // Reenable the PDE program.
    Enable();
    glUniform2f(mNonlinearLocation, mNonlinear0, mNonlinear1);
    return true;
}
//----------------------------------------------------------------------------
bool GpuNonlocalSolver3::OnPostIteration (uint64_t)
{
    // The output of the last pass is u1; copy to u0.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[0]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mBound[0], mBound[1],
        0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    // Get the maximum of u1.  Both u1 and u2 are overwritten.
    if (!mMaxPyramid.Use(mTexture[mActive[1]], mTexture[mActive[2]],
        mFrameBuffer[mActive[1]], mFrameBuffer[mActive[2]]))
    {
        return false;
    }
    if (!mMaxPyramid.Execute())
    {
        return false;
    }
    float umax = mMaxPyramid.GetMaximum();

    // Copy to u0 to u1 and u2, so now u0, u1 and u2 are equal for the next
    // pass.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[0]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[1]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mBound[0], mBound[1],
        0);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[2]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mBound[0], mBound[1],
        0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    Enable();
    glUniform2f(mNonlinearLocation, mNonlinear0, mNonlinear1);
    return IsFinite(umax);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GpuNonlocalSolver3::Evaluator
//----------------------------------------------------------------------------
const GLchar* GpuNonlocalSolver3::Evaluator::msDeclarations =
    "uniform float umax;";

const GLchar* GpuNonlocalSolver3::Evaluator::msEquation =
    "outX = exp(inX - umax);";
//----------------------------------------------------------------------------
GpuNonlocalSolver3::Evaluator::Evaluator (int bound0, int bound1,
    bool& success)
    :
    GpuEvaluate2(msDeclarations, msEquation, bound0, bound1, 1, 1, success),
    mUMax(0.0f)
{
    mUMaxLocation = glGetUniformLocation(mProgram, "umax");
}
//----------------------------------------------------------------------------
GpuNonlocalSolver3::Evaluator::~Evaluator ()
{
}
//----------------------------------------------------------------------------
void GpuNonlocalSolver3::Evaluator::SetUMax (float umax)
{
    mUMax = umax;
}
//----------------------------------------------------------------------------
bool GpuNonlocalSolver3::Evaluator::OnPreEvaluation (GLuint texture,
    GLuint frameBuffer)
{
    if (!GpuEvaluate2::OnPreEvaluation(texture, frameBuffer))
    {
        return false;
    }
    glUniform1f(mUMaxLocation, mUMax);
    return true;
}
//----------------------------------------------------------------------------
#else
//----------------------------------------------------------------------------
void GpuNonlocalSolver3::GetIntegral (float& umax, float& integral)
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

    Image3<float> temp(mDimension[0], mDimension[1], mDimension[2]);
    float* tcurrent = temp.GetPixels1D();
    ucurrent = mReadBack;
    for (j = 0; j < mNumTexels; ++j)
    {
        *tcurrent++ = exp(*ucurrent++ - umax);
    }

    // Use a mipmap approach to averaging.
    int logDim0 = Log2OfPowerOfTwo(mDimension[0]);
    int logDim1 = Log2OfPowerOfTwo(mDimension[1]);
    int logDim2 = Log2OfPowerOfTwo(mDimension[2]);
    int i0, i1, i2, i0max, i1max, i2max;
    for (j = 0, i0max = mDimension[0]/2; j < logDim0; ++j, i0max /= 2)
    {
        for (i2 = 0; i2 < mDimension[2]; ++i2)
        {
            for (i1 = 0; i1 < mDimension[1]; ++i1)
            {
                for (i0 = 0; i0 < i0max; ++i0)
                {
                    temp(i0, i1, i2) = 0.5f*(temp(2*i0, i1, i2) +
                        temp(2*i0+1, i1, i2));
                }
            }
        }
    }
    for (j = 0, i1max = mDimension[1]/2; j < logDim1; ++j, i1max /= 2)
    {
        for (i2 = 0; i2 < mDimension[2]; ++i2)
        {
            for (i1 = 0; i1 < i1max; ++i1)
            {
                temp(0, i1, i2) = 0.5f*(temp(0, 2*i1, i2) +
                    temp(0, 2*i1+1, i2));
            }
        }
    }
    for (j = 0, i2max = mDimension[2]/2; j < logDim2; ++j, i2max /= 2)
    {
        for (i2 = 0; i2 < i2max; ++i2)
        {
            temp(0, 0, i2) = 0.5f*(temp(0, 0, 2*i2) +
                temp(0, 0, 2*i2+1));
        }
    }

    // Define m = max_{i0,i1,i2}[u(i0,i1,i2)] and f(i0,i1,i2) =
    // exp(u(i0,i1,i2)-m); the average is
    //   A = (1/(n0*n1*n2))*sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1}
    //       sum_{i2=0}^{n2-1} f(i0,i1,i2)
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
    float dim0m1 = mDimension[0] - 1.0f;
    float dim1m1 = mDimension[1] - 1.0f;
    float dim2m1 = mDimension[2] - 1.0f;
    float ratio = 8.0f/(dim0m1*dim1m1*dim2m1);
    float k0 = ratio*mDimension[0]*mDimension[1]*mDimension[2];
    float k1 = ratio*(mDimension[0]*dim1m1 + mDimension[1]*dim2m1 +
        mDimension[2]*dim0m1 + 1.0f);
    integral = pow(k0*average - k1*exp(-umax), mPower);
}
//----------------------------------------------------------------------------
bool GpuNonlocalSolver3::OnPreIteration (uint64_t iteration)
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
#endif

    float u0max, integral;
    GetIntegral(u0max, integral);
    mNonlinear0 = mCoeff1/integral;
    mNonlinear1 = mPower*u0max;
    glUniform2f(mNonlinearLocation, mNonlinear0, mNonlinear1);
    return true;
}
//----------------------------------------------------------------------------
bool GpuNonlocalSolver3::OnPostIteration (uint64_t)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, mBound[0], mBound[1], GL_RED, GL_FLOAT, mReadBack);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
#endif
//----------------------------------------------------------------------------
