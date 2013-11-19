// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef GPULOCALSOLVER1_H
#define GPULOCALSOLVER1_H

#include "GpuPdeSolver1.h"

class GpuLocalSolver1 : public GpuPdeSolver1
{
public:
    GpuLocalSolver1 (int dimension, const Image1<float>* initial,
        const Image1<unsigned char>* domain, float dt, float dx,
        const std::string& folder, bool& success);

    virtual ~GpuLocalSolver1 ();

private:
    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float* mReadBack;
    std::string mFolder;

    static const GLchar* msDeclarations;
    static const GLchar* msEquation;
};

#endif
