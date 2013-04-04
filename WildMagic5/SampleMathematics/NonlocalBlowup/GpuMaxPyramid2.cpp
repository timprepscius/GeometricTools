// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "GpuMaxPyramid2.h"

const GLchar* GpuMaxPyramid2::msQuadMaximum =
"float result = max(max(f00, f10), max(f01, f11));";

//----------------------------------------------------------------------------
GpuMaxPyramid2::GpuMaxPyramid2 (int dimension0, int dimension1,
    const Image2<float>& initial, bool& success)
    :
    GpuPyramid2("", msQuadMaximum, dimension0, dimension1, initial, success),
    mMaximum(0.0f)
{
}
//----------------------------------------------------------------------------
GpuMaxPyramid2::GpuMaxPyramid2 (int dimension0, int dimension1,
    GLuint texture0, GLuint texture1, GLuint frameBuffer0,
    GLuint frameBuffer1, bool& success)
    :
    GpuPyramid2("", msQuadMaximum, dimension0, dimension1, texture0, texture1,
        frameBuffer0, frameBuffer1, success),
    mMaximum(0.0f)
{
}
//----------------------------------------------------------------------------
GpuMaxPyramid2::~GpuMaxPyramid2 ()
{
}
//----------------------------------------------------------------------------
float GpuMaxPyramid2::GetMaximum () const
{
    return mMaximum;
}
//----------------------------------------------------------------------------
bool GpuMaxPyramid2::OnPostDraw (int level, int, int frameBuffer)
{
    if (level + 1 == mNumLevels)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[frameBuffer]);
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glReadPixels(0, 0, 1, 1, GL_RED, GL_FLOAT, &mMaximum);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glReadBuffer(GL_BACK);
    }
    return true;
}
//----------------------------------------------------------------------------
