// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "NonlocalSolver2.h"
#include "FpuSupport.h"

const GLchar* NonlocalSolver2::msDeclarations =
    "uniform vec2 nonlinear;";

const GLchar* NonlocalSolver2::msEquation =
    "result += nonlinear.x*exp(uZZ - nonlinear.y);";

//----------------------------------------------------------------------------
NonlocalSolver2::NonlocalSolver2 (int dimension0, int dimension1,
    const Image2<float>* initial, const Image2<unsigned char>* domain,
    float dt, float dx0, float dx1, float p, bool& success)
    :
    GpuPdeSolver2(msDeclarations, msEquation, dimension0, dimension1, initial,
        domain, dt, dx0, dx1, success),
    mPower(p),
    mReadBack(0),
    mNonlinear0(0.0f),
    mNonlinear1(0.0f),
    mNonlinearLocation(-1),
    mMaxPyramid(dimension0, dimension1, 0, 0, 0, 0, success),
    mEvaluator(dimension0, dimension1, success),
    mAvrPyramid(dimension0, dimension1, 0, 0, 0, 0, success)
{
    if (success)
    {
        mNonlinearLocation = glGetUniformLocation(mProgram, "nonlinear");
        mReadBack = new1<float>(mDimension[0]*mDimension[1]);
        memset(mReadBack, 0, mDimension[0]*mDimension[1]*sizeof(float));
    }
}
//----------------------------------------------------------------------------
NonlocalSolver2::~NonlocalSolver2 ()
{
    delete1(mReadBack);
}
//----------------------------------------------------------------------------
bool NonlocalSolver2::OnPreIteration (uint64_t)
{
    // The output of the last pass is u2; copy to u0.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[2]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[0]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mDimension[0],
        mDimension[1], 0);
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

    // Define m = max_{i0,i1}[u(i0,i1)] and f(i0,i1) = exp(u(i0,i1)-m); the
    // average is
    //   A = (1/(n0*n1))*sum_{i0=0}^{n0-1} sum_{i1=0}^{n1-1} f(i0,i1)
    float average = mAvrPyramid.GetAverage();

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
    float ratio = 4.0f/((mDimension[0] - 1.0f)*(mDimension[1] - 1.0f));
    float k0 = ratio*mDimension[0]*mDimension[1];
    float k1 = ratio*(mDimension[0] + mDimension[1] - 5.0f);
    float integral = pow(k0*average - k1*exp(-umax), mPower);
    mNonlinear0 = mCoeff[3]/integral;
    mNonlinear1 = mPower*umax;

    // Copy to u0 to u1, so now u0, u1 and u2 are equal for the next pass.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[0]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[1]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mDimension[0],
        mDimension[1], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    // Reenable the PDE program.
    Enable();
    glUniform2f(mNonlinearLocation, mNonlinear0, mNonlinear1);
    return true;
}
//----------------------------------------------------------------------------
bool NonlocalSolver2::OnPostIteration (uint64_t)
{
    // The output of the last pass is u1; copy to u0.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[1]]);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[0]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mDimension[0],
        mDimension[1], 0);
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
    glReadPixels(0, 0, mDimension[0], mDimension[1], GL_RED, GL_FLOAT,
        mReadBack);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[1]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mDimension[0],
        mDimension[1], 0);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[2]]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 0, 0, mDimension[0],
        mDimension[1], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glReadBuffer(GL_BACK);

    Enable();
    glUniform2f(mNonlinearLocation, mNonlinear0, mNonlinear1);
    return IsFinite(umax);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// NonlocalSolver2::Evaluator
//----------------------------------------------------------------------------
const GLchar* NonlocalSolver2::Evaluator::msDeclarations =
    "uniform float umax;";

const GLchar* NonlocalSolver2::Evaluator::msEquation =
    "outX = exp(inX - umax);";
//----------------------------------------------------------------------------
NonlocalSolver2::Evaluator::Evaluator (int dimension0, int dimension1,
    bool& success)
    :
    GpuEvaluate2(msDeclarations, msEquation, dimension0, dimension1, 1, 1,
        success),
    mUMax(0.0f)
{
    mUMaxLocation = glGetUniformLocation(mProgram, "umax");
}
//----------------------------------------------------------------------------
NonlocalSolver2::Evaluator::~Evaluator ()
{
}
//----------------------------------------------------------------------------
void NonlocalSolver2::Evaluator::SetUMax (float umax)
{
    mUMax = umax;
}
//----------------------------------------------------------------------------
bool NonlocalSolver2::Evaluator::OnPreEvaluation (GLuint texture,
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
