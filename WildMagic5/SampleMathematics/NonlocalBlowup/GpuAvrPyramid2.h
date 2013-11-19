// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPUAVRPYRAMID2_H
#define GPUAVRPYRAMID2_H

#include "GpuPyramid2.h"

class GpuAvrPyramid2 : public GpuPyramid2
{
public:
    // Construction and destruction.  The return 'success' is 'true' iff the
    // solver is successfully created.
    GpuAvrPyramid2 (int dimension0, int dimension1,
        const Image2<float>& initial, bool& success);

    GpuAvrPyramid2 (int dimension0, int dimension1, GLuint texture0,
        GLuint texture1, GLuint frameBuffer0, GLuint frameBuffer1,
        bool& success);

    virtual ~GpuAvrPyramid2 ();

    // Member access.
    float GetAverage () const;

protected:
    // Read back the texel from the largest level.
    virtual bool OnPostDraw (int level, int texture, int frameBuffer);

    float mAverage;
    static const GLchar* msQuadAverage;
};

#endif
