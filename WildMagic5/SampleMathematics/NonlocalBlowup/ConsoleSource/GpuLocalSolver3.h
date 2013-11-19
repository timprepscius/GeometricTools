// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPULOCALSOLVER3_H
#define GPULOCALSOLVER3_H

#include "GpuPdeSolver3.h"

class GpuLocalSolver3 : public GpuPdeSolver3
{
public:
    GpuLocalSolver3 (int dimension0, int dimension1, int dimension2,
        const Image3<float>* initial, const Image3<unsigned char>* domain,
        float dt, float dx0, float dx1, float dx2, const std::string& folder,
        bool& success);

    virtual ~GpuLocalSolver3 ();

private:
    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float* mReadBack;
    float* mSlice;
    std::string mFolder;

    static const GLchar* msDeclarations;
    static const GLchar* msEquation;
};

#endif
