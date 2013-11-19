// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef CPULOCALSOLVER3_H
#define CPULOCALSOLVER3_H

#include "CpuPdeSolver3.h"

class CpuLocalSolver3 : public CpuPdeSolver3<float>
{
public:
    CpuLocalSolver3 (int dimension0, int dimension1, int dimension2,
        const Image3<float>* initial, const Image3<unsigned char>* domain,
        float dt, float dx0, float dx1, float dx2, const std::string& folder,
        bool& success);

    virtual ~CpuLocalSolver3 ();

private:
    void GetIntegral (float& umax, float& integral);

    virtual float Equation (int i0, int i1, int i2, const Image3<float>& u0,
        const Image3<float>& u1);

    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float* mReadBack;
    float* mSlice;
    std::string mFolder;
};

#endif
