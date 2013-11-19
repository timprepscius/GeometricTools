// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef CPUNONLOCALSOLVER3_H
#define CPUNONLOCALSOLVER3_H

#include "CpuPdeSolver3.h"

class CpuNonlocalSolver3 : public CpuPdeSolver3<float>
{
public:
    CpuNonlocalSolver3 (int dimension0, int dimension1, int dimension2,
        const Image3<float>* initial, const Image3<unsigned char>* domain,
        float dt, float dx0, float dx1, float dx2, float p,
        const std::string& folder, bool& success);

    virtual ~CpuNonlocalSolver3 ();

private:
    void GetIntegral (float& umax, float& integral);

    virtual float Equation (int i0, int i1, int i2, const Image3<float>& u0,
        const Image3<float>& u1);

    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float mPower;
    float mNonlinear0, mNonlinear1;
    float* mReadBack;
    float* mSlice;
    std::string mFolder;
};

#endif
