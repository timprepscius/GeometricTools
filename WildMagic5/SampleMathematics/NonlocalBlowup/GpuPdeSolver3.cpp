// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2010/09/09)

#include "GpuPdeSolver3.h"
#include "Wm5BitHacks.h"
using namespace Wm5;

const float GpuPdeSolver3::msSquare[4][2] =
{
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

const GLchar* GpuPdeSolver3::msVertexText =
"attribute vec2 modelPosition;\n"
"varying vec2 tcoord;\n"
"void main ()\n"
"{\n"
"    tcoord = modelPosition;\n"
"    gl_Position = vec4(2.0*modelPosition - 1.0, 0.0, 1.0);\n"
"}\n";

const GLchar* GpuPdeSolver3::msFragmentDeclareText =
"varying vec2 tcoord;\n"
"uniform vec4 delta;\n"
"uniform vec4 coeff0;\n"
"uniform float coeff1;\n"
"uniform vec2 uvDimensions;\n"
"uniform vec2 xyDimensions;\n"
"uniform float tileColumns;\n"
"uniform sampler2D u0Sampler;\n"
"uniform sampler2D u1Sampler;\n"
"uniform sampler2D maskSampler;\n";

const GLchar* GpuPdeSolver3::msFragmentSamplerText =
"void main ()\n"
"{\n"
"    vec2 uv = uvDimensions*tcoord - vec2(0.5, 0.5);\n"
"    vec2 xy = mod(uv, xyDimensions);\n"
"    vec2 quotient = (uv - xy)/xyDimensions;\n"
"    float z = quotient.x + quotient.y*tileColumns;\n"
"    vec2 znbr = vec2(z + 1.0, z - 1.0);\n"
"    vec4 intOffset;\n"
"    intOffset.xz = mod(znbr, tileColumns);\n"
"    intOffset.yw = (znbr - intOffset.xz)/tileColumns;\n"
"    vec4 offset;\n"
"    offset.xy = xy + intOffset.xy*xyDimensions;\n"
"    offset.zw = xy + intOffset.zw*xyDimensions;\n"
"    float uZZZ = texture2D(u0Sampler, tcoord).r;\n"
"    float uPZZ = texture2D(u1Sampler, tcoord + delta.xy).r;\n"
"    float uMZZ = texture2D(u1Sampler, tcoord - delta.xy).r;\n"
"    float uZPZ = texture2D(u1Sampler, tcoord + delta.zw).r;\n"
"    float uZMZ = texture2D(u1Sampler, tcoord - delta.zw).r;\n"
"    float uZZP = texture2D(u1Sampler, tcoord + offset.xy).r;\n"
"    float uZZM = texture2D(u1Sampler, tcoord + offset.zw).r;\n"
"    float mask = texture2D(maskSampler, tcoord).r;\n"
"    float result = coeff0.x*uZZZ + coeff0.y*(uPZZ + uMZZ) +\n"
"        coeff0.z*(uZPZ + uZMZ) + coeff0.w*(uZZP + uZZM);\n";

const GLchar* GpuPdeSolver3::msFragmentResultText =
"    gl_FragColor.rgba = vec4(result*mask, 0.0, 0.0, 0.0);\n"
"}\n";

//----------------------------------------------------------------------------
GpuPdeSolver3::GpuPdeSolver3 (const GLchar* declarations,
    const GLchar* equation, int dimension0, int dimension1, int dimension2,
    const Image3<float>* initial, const Image3<unsigned char>* domain,
    float dt, float dx0, float dx1, float dx2, bool& success)
    :
    mVertexBuffer(0),
    mVertexShader(0),
    mFragmentShader(0),
    mProgram(0),
    mModelPositionAttribute(0),
    mU0SamplerLocation(-1),
    mU1SamplerLocation(-1),
    mMaskSamplerLocation(-1),
    mDeltaLocation(-1),
    mCoeff0Location(-1),
    mCoeff1Location(-1),
    mUVDimensionsLocation(-1),
    mXYDimensionsLocation(-1),
    mTileColumnsLocation(-1),
    mMaskTexture(0),
    mTileColumns(0),
    mCoeff1(0.0f)
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
    for (i = 0; i < 2; ++i)
    {
        mFactor[i] = 0;
        mBound[i] = 0;
        mUVDimensions[i] = 0.0f;
        mXYDimensions[i] = 0.0f;
    }
    for (i = 0; i < 4; ++i)
    {
        mDelta[i] = 0.0f;
        mCoeff0[i] = 0.0f;
    }

    success = false;

    // Start the construction that might have failures along the way.
    if (dimension0 <= 0 || !IsPowerOfTwo(dimension0)
    ||  dimension1 <= 0 || !IsPowerOfTwo(dimension1)
    ||  dimension2 <= 0 || !IsPowerOfTwo(dimension2)
    ||  dt <= 0.0f || dx0 <= 0.0f || dx1 <= 0.0f || dx2 <= 0.0f)
    {
        return;
    }

    mDimension[0] = dimension0;
    mDimension[1] = dimension1;
    mDimension[2] = dimension2;
    mNumTexels = dimension0*dimension1*dimension2;

    if (!CreateGraphicsObjects(declarations, equation))
    {
        return;
    }

    // The 3D initial data and domain must be mapped to 2D chunks of memory
    // for use as textures and framebuffer objects.
    int logDim2 = Log2OfPowerOfTwo(dimension2);
    mFactor[0] = (1 << (logDim2 - logDim2/2));
    mFactor[1] = (1 << (logDim2/2));
    mBound[0] = mDimension[0]*mFactor[0];
    mBound[1] = mDimension[1]*mFactor[1];

    SetShaderConstants(dt, dx0, dx1, dx2);
    SetInitialValues(initial);
    SetMaskValues(domain);

    success = true;
}
//----------------------------------------------------------------------------
GpuPdeSolver3::~GpuPdeSolver3 ()
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
bool GpuPdeSolver3::Enable ()
{
    if (mProgram)
    {
        glUseProgram(mProgram);
        glUniform1i(mU0SamplerLocation, 0);
        glUniform1i(mU1SamplerLocation, 1);
        glUniform1i(mMaskSamplerLocation, 2);
        glUniform4fv(mDeltaLocation, 1, mDelta);
        glUniform4fv(mCoeff0Location, 1, mCoeff0);
        glUniform1f(mCoeff1Location, mCoeff1);
        glUniform2fv(mUVDimensionsLocation, 1, mUVDimensions);
        glUniform2fv(mXYDimensionsLocation, 1, mXYDimensions);
        glUniform1f(mTileColumnsLocation, mTileColumns);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver3::Disable ()
{
    if (mProgram)
    {
        glUseProgram(0);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver3::Execute (uint64_t iteration, int numGaussSeidel)
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
        glViewport(0, 0, mBound[0], mBound[1]);
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
bool GpuPdeSolver3::CreateGraphicsObjects (const GLchar* declarations,
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
    mCoeff0Location = glGetUniformLocation(mProgram, "coeff0");
    mCoeff1Location = glGetUniformLocation(mProgram, "coeff1");
    mUVDimensionsLocation = glGetUniformLocation(mProgram, "uvDimensions");
    mXYDimensionsLocation = glGetUniformLocation(mProgram, "xyDimensions");
    mTileColumnsLocation = glGetUniformLocation(mProgram, "tileColumns");
    return true;
}
//----------------------------------------------------------------------------
void GpuPdeSolver3::SetShaderConstants (float dt, float dx0, float dx1,
    float dx2)
{
    mUVDimensions[0] = (float)mBound[0];
    mUVDimensions[1] = (float)mBound[1];
    mXYDimensions[0] = (float)mDimension[0];
    mXYDimensions[1] = (float)mDimension[1];
    mTileColumns = (float)mFactor[0];

    mDelta[0] = 1.0f/(float)mBound[0];
    mDelta[1] = 0.0f;
    mDelta[2] = 0.0f;
    mDelta[3] = 1.0f/(float)mBound[1];

    float r0 = dt/(dx0*dx0), r1 = dt/(dx1*dx1), r2 = dt/(dx2*dx2);
    mCoeff0[0] = 1.0f/(1.0f + 2.0f*(r0 + r1 + r2));
    mCoeff0[1] = mCoeff0[0]*r0;
    mCoeff0[2] = mCoeff0[0]*r1;
    mCoeff0[3] = mCoeff0[0]*r2;
    mCoeff1 = mCoeff0[0]*dt;
}
//----------------------------------------------------------------------------
void GpuPdeSolver3::SetInitialValues (const Image3<float>* initial)
{
    float* texels = new1<float>(mNumTexels);
    int u, v, i0, i1, i2;

    if (initial)
    {
        for (v = 0; v < mBound[1]; ++v)
        {
            for (u = 0; u < mBound[0]; ++u)
            {
                Map2Dto3D(u, v, i0, i1, i2);
                texels[u + mBound[0]*v] = (*initial)(i0, i1, i2);
            }
        }
    }
    else
    {
        memset(texels, 0, mNumTexels*sizeof(float));
    }

    for (int i = 0; i < 3; ++i)
    {
        mTexture[i] = OpenGL::CreateTexture2D(mBound[0], mBound[1], GL_RED,
            GL_R32F, GL_FLOAT, texels);
        mFrameBuffer[i] = OpenGL::CreateFrameBuffer(mTexture[i]);
    }

    delete1(texels);
}
//----------------------------------------------------------------------------
void GpuPdeSolver3::SetMaskValues (const Image3<unsigned char>* domain)
{
    unsigned char* mask = new1<unsigned char>(mNumTexels);
    int u, v, i0, i1, i2;

    if (domain)
    {
        for (v = 0; v < mBound[1]; ++v)
        {
            for (u = 0; u < mBound[0]; ++u)
            {
                Map2Dto3D(u, v, i0, i1, i2);
                mask[u + mBound[0]*v] = (*domain)(i0, i1, i2);
            }
        }
    }
    else
    {
        memset(mask, 0xFF, mNumTexels);
        for (v = 0; v < mBound[1]; ++v)
        {
            for (u = 0; u < mBound[0]; ++u)
            {
                Map2Dto3D(u, v, i0, i1, i2);
                if (i0 == 0 || i0 == mDimension[0] - 1
                ||  i1 == 0 || i1 == mDimension[1] - 1
                ||  i2 == 0 || i2 == mDimension[2] - 1)
                {
                    mask[u + mBound[0]*v] = 0;
                }
            }
        }
    }

    mMaskTexture = OpenGL::CreateTexture2D(mBound[0], mBound[1], GL_RED,
        GL_R8, GL_UNSIGNED_BYTE, mask);

    delete1(mask);
}
//----------------------------------------------------------------------------
void GpuPdeSolver3::Map2Dto3D (int u, int v, int& x, int& y, int& z) const
{
    x = u % mDimension[0];
    y = v % mDimension[1];
    z = (u / mDimension[0]) + (v / mDimension[1]) * mFactor[0];
}
//----------------------------------------------------------------------------
void GpuPdeSolver3::Map3Dto2D (int x, int y, int z, int& u, int& v) const
{
    u = x + (z % mFactor[0]) * mDimension[0];
    v = y + (z / mFactor[0]) * mDimension[1];
}
//----------------------------------------------------------------------------
bool GpuPdeSolver3::OnPreIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
bool GpuPdeSolver3::OnPostIteration (uint64_t)
{
    // Stub for derived classes.
    return true;
}
//----------------------------------------------------------------------------
