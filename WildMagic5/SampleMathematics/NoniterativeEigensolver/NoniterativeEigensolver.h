// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef NONITERATIVEEIGENSOLVER_H
#define NONITERATIVEEIGENSOLVER_H

#include "Wm5ConsoleApplication.h"
using namespace Wm5;

class NoniterativeEigensolver : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    NoniterativeEigensolver ();

    virtual int Main (int numArguments, char** arguments);

private:
    void SolveAndMeasure (const Matrix3f& A, float& iterativeError,
        float& noniterativeError, float& iterativeDeterminant,
        float& noniterativeDeterminant);

    // Data for iterative method.
    float mIterativeEigenvalues[3];
    Vector3f mIterativeEigenvectors[3];

    // Data for noniterative method.
    float mNoniterativeEigenvalues[3];
    Vector3f mNoniterativeEigenvectors[3];
};

WM5_REGISTER_INITIALIZE(NoniterativeEigensolver);
WM5_REGISTER_TERMINATE(NoniterativeEigensolver);

#endif
