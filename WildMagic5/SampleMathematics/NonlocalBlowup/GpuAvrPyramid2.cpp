// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "GpuAvrPyramid2.h"

const GLchar* GpuAvrPyramid2::msQuadAverage =
"float result = 0.25*(f00 + f10 + f01 + f11);";

//----------------------------------------------------------------------------
GpuAvrPyramid2::GpuAvrPyramid2 (int dimension0, int dimension1,
    const Image2<float>& initial, bool& success)
    :
    GpuPyramid2("", msQuadAverage, dimension0, dimension1, initial, success),
    mAverage(0.0f)
{
}
//----------------------------------------------------------------------------
GpuAvrPyramid2::GpuAvrPyramid2 (int dimension0, int dimension1,
    GLuint texture0, GLuint texture1, GLuint frameBuffer0,
    GLuint frameBuffer1, bool& success)
    :
    GpuPyramid2("", msQuadAverage, dimension0, dimension1, texture0, texture1,
        frameBuffer0, frameBuffer1, success),
    mAverage(0.0f)
{
}
//----------------------------------------------------------------------------
GpuAvrPyramid2::~GpuAvrPyramid2 ()
{
}
//----------------------------------------------------------------------------
float GpuAvrPyramid2::GetAverage () const
{
    return mAverage;
}
//----------------------------------------------------------------------------
bool GpuAvrPyramid2::OnPostDraw (int level, int, int frameBuffer)
{
    if (level + 1 == mNumLevels)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[frameBuffer]);
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glReadPixels(0, 0, 1, 1, GL_RED, GL_FLOAT, &mAverage);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glReadBuffer(GL_BACK);
    }
    return true;
}
//----------------------------------------------------------------------------
