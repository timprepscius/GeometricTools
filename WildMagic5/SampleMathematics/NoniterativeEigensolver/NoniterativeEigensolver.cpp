// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "NoniterativeEigensolver.h"

WM5_CONSOLE_APPLICATION(NoniterativeEigensolver);

// Enable only one of these.  You should run all of these in release mode.
#define MEASURE_NONITERATIVE
//#define TIMING_NONITERATIVE
//#define TIMING_ITERATIVE

//----------------------------------------------------------------------------
NoniterativeEigensolver::NoniterativeEigensolver ()
    :
    ConsoleApplication("SampleMathematics/NoniterativeEigensolver")
{
}
//----------------------------------------------------------------------------
int NoniterativeEigensolver::Main (int, char**)
{
#ifdef MEASURE_NONITERATIVE
    float maxIterativeError = 0.0f;
    float maxNoniterativeError = 0.0f;
    int iterativeErrorIndex = -1;
    int noniterativeErrorIndex = -1;
    float minIterativeDeterminant = 1.0f;
    float minNoniterativeDeterminant = 1.0f;
    int iterativeDeterminantIndex = -1;
    int noniterativeDeterminantIndex = -1;
    const int imax = (1 << 28);
#endif

#if defined(TIMING_ITERATIVE) || defined(TIMING_NONITERATIVE)
    const int imax = (1 << 24);
    clock_t startTime = clock();
#endif

    for (int i = 0, type = 0; i < imax; ++i)
    {
        Vector3f axis = Vector3f(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(), Mathf::SymmetricRandom());
        axis.Normalize();
        float angle = Mathf::IntervalRandom(0.0f, Mathf::PI);
        Matrix3f rotate(axis, angle);
        float d0, d1, d2;

        if (type == 0)
        {
            // d0 = d1 = d2
            d1 = Mathf::IntervalRandom(-1.0f, 1.0f);
            d0 = d1;
            d2 = d1;
        }
        else if (type == 1)
        {
            // d0 = d1 < d2
            d1 = Mathf::IntervalRandom(-1.0f, 1.0f);
            d0 = d1;
            d2 = Mathf::IntervalRandom(0.5f*(d1 + 1.0f), 1.0f);
        }
        else if (type == 2)
        {
            // d0 < d1 = d2
            d1 = Mathf::IntervalRandom(-1.0f, 1.0f);
            d0 = Mathf::IntervalRandom(-1.0f, 0.5f*(d1 - 1.0f));
            d2 = d1;
        }
        else
        {
            // d0 < d1 < d2
            d1 = Mathf::IntervalRandom(-1.0f, 1.0f);
            d0 = Mathf::IntervalRandom(-1.0f, 0.5f*(d1 - 1.0f));
            d2 = Mathf::IntervalRandom(0.5f*(d1 + 1.0f), 1.0f);
        }

        Matrix3f A = rotate.TransposeTimes(Matrix3f(d0, d1, d2)*rotate);

#ifdef MEASURE_NONITERATIVE
        float iterativeError, noniterativeError;
        float iterativeDeterminant, noniterativeDeterminant;
        SolveAndMeasure(A, iterativeError, noniterativeError,
            iterativeDeterminant, noniterativeDeterminant);

        if (iterativeError > maxIterativeError)
        {
            maxIterativeError = iterativeError;
            iterativeErrorIndex = i;
        }

        if (noniterativeError > maxNoniterativeError)
        {
            maxNoniterativeError = noniterativeError;
            noniterativeErrorIndex = i;
        }

        if (iterativeDeterminant < minIterativeDeterminant)
        {
            minIterativeDeterminant = iterativeDeterminant;
            iterativeDeterminantIndex = i;
        }

        if (noniterativeDeterminant < minNoniterativeDeterminant)
        {
            minNoniterativeDeterminant = noniterativeDeterminant;
            noniterativeDeterminantIndex = i;
        }
#endif

#ifdef TIMING_ITERATIVE
        EigenDecompositionf iterativeSolver(A);
        iterativeSolver.Solve(true);
#endif

#ifdef TIMING_NONITERATIVE
        NoniterativeEigen3x3f noniterativeSolve(A);
#endif

        if (++type == 4)
        {
            type = 0;
        }
    }

#ifdef MEASURE_NONITERATIVE
    std::ofstream measureOutFile("measures.txt");
    measureOutFile << "iterations = "
        << imax << std::endl;
    measureOutFile << "max iterative error = "
        << maxIterativeError << std::endl;
    measureOutFile << "max noniterative error = "
        << maxNoniterativeError << std::endl;
    measureOutFile << "min iterative determinant = "
        << minIterativeDeterminant << std::endl;
    measureOutFile << "min noniterative determinant = "
        << minNoniterativeDeterminant << std::endl;
    measureOutFile.close();
#endif

#if defined(TIMING_ITERATIVE) || defined(TIMING_NONITERATIVE)
    clock_t finalTime = clock();
    long diffTime = finalTime - startTime;
#endif

#ifdef TIMING_ITERATIVE
    std::ofstream iterativeOutFile("iterative.txt");
    iterativeOutFile << "ticks = " << diffTime << std::endl;
    iterativeOutFile.close();
#endif

#ifdef TIMING_NONITERATIVE
    std::ofstream noniterativeOutFile("noniterative.txt");
    noniterativeOutFile << "ticks = " << diffTime << std::endl;
    noniterativeOutFile.close();
#endif

    return 0;
}
//----------------------------------------------------------------------------
void NoniterativeEigensolver::SolveAndMeasure (const Matrix3f& A,
    float& iterativeError, float& noniterativeError,
    float& iterativeDeterminant, float& noniterativeDeterminant)
{
    int i;
    Vector3f result;
    float length;
    iterativeError = 0.0f;
    noniterativeError = 0.0f;

    // Iterative eigensolver.
    EigenDecompositionf iterativeSolver(A);
    iterativeSolver.Solve(true);
    for (i = 0; i < 3; ++i)
    {
        mIterativeEigenvalues[i] = iterativeSolver.GetEigenvalue(i);
        mIterativeEigenvectors[i] = iterativeSolver.GetEigenvector3(i);
        result = A*mIterativeEigenvectors[i] -
            mIterativeEigenvalues[i]*mIterativeEigenvectors[i];
        length = result.Length();
        if (length > iterativeError)
        {
            iterativeError = length;
        }
    }
    iterativeDeterminant = Mathf::FAbs(
        mIterativeEigenvectors[0].Dot(mIterativeEigenvectors[1].Cross(
        mIterativeEigenvectors[2])));

    // Bounded-time eigensolver.
    NoniterativeEigen3x3f noniterativeSolver(A);
    for (i = 0; i < 3; ++i)
    {
        mNoniterativeEigenvalues[i] = noniterativeSolver.GetEigenvalue(i);
        mNoniterativeEigenvectors[i] = noniterativeSolver.GetEigenvector(i);
        result = A*mNoniterativeEigenvectors[i] -
            mNoniterativeEigenvalues[i]*mNoniterativeEigenvectors[i];
        length = result.Length();
        if (length > noniterativeError)
        {
            noniterativeError = length;
        }
    }
    noniterativeDeterminant = Mathf::FAbs(
        mNoniterativeEigenvectors[0].Dot(mNoniterativeEigenvectors[1].Cross(
        mNoniterativeEigenvectors[2])));
}
//----------------------------------------------------------------------------
