// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPULOCALSOLVER2_H
#define GPULOCALSOLVER2_H

#include "GpuPdeSolver2.h"

class GpuLocalSolver2 : public GpuPdeSolver2
{
public:
    GpuLocalSolver2 (int dimension0, int dimension1,
        const Image2<float>* initial, const Image2<unsigned char>* domain,
        float dt, float dx0, float dx1, const std::string& folder,
        bool& success);

    virtual ~GpuLocalSolver2 ();

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
