// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2010/09/09)

#include "GpuPdeSolver2.h"
#include "Wm5BitHacks.h"
using namespace Wm5;

const float GpuPdeSolver2::msSquare[4][2] =
{
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

const GLchar* GpuPdeSolver2::msVertexText =
"attribute vec2 modelPosition;\n"
"varying vec2 tcoord;\n"
"void main ()\n"
"{\n"
"    tcoord = modelPosition;\n"
"    gl_Position = vec4(2.0*modelPosition - 1.0, 0.0, 1.0);\n"
"}\n";

const GLchar* GpuPdeSolver2::msFragmentDeclareText =
"varying vec2 tcoord;\n"
"uniform vec4 delta;\n"
"uniform vec4 coeff;\n"
"uniform sampler2D u0Sampler;\n"
"uniform sampler2D u1Sampler;\n"
"uniform sampler2D maskSampler;\n";

const GLchar* GpuPdeSolver2::msFragmentSamplerText =
"void main ()\n"
"{\n"
"    float uZZ = texture2D(u0Sampler, tcoord).r;\n"
"    float uPZ = texture2D(u1Sampler, tcoord + delta.xy).r;\n"
"    float uMZ = texture2D(u1Sampler, tcoord - delta.xy).r;\n"
"    float uZP = texture2D(u1Sampler, tcoord + delta.zw).r;\n"
"    float uZM = texture2D(u1Sampler, tcoord - delta.zw).r;\n"
"    float mask = texture2D(maskSampler, tcoord).r;\n"
"    float result = coeff.x*uZZ + coeff.y*(uPZ + uMZ) +\n"
"        coeff.z*(uZP + uZM);\n";

const GLchar* GpuPdeSolver2::msFragmentResultText =
"    gl_FragColor.rgba = vec4(result*mask, 0.0, 0.0, 0.0);\n"
"}\n";

//----------------------------------------------------------------------------
GpuPdeSolver2::GpuPdeSolver2 (const GLchar* declarations,
    const GLchar* equation, int dimension0, int dimension1,
    const Image2<float>* initial, const Image2<unsigned char>* domain,
    float dt, float dx0, float dx1, bool& success)
    :
    mNumTexels(0),
    mVertexBuffer(0),
    mVertexShader(0),
    mFragmentShader(0),
    mProgram(0),
    mModelPositionAttribute(0),
    mU0SamplerLocation(-1),
    mU1SamplerLocation(-1),
    mMaskSamplerLocation(-1),
    mDeltaLocation(-1),
    mCoeffLocation(-1),
    mMaskTexture(0)
{
    // Default initialization of the remaining class members that are arrays.
    int i;
    for (i = 0; i < 3; ++i)
    {
        mDimension[i] = 0;
        mTexture[i] = 0;
        mFrameBuffer[i] = 0;
        mActive[i] = 0;
    }

    for (i = 0; i < 4; ++i)
    {
        mDelta[i] = 0.0f;
        mCoeff[i] = 0.0f;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (dimension0 <= 0 || !IsPowerOfTwo(dimension0)
    ||  dimension1 <= 0 || !IsPowerOfTwo(dimension1)
    ||  dt <= 0.0f || dx0 <= 0.0f || dx1 <= 0.0f)
    {
        return;
    }

    mDimension[0] = dimension0;
    mDimension[1] = dimension1;
    mNumTexels = dimension0*dimension1;

    if (!CreateGraphicsObjects(declarations, equation))
    {
        return;
    }

    SetShaderConstants(dt, dx0, dx1);
    SetInitialValues(initial);
    SetMaskValues(domain);

    success = true;
}
//----------------------------------------------------------------------------
GpuPdeSolver2::~GpuPdeSolver2 ()
{
    OpenGL::DestroyProgram(mProgram);
    OpenGL::DestroyTexture(mMaskTexture);
    for (int i = 0; i < 3; ++i)
    {
        OpenGL::DestroyFrameBuffer(mFrameBuffer[i]);
        OpenGL::DestroyTexture(mTexture[i]);
    }
    OpenGL::DestroyShader(mFragmentShader);
    OpenGL::DestroyShader(mVertexShader);
    OpenGL::DestroyBuffer(mVertexBuffer);
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::Enable ()
{
    if (mProgram)
    {
        glUseProgram(mProgram);
        glUniform1i(mU0SamplerLocation, 0);
        glUniform1i(mU1SamplerLocation, 1);
        glUniform1i(mMaskSamplerLocation, 2);
        glUniform4fv(mDeltaLocation, 1, mDelta);
        glUniform4fv(mCoeffLocation, 1, mCoeff);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::Disable ()
{
    if (mProgram)
    {
        glUseProgram(0);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::Execute (uint64_t iteration, int numGaussSeidel)
{
    if (iteration == 0)
    {
        mActive[0] = 0;
    }

    mActive[1] = 1 - mActive[0];
    mActive[2] = 2;

    if (!OnPreIteration(iteration))
    {
        return false;
    }

    // Enable the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glEnableVertexAttribArray(mModelPositionAttribute);
    glVertexAttribPointer(mModelPositionAttribute, 2, GL_FLOAT, GL_FALSE,
        2*sizeof(float), 0);

    // Enable the mask texture.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mMaskTexture);

    // Enable the u0 texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[mActive[0]]);

    for (int i = 0; i < numGaussSeidel; ++i)
    {
        // Enable the u1 texture.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mTexture[mActive[1]]);

        // Enable the u2 framebuffer.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer[mActive[2]]);
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

        // Execute the program.
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, mDimension[0], mDimension[1]);
        glDrawArrays(GL_QUADS, 0, 4);
        glPopAttrib();

        // Disable the u2 framebuffer.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);

        // Disable the u1 texture.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        int save = mActive[1];
        mActive[1] = mActive[2];
        mActive[2] = save;
    }

    // Disable the u0 texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Disable the mask texture.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Disable the vertex buffer.
    glDisableVertexAttribArray(mModelPositionAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!OnPostIteration(iteration))
    {
        return false;
    }

    mActive[0] = 1 - mActive[0];
    return true;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::CreateGraphicsObjects (const GLchar* declarations,
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
    mU0SamplerLocation = glGetUniformLocation(mProgram, "u0Sampler");
    mU1SamplerLocation = glGetUniformLocation(mProgram, "u1Sampler");
    mMaskSamplerLocation = glGetUniformLocation(mProgram, "maskSampler");
    mDeltaLocation = glGetUniformLocation(mProgram, "delta");
    mCoeffLocation = glGetUniformLocation(mProgram, "coeff");
    return true;
}
//----------------------------------------------------------------------------
void GpuPdeSolver2::SetShaderConstants (float dt, float dx0, float dx1)
{
    mDelta[0] = 1.0f/(float)mDimension[0];
    mDelta[1] = 0.0f;
    mDelta[2] = 0.0f;
    mDelta[3] = 1.0f/(float)mDimension[1];

    float r0 = dt/(dx0*dx0), r1 = dt/(dx1*dx1);
    mCoeff[0] = 1.0f/(1.0f + 2.0f*(r0 + r1));
    mCoeff[1] = mCoeff[0]*r0;
    mCoeff[2] = mCoeff[0]*r1;
    mCoeff[3] = mCoeff[0]*dt;
}
//----------------------------------------------------------------------------
void GpuPdeSolver2::SetInitialValues (const Image2<float>* initial)
{
    int i;

    if (initial)
    {
        for (i = 0; i < 3; ++i)
        {
            mTexture[i] = OpenGL::CreateTexture2D(mDimension[0],
                mDimension[1], GL_RED, GL_R32F, GL_FLOAT,
                initial->GetPixels1D());
        }
    }
    else
    {
        float* zeros = new1<float>(mNumTexels);
        memset(zeros, 0, mNumTexels*sizeof(float));
        for (i = 0; i < 3; ++i)
        {
            mTexture[i] = OpenGL::CreateTexture2D(mDimension[0],
                mDimension[1], GL_RED, GL_R32F, GL_FLOAT, zeros);
        }
        delete1(zeros);
    }

    for (i = 0; i < 3; ++i)
    {
        mFrameBuffer[i] = OpenGL::CreateFrameBuffer(mTexture[i]);
    }
}
//----------------------------------------------------------------------------
void GpuPdeSolver2::SetMaskValues (const Image2<unsigned char>* domain)
{
    if (domain)
    {
        mMaskTexture = OpenGL::CreateTexture2D(mDimension[0], mDimension[1],
            GL_RED, GL_R8, GL_UNSIGNED_BYTE, domain->GetPixels1D());
    }
    else
    {
        Image2<unsigned char> mask(mDimension[0], mDimension[1]);
        memset(mask.GetPixels1D(), 0xFF, mNumTexels);
        for (int i0 = 0; i0 < mDimension[0]; ++i0)
        {
            mask(i0, 0) = 0;
            mask(i0, mDimension[1] - 1) = 0;
        }
        for (int i1 = 0; i1 < mDimension[1]; ++i1)
        {
            mask(0, i1) = 0;
            mask(mDimension[0] - 1, i1) = 0;
        }
        mMaskTexture = OpenGL::CreateTexture2D(mDimension[0], mDimension[1],
            GL_RED, GL_R8, GL_UNSIGNED_BYTE, mask.GetPixels1D());
    }
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::OnPreIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver2::OnPostIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
