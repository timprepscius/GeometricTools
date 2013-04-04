// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPUPDESOLVER1_H
#define GPUPDESOLVER1_H

#include "OpenGLHelper.h"
#include "Image1.h"

class GpuPdeSolver1
{
protected:
    // Abstract base class.  The return 'success' is 'true' iff the solver is
    // successfully created.
    GpuPdeSolver1 (const GLchar* declarations, const GLchar* equation,
        int dimension, const Image1<float>* initial,
        const Image1<unsigned char>* domain, float dt, float dx,
        bool& success);

public:
    virtual ~GpuPdeSolver1 ();

    // The function Enable() activates the GPU program and sets the shader
    // uniforms.  The function Disable() deactives the GPU program.  A derived
    // class should override these when additional shader uniforms must be set
    // or when other OpenGL state must be enabled or disabled.
    virtual bool Enable ();
    virtual bool Disable ();

    // Compute one step of the solver.  This function must be called after
    // Enable() is called the first time.  If an application performs OpenGL
    // operations that cause a change in active program, Enable() must be
    // recalled before Execute() to restore the solver program and set the
    // uniforms.  The callback OnPreIteration(...) is called before the
    // solver step and the callback OnPostIteration(...) is called after
    // the solver step.  A derived class may override these if additional
    // behavior is needed before and after the solver step.  If the input
    // 'iteration' is zero, the solver assumes this is the initial step and
    // sets the flip-flop buffers accordingly.
    bool Execute (uint64_t iteration, int numGaussSeidel);

protected:
    // Support for construction.
    bool CreateGraphicsObjects (const GLchar* declarations,
        const GLchar* equation);
    void SetShaderConstants (float dt, float dx);
    void SetInitialValues (const Image1<float>* initial);
    void SetMaskValues (const Image1<unsigned char>* domain);

    // Overrides to be specialized by derived classes.  Return 'true' to
    // continue the solver, 'false' to terminate the solver.
    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    int mDimension, mNumTexels;
    GLuint mVertexBuffer, mVertexShader, mFragmentShader, mProgram;
    GLuint mModelPositionAttribute;
    GLint mU0SamplerLocation, mU1SamplerLocation, mMaskSamplerLocation;
    GLint mDeltaLocation, mCoeffLocation;
    GLuint mTexture[3], mFrameBuffer[3], mMaskTexture;
    float mDelta[2], mCoeff[3];
    int mActive[3];

private:
    static const float msSquare[4][2];
    static const GLchar* msVertexText;
    static const GLchar* msFragmentDeclareText;
    static const GLchar* msFragmentSamplerText;
    static const GLchar* msFragmentResultText;
};

#endif
