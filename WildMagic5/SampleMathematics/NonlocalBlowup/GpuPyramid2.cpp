// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "GpuPyramid2.h"
#include "Wm5BitHacks.h"
using namespace Wm5;

const float GpuPyramid2::msSquare[4][2] =
{
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

const GLchar* GpuPyramid2::msVertexText =
"attribute vec2 modelPosition;\n"
"varying vec2 tcoord;\n"
"void main ()\n"
"{\n"
"    tcoord = modelPosition;\n"
"    gl_Position = vec4(2.0*modelPosition - 1.0, 0.0, 1.0);\n"
"}\n";

const GLchar* GpuPyramid2::msFragmentDeclareText =
"varying vec2 tcoord;\n"
"uniform vec4 delta;\n"
"uniform vec2 multiplier;\n"
"uniform vec2 offset;\n"
"uniform sampler2D rSampler;\n";

const GLchar* GpuPyramid2::msFragmentSamplerText =
"void main ()\n"
"{\n"
"    vec2 prevTCoord = multiplier*tcoord - offset;\n"
"    float f00 = texture2D(rSampler, prevTCoord).r;\n"
"    float f10 = texture2D(rSampler, prevTCoord + delta.xy).r;\n"
"    float f01 = texture2D(rSampler, prevTCoord + delta.zw).r;\n"
"    float f11 = texture2D(rSampler, prevTCoord + delta.xy + delta.zw).r;\n";

const GLchar* GpuPyramid2::msFragmentResultText =
"    gl_FragColor.r = result;\n"
"    gl_FragColor.gba = vec3(0.0, 0.0, 0.0);\n"
"}\n";

//----------------------------------------------------------------------------
GpuPyramid2::GpuPyramid2 (const GLchar* declarations, const GLchar* equation,
    int dimension0, int dimension1, const Image2<float>& initial,
    bool& success)
    :
    mNumTexels(0),
    mNumLevels(0),
    mVertexBuffer(0),
    mVertexShader(0),
    mFragmentShader(0),
    mProgram(0),
    mModelPositionAttribute(0),
    mRSamplerLocation(-1),
    mDeltaLocation(-1),
    mMultiplierLocation(-1),
    mOffsetLocation(-1),
    mOwner(true)
{
    // Default initialization of the remaining class members that are arrays.
    for (int i = 0; i < 2; ++i)
    {
        mDimension[i] = 0;
        mTexture[i] = 0;
        mFrameBuffer[i] = 0;
        mDelta[i] = 0.0f;
        mMultiplier[i] = 0.0f;
        mSize[i] = 0;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (dimension0 <= 0 || !IsPowerOfTwo(dimension0)
    ||  dimension1 <= 0 || !IsPowerOfTwo(dimension1))
    {
        return;
    }

    mDimension[0] = dimension0;
    mDimension[1] = dimension1;
    mNumTexels = dimension0*dimension1;
    int logDim0 = Log2OfPowerOfTwo(dimension0);
    int logDim1 = Log2OfPowerOfTwo(dimension1);
    mNumLevels = (logDim0 >= logDim1 ? logDim0 + 1 : logDim1 + 1);

    if (!CreateGraphicsObjects(declarations, equation))
    {
        return;
    }

    SetInitialValues(initial);

    success = true;
}
//----------------------------------------------------------------------------
GpuPyramid2::GpuPyramid2 (const GLchar* declarations, const GLchar* equation,
    int dimension0, int dimension1, GLuint texture0, GLuint texture1,
    GLuint frameBuffer0, GLuint frameBuffer1, bool& success)
    :
    mNumTexels(0),
    mNumLevels(0),
    mFragmentShader(0),
    mProgram(0),
    mModelPositionAttribute(0),
    mRSamplerLocation(-1),
    mDeltaLocation(-1),
    mMultiplierLocation(-1),
    mOffsetLocation(-1),
    mOwner(false)
{
    // Default initialization of the remaining class members that are arrays.
    mTexture[0] = texture0;
    mTexture[1] = texture1;
    mFrameBuffer[0] = frameBuffer0;
    mFrameBuffer[1] = frameBuffer1;
    for (int i = 0; i < 2; ++i)
    {
        mDimension[i] = 0;
        mDelta[i] = 0.0f;
        mMultiplier[i] = 0.0f;
        mSize[i] = 0;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (dimension0 <= 0 || !IsPowerOfTwo(dimension0)
    ||  dimension1 <= 0 || !IsPowerOfTwo(dimension1))
    {
        return;
    }

    mDimension[0] = dimension0;
    mDimension[1] = dimension1;
    mNumTexels = dimension0*dimension1;
    int logDim0 = Log2OfPowerOfTwo(dimension0);
    int logDim1 = Log2OfPowerOfTwo(dimension1);
    mNumLevels = (logDim0 >= logDim1 ? logDim0 + 1 : logDim1 + 1);

    if (!CreateGraphicsObjects(declarations, equation))
    {
        return;
    }

    success = true;
}
//----------------------------------------------------------------------------
GpuPyramid2::~GpuPyramid2 ()
{
    OpenGL::DestroyProgram(mProgram);

    if (mOwner)
    {
        for (int i = 0; i < 2; ++i)
        {
            OpenGL::DestroyFrameBuffer(mFrameBuffer[i]);
            OpenGL::DestroyTexture(mTexture[i]);
        }
    }

    OpenGL::DestroyShader(mFragmentShader);
    OpenGL::DestroyShader(mVertexShader);
    OpenGL::DestroyBuffer(mVertexBuffer);
}
//----------------------------------------------------------------------------
bool GpuPyramid2::Execute ()
{
    int j;
    for (j = 0; j < 2; ++j)
    {
        mDelta[j] = 0.5f/(float)mDimension[j];
        mSize[j] = mDimension[j];
        mMultiplier[j] = 2.0f;
    }

    // Base class enables mProgram.
    if (!OnPreIteration())
    {
        return false;
    }

    for (int level = 1, active = 0; level < mNumLevels; ++level)
    {
        for (j = 0; j < 2; ++j)
        {
            if (mSize[j] > 1)
            {
                mSize[j] /= 2;
                mMultiplier[j] *= 0.5f;
            }
        }

        if (!OnPreDraw(level, active, 1-active))
        {
            return false;
        }

        // Enable the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glEnableVertexAttribArray(mModelPositionAttribute);
        glVertexAttribPointer(mModelPositionAttribute, 2, GL_FLOAT, GL_FALSE,
            2*sizeof(float), 0);

        // Update the multipliers.
        glUniform2f(mMultiplierLocation, mMultiplier[0], mMultiplier[1]);

        // Enable the input texture.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture[active]);

        // Enable the output framebuffer.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[1-active]);
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

        // Execute the program.
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, mSize[0], mSize[1]);
        glDrawArrays(GL_QUADS, 0, 4);
        glPopAttrib();

        // Disable the output framebuffer.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);

        // Disable the input texture;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Disable the vertex buffer.
        glDisableVertexAttribArray(mModelPositionAttribute);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!OnPostDraw(level, active, 1-active))
        {
            return false;
        }

        for (j = 0; j < 2; ++j)
        {
            if (mSize[j] == 1 && mDelta[j] > 0.0f)
            {
                mDelta[j] = 0.0f;
                glUniform4f(mDeltaLocation, mDelta[0], 0.0f, 0.0f, mDelta[1]);
            }
        }

        active = 1 - active;
    }

    // Base class disables mProgram.
    if (!OnPostIteration())
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool GpuPyramid2::Use (GLuint texture0, GLuint texture1, GLuint frameBuffer0,
    GLuint frameBuffer1)
{
    if (!mOwner)
    {
        mTexture[0] = texture0;
        mTexture[1] = texture1;
        mFrameBuffer[0] = frameBuffer0;
        mFrameBuffer[1] = frameBuffer1;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPyramid2::CreateGraphicsObjects (const GLchar* declarations,
    const GLchar* equation)
{
    mVertexBuffer = OpenGL::CreateVertexBuffer(4, 2*sizeof(float),
        GL_STATIC_DRAW, &msSquare[0][0]);
    if (!mVertexBuffer)
    {
        return false;
    }

    mVertexShader = OpenGL::CreateVertexShader(msVertexText);
    if (!mVertexShader)
    {
        OpenGL::DestroyBuffer(mVertexBuffer);
        return false;
    }

    std::string fragmentText =
        std::string(msFragmentDeclareText) +
        std::string(declarations) + std::string("\n") +
        std::string(msFragmentSamplerText) +
        std::string(equation) + std::string("\n") +
        std::string(msFragmentResultText);

    mFragmentShader = OpenGL::CreateFragmentShader(fragmentText.c_str());
    if (!mFragmentShader)
    {
        OpenGL::DestroyShader(mVertexShader);
        OpenGL::DestroyBuffer(mVertexBuffer);
        return false;
    }

    mProgram = OpenGL::CreateProgram(mVertexShader, mFragmentShader);
    if (!mProgram)
    {
        OpenGL::DestroyShader(mFragmentShader);
        OpenGL::DestroyShader(mVertexShader);
        OpenGL::DestroyBuffer(mVertexBuffer);
        return false;
    }

    // Bind the model position to index mModelPosition.
    glBindAttribLocation(mProgram, mModelPositionAttribute, "modelPosition");

    // Get the uniform locations.
    mRSamplerLocation = glGetUniformLocation(mProgram, "rSampler");
    mDeltaLocation = glGetUniformLocation(mProgram, "delta");
    mMultiplierLocation = glGetUniformLocation(mProgram, "multiplier");
    mOffsetLocation = glGetUniformLocation(mProgram, "offset");
    return true;
}
//----------------------------------------------------------------------------
void GpuPyramid2::SetInitialValues (const Image2<float>& initial)
{
    mTexture[0] = OpenGL::CreateTexture2D(mDimension[0], mDimension[1],
        GL_RED, GL_R32F, GL_FLOAT, initial.GetPixels1D());

    mTexture[1] = OpenGL::CreateTexture2D(mDimension[0], mDimension[1],
        GL_RED, GL_R32F, GL_FLOAT, 0);

    mFrameBuffer[0] = OpenGL::CreateFrameBuffer(mTexture[0]);
    mFrameBuffer[1] = OpenGL::CreateFrameBuffer(mTexture[1]);
}
//----------------------------------------------------------------------------
bool GpuPyramid2::OnPreIteration ()
{
    if (!mProgram)
    {
        return false;
    }

    int j;
    if (!mOwner)
    {
        for (j = 0; j < 2; ++j)
        {
            if (mTexture[j] == 0 || mFrameBuffer[j] == 0)
            {
                return false;
            }
        }
    }

    glUseProgram(mProgram);
    glUniform1i(mRSamplerLocation, 0);
    glUniform4f(mDeltaLocation, mDelta[0], 0.0f, 0.0f, mDelta[1]);
    glUniform2f(mMultiplierLocation, mMultiplier[0], mMultiplier[1]);
    glUniform2f(mOffsetLocation, mDelta[0], mDelta[1]);
    return true;
}
//----------------------------------------------------------------------------
bool GpuPyramid2::OnPostIteration ()
{
    if (mProgram)
    {
        glUseProgram(0);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPyramid2::OnPreDraw (int, int, int)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
bool GpuPyramid2::OnPostDraw (int, int, int)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
