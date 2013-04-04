// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef NONLOCALSOLVER2_H
#define NONLOCALSOLVER2_H

#include "GpuPdeSolver2.h"
#include "GpuMaxPyramid2.h"
#include "GpuEvaluate2.h"
#include "GpuAvrPyramid2.h"

class NonlocalSolver2 : public GpuPdeSolver2
{
public:
    NonlocalSolver2 (int dimension0, int dimension1,
        const Image2<float>* initial, const Image2<unsigned char>* domain,
        float dt, float dx0, float dx1, float p, bool& success);

    virtual ~NonlocalSolver2 ();

    float* GetReadBack () { return mReadBack; }

private:
    class Evaluator : public GpuEvaluate2
    {
    public:
        Evaluator (int dimension0, int dimension1, bool& success);
        virtual ~Evaluator ();

        void SetUMax (float umax);

    protected:
        virtual bool OnPreEvaluation (GLuint texture, GLuint frameBuffer);

        GLint mUMaxLocation;
        float mUMax;

        static const GLchar* msDeclarations;
        static const GLchar* msEquation;
    };

    virtual bool OnPreIteration (uint64_t iteration);
    virtual bool OnPostIteration (uint64_t iteration);

    float mPower;
    float* mReadBack;
    float mNonlinear0, mNonlinear1;
    GLint mNonlinearLocation;
    GpuMaxPyramid2 mMaxPyramid;
    Evaluator mEvaluator;
    GpuAvrPyramid2 mAvrPyramid;

    static const GLchar* msDeclarations;
    static const GLchar* msEquation;
};

#endif
