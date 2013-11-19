// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef CPULOCALSOLVER2_H
#define CPULOCALSOLVER2_H

#include "CpuPdeSolver2.h"

class CpuLocalSolver2 : public CpuPdeSolver2<float>
{
public:
    CpuLocalSolver2 (int dimension0, int dimension1,
        const Image2<float>* initial, const Image2<unsigned char>* domain,
        float dt, float dx0, float dx1, const std::string& folder,
        bool& success);

    virtual ~CpuLocalSolver2 ();

private:
    void GetIntegral (float& umax, float& integral);

    virtual float Equation (int i0, int i1, const Image2<float>& u0,
        const Image2<float>& u1);

    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float* mReadBack;
    float* mSlice;
    std::string mFolder;
};

#endif
