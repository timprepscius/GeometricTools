// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPUEVALUATE2_H
#define GPUEVALUATE2_H

#include "OpenGLHelper.h"

class GpuEvaluate2
{
protected:
    // Abstract base class.  The return 'success' is 'true' iff the solver is
    // successfully created.
    GpuEvaluate2 (const GLchar* declarations, const GLchar* equation,
        int dimension0, int dimension1, int numInputs, int numOutputs,
        bool& success);

public:
    virtual ~GpuEvaluate2 ();

    // Evaluate the function at each texel.
    bool Evaluate (GLuint inTexture, GLuint outFrameBuffer);

protected:
    // Support for construction.
    bool CreateGraphicsObjects (const GLchar* declarations,
        const GLchar* equation, int numInputs, int numOutputs);

    // Overrides to be specialized by derived classes.
    virtual bool OnPreEvaluation (GLuint inTexture, GLuint outFrameBuffer);
    virtual bool OnPostEvaluation (GLuint inTexture, GLuint outFrameBuffer);

    int mDimension[2], mNumTexels;
    GLuint mVertexBuffer, mVertexShader, mFragmentShader, mProgram;
    GLuint mModelPositionAttribute;
    GLint mInputSamplerLocation;

    static const float msSquare[4][2];
    static const GLchar* msVertexText;
    static const GLchar* msFragmentDeclareText;
    static const GLchar* msFragmentPrefix;
    static const GLchar* msFragmentInputText[4];
    static const GLchar* msFragmentOutputDeclareText[4];
    static const GLchar* msFragmentOutputText[4];
    static const GLchar* msFragmentSuffix;
};

#endif
