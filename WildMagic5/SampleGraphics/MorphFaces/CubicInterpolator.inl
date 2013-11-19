// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

//----------------------------------------------------------------------------
template <int N, typename Real>
CubicInterpolator<N,Real>::CubicInterpolator (int numSamples, Real* inputs,
    Tuple<N,Real>* outputs, Tuple<N,Real>* tangents0,
    Tuple<N,Real>* tangents1, bool ownerInputs, bool ownerOutputs,
    bool ownerTangents0, bool ownerTangents1)
    :
    mNumSamples(numSamples),
    mInputs(inputs),
    mOutputs(outputs),
    mTangents0(tangents0),
    mTangents1(tangents1),
    mOwnerInputs(ownerInputs),
    mOwnerOutputs(ownerOutputs),
    mOwnerTangents0(ownerTangents0),
    mOwnerTangents1(ownerTangents1),
    mNumSamplesM1(numSamples - 1),
    mC0(outputs),
    mC1(tangents0),
    mLastIndex(0)
{
    assertion(mNumSamples >= 2,
        "Interpolation requires two or more samples.\n");

    assertion(
        mInputs != 0 && mOutputs != 0 && mTangents0 != 0 && mTangents1 != 0,
        "Input arrays must be nonnull.\n");

    Initialize();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
CubicInterpolator<N,Real>::CubicInterpolator (const std::string& filename)
    :
    mNumSamples(0),
    mInputs(0),
    mOutputs(0),
    mTangents0(0),
    mTangents1(0),
    mOwnerInputs(true),
    mOwnerOutputs(true),
    mOwnerTangents0(true),
    mOwnerTangents1(true),
    mNumSamplesM1(0),
    mC0(0),
    mC1(0),
    mLastIndex(0)
{
    std::ifstream inFile(filename.c_str());
    if (!inFile)
    {
        assertion(false, "Failed to open %s\n", filename.c_str());
        return;
    }

    inFile >> mNumSamples;
    assertion(mNumSamples >= 2,
        "Interpolation requires two or more samples.\n");

    mNumSamplesM1 = mNumSamples - 1;

    mInputs = new1<Real>(mNumSamples);
    mOutputs = new1<Tuple<N,Real> >(mNumSamples);
    mTangents0 = new1<Tuple<N,Real> >(mNumSamples);
    mTangents1 = new1<Tuple<N,Real> >(mNumSamples);
    mC0 = mOutputs;
    mC1 = mTangents0;

    int i, j;
    for (i = 0; i < mNumSamples; ++i)
    {
        inFile >> mInputs[i];
    }

    for (i = 0; i < mNumSamples; ++i)
    {
        Tuple<N,Real>& output = mOutputs[i];
        for (j = 0; j < N; ++j)
        {
            inFile >> output[j];
        }
    }

    for (i = 0; i < mNumSamples; ++i)
    {
        Tuple<N,Real>& tangent0 = mTangents0[i];
        for (j = 0; j < N; ++j)
        {
            inFile >> tangent0[j];
        }
    }

    for (i = 0; i < mNumSamples; ++i)
    {
        Tuple<N,Real>& tangent1 = mTangents1[i];
        for (j = 0; j < N; ++j)
        {
            inFile >> tangent1[j];
        }
    }

    Initialize();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
CubicInterpolator<N,Real>::~CubicInterpolator ()
{
    if (mOwnerInputs)
    {
        delete1(mInputs);
    }
    if (mOwnerOutputs)
    {
        delete1(mOutputs);
    }
    if (mOwnerTangents0)
    {
        delete1(mTangents0);
    }
    if (mOwnerTangents1)
    {
        delete1(mTangents1);
    }

    delete1(mInvDeltas);
    delete1(mC2);
    delete1(mC3);
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline int CubicInterpolator<N,Real>::GetNumSamples () const
{
    return mNumSamples;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real* CubicInterpolator<N,Real>::GetInputs () const
{
    return mInputs;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real* CubicInterpolator<N,Real>::GetOutputs () const
{
    return mOutputs;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real* CubicInterpolator<N,Real>::GetTangents0 () const
{
    return mTangents0;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real* CubicInterpolator<N,Real>::GetTangents1 () const
{
    return mTangents1;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real CubicInterpolator<N,Real>::GetMinInput () const
{
    return mInputs[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
inline const Real CubicInterpolator<N,Real>::GetMaxInput () const
{
    return mInputs[mNumSamples - 1];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Tuple<N,Real> CubicInterpolator<N,Real>::operator() (Real input) const
{
    Real u;
    int k;
    GetKeyInfo(input, u, k);

    Tuple<N,Real> output;
    for (int j = 0; j < N; ++j)
    {
        output[j] = mC0[k][j] + u*(mC1[k][j] + u*(mC2[k][j] + u*mC3[k][j]));
    }
    return output;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void CubicInterpolator<N,Real>::Initialize ()
{
    mInvDeltas = new1<Real>(mNumSamplesM1);
    mC2 = new1<Tuple<N,Real> >(mNumSamplesM1);
    mC3 = new1<Tuple<N,Real> >(mNumSamplesM1);

    for (int i0 = 0, i1 = 1; i1 < mNumSamples; ++i0, ++i1)
    {
        Real delta = mInputs[i1] - mInputs[i0];
        assertion(delta > (Real)0,
            "The inputs must be strictly increasing.\n");

        mInvDeltas[i0] = ((Real)1)/delta;
        for (int j = 0; j < N; ++j)
        {
            Real diff0 = mOutputs[i1][j] - mOutputs[i0][j];
            Real diff1 = mTangents1[i0][j] - mTangents0[i0][j];
            Real tmp0 = diff0 - delta*mTangents0[i0][j];
            Real tmp1 = delta*diff1;
            mC2[i0][j] = (((Real)3)*tmp0 - tmp1);
            mC3[i0][j] = (tmp1 - ((Real)2)*tmp0);
        }
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void CubicInterpolator<N,Real>::GetKeyInfo (Real input, Real& normInput,
    int& key) const
{
    if (input <= mInputs[0])
    {
        normInput = (Real)0;
        mLastIndex = 0;
        key = 0;
        return;
    }

    if (input >= mInputs[mNumSamplesM1])
    {
        normInput = (Real)1;
        mLastIndex = mNumSamplesM1 - 1;
        key = mLastIndex;
        return;
    }

    int nextIndex;
    if (input > mInputs[mLastIndex])
    {
        nextIndex = mLastIndex + 1;
        while (input >= mInputs[nextIndex])
        {
            mLastIndex = nextIndex;
            ++nextIndex;
        }

        key = mLastIndex;
        normInput = (input - mInputs[key])*mInvDeltas[key];
    }
    else if (input < mInputs[mLastIndex])
    {
        nextIndex = mLastIndex - 1;
        while (input <= mInputs[nextIndex])
        {
            mLastIndex = nextIndex;
            --nextIndex;
        }

        key = nextIndex;
        normInput = (input - mInputs[key])*mInvDeltas[key];
    }
    else
    {
        normInput = (Real)0;
        key = mLastIndex;
    }
}
//----------------------------------------------------------------------------
