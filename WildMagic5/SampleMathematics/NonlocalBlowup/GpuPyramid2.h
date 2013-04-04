// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPUPYRAMID2_H
#define GPUPYRAMID2_H

#include "OpenGLHelper.h"
#include "Image2.h"

class GpuPyramid2
{
protected:
    // Abstract base class.  The return 'success' is 'true' iff the solver is
    // successfully created.
    GpuPyramid2 (const GLchar* declarations, const GLchar* equation,
        int dimension0, int dimension1, const Image2<float>& initial,
        bool& success);

    GpuPyramid2 (const GLchar* declarations, const GLchar* equation,
        int dimension0, int dimension1, GLuint texture0, GLuint texture1,
        GLuint frameBuffer0, GLuint frameBuffer1, bool& success);

public:
    virtual ~GpuPyramid2 ();

    // Execute the pyramid scheme.
    bool Execute ();

    // Choose different textures/framebuffers to use during Execute().  This
    // is successful only when the second constructor is used.
    bool Use (GLuint texture0, GLuint texture1, GLuint frameBuffer0,
        GLuint frameBuffer1);

protected:
    // Support for construction.
    bool CreateGraphicsObjects (const GLchar* declarations,
        const GLchar* equation);
    void SetInitialValues (const Image2<float>& initial);

    // Overrides to be specialized by derived classes.
    virtual bool OnPreIteration ();
    virtual bool OnPostIteration ();
    virtual bool OnPreDraw (int level, int texture, int frameBuffer);
    virtual bool OnPostDraw (int level, int texture, int frameBuffer);

    int mDimension[2], mNumTexels, mNumLevels;
    GLuint mVertexBuffer, mVertexShader, mFragmentShader, mProgram;
    GLuint mModelPositionAttribute;
    GLint mRSamplerLocation;
    GLint mDeltaLocation, mMultiplierLocation, mOffsetLocation;
    GLuint mTexture[2], mFrameBuffer[2];
    float mDelta[2], mMultiplier[2];
    int mSize[2];
    bool mOwner;

    static const float msSquare[4][2];
    static const GLchar* msVertexText;
    static const GLchar* msFragmentDeclareText;
    static const GLchar* msFragmentSamplerText;
    static const GLchar* msFragmentResultText;
};

#endif
