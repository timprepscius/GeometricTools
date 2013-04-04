// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPUMAXPYRAMID2_H
#define GPUMAXPYRAMID2_H

#include "GpuPyramid2.h"

class GpuMaxPyramid2 : public GpuPyramid2
{
public:
    // Construction and destruction.  The return 'success' is 'true' iff the
    // solver is successfully created.
    GpuMaxPyramid2 (int dimension0, int dimension1,
        const Image2<float>& initial, bool& success);

    GpuMaxPyramid2 (int dimension0, int dimension1, GLuint texture0,
        GLuint texture1, GLuint frameBuffer0, GLuint frameBuffer1,
        bool& success);

    virtual ~GpuMaxPyramid2 ();

    // Member access.
    float GetMaximum () const;

protected:
    // Read back the texel from the largest level.
    virtual bool OnPostDraw (int level, int texture, int frameBuffer);

    float mMaximum;
    static const GLchar* msQuadMaximum;
};

#endif
