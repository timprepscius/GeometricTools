// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "GpuEvaluate2.h"
#include "Wm5BitHacks.h"
using namespace Wm5;

const float GpuEvaluate2::msSquare[4][2] =
{
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

const GLchar* GpuEvaluate2::msVertexText =
"attribute vec2 modelPosition;\n"
"varying vec2 tcoord;\n"
"void main ()\n"
"{\n"
"    tcoord = modelPosition;\n"
"    gl_Position = vec4(2.0*modelPosition - 1.0, 0.0, 1.0);\n"
"}\n";

const GLchar* GpuEvaluate2::msFragmentDeclareText =
"varying vec2 tcoord;\n"
"uniform sampler2D inputSampler;\n";

const GLchar* GpuEvaluate2::msFragmentPrefix =
"void main ()\n"
"{\n";

const GLchar* GpuEvaluate2::msFragmentInputText[4] =
{
"    float inX = texture2D(inputSampler, tcoord).r;\n",
"    vec2 inXY = texture2D(inputSampler, tcoord).rg;\n",
"    vec3 inXYZ = texture2D(inputSampler, tcoord).rgb;\n",
"    vec4 inXYZW = texture2D(inputSampler, tcoord).rgba;\n"
};

const GLchar* GpuEvaluate2::msFragmentOutputDeclareText[4] =
{
"    float outX;\n",
"    vec2 outXY;\n",
"    vec3 outXYZ;\n",
"    vec4 outXYZW;\n"
};

const GLchar* GpuEvaluate2::msFragmentOutputText[4] =
{
"    gl_FragColor = vec4(outX, 0.0, 0.0, 0.0);\n",
"    gl_FragColor = vec4(outXY, 0.0, 0.0);\n",
"    gl_FragColor = vec4(outXYZ, 0.0);\n",
"    gl_FragColor = outXYZW;\n"
};

const GLchar* GpuEvaluate2::msFragmentSuffix =
"}\n";

//----------------------------------------------------------------------------
GpuEvaluate2::GpuEvaluate2 (const GLchar* declarations,
    const GLchar* equation, int dimension0, int dimension1, int numInputs,
    int numOutputs, bool& success)
    :
    mNumTexels(0),
    mVertexBuffer(0),
    mVertexShader(0),
    mFragmentShader(0),
    mProgram(0),
    mModelPositionAttribute(0),
    mInputSamplerLocation(-1)
{
    // Default initialization of the remaining class members that are arrays.
    for (int i = 0; i < 2; ++i)
    {
        mDimension[i] = 0;
    }

    // Start the construction that might have failures along the way.
    success = false;

    if (dimension0 <= 0 || !IsPowerOfTwo(dimension0)
    ||  dimension1 <= 0 || !IsPowerOfTwo(dimension1)
    ||  numInputs < 1 || numInputs > 4
    ||  numOutputs < 1 || numOutputs > 4)
    {
        return;
    }

    mDimension[0] = dimension0;
    mDimension[1] = dimension1;
    mNumTexels = dimension0*dimension1;

    if (!CreateGraphicsObjects(declarations, equation, numInputs, numOutputs))
    {
        return;
    }

    success = true;
}
//----------------------------------------------------------------------------
GpuEvaluate2::~GpuEvaluate2 ()
{
    OpenGL::DestroyProgram(mProgram);
    OpenGL::DestroyShader(mFragmentShader);
    OpenGL::DestroyShader(mVertexShader);
    OpenGL::DestroyBuffer(mVertexBuffer);
}
//----------------------------------------------------------------------------
bool GpuEvaluate2::Evaluate (GLuint inTexture, GLuint outFrameBuffer)
{
    // Base class enables mProgram.
    if (!OnPreEvaluation(inTexture, outFrameBuffer))
    {
        return false;
    }

    // Enable the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glEnableVertexAttribArray(mModelPositionAttribute);
    glVertexAttribPointer(mModelPositionAttribute, 2, GL_FLOAT, GL_FALSE,
        2*sizeof(float), 0);

    // Enable the input texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inTexture);

    // Enable the output framebuffer.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, outFrameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

    // Execute the program.
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, mDimension[0], mDimension[1]);
    glDrawArrays(GL_QUADS, 0, 4);
    glPopAttrib();

    // Disable the output framebuffer.
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDrawBuffer(GL_BACK);

    // Disable the input texture;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Base class disables mProgram.
    if (!OnPostEvaluation(inTexture, outFrameBuffer))
    {
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool GpuEvaluate2::CreateGraphicsObjects (const GLchar* declarations,
    const GLchar* equation, int numInputs, int numOutputs)
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
        std::string(msFragmentPrefix) +
        std::string(msFragmentInputText[numInputs-1]) +
        std::string(msFragmentOutputDeclareText[numOutputs-1]) +
        std::string(equation) + std::string("\n") +
        std::string(msFragmentOutputText[numOutputs-1]) +
        std::string(msFragmentSuffix);

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

    // Get the uniform location.
    mInputSamplerLocation = glGetUniformLocation(mProgram, "inputSampler");
    return true;
}
//----------------------------------------------------------------------------
bool GpuEvaluate2::OnPreEvaluation (GLuint, GLuint)
{
    if (mProgram)
    {
        glUseProgram(mProgram);
        glUniform1i(mInputSamplerLocation, 0);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool GpuEvaluate2::OnPostEvaluation (GLuint, GLuint)
{
    if (mProgram)
    {
        glUseProgram(0);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
