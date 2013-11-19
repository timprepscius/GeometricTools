// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SMOKE2D_H
#define SMOKE2D_H

#include "Wm5Graphics.h"
#include "Wm5Renderer.h"
using namespace Wm5;

class Smoke2D
{
public:
    // Construction and destruction.
    Smoke2D (Renderer* renderer, float x0, float y0, float x1, float y1,
        float dt, float denViscosity, float velViscosity, int imax, int jmax,
        int numGaussSeidelIterations, int numVortices);

    ~Smoke2D ();

    // Member access.
    inline float GetX0 () const;
    inline float GetY0 () const;
    inline float GetX1 () const;
    inline float GetY1 () const;
    inline float GetDt () const;
    inline float GetDx () const;
    inline float GetDy () const;
    inline float GetTime () const;
    inline int GetIMax () const;
    inline int GetJMax () const;

    void CreateScene ();
    void Initialize ();
    void DoSimulationStep ();

    // Member access.
    inline int GetNumVortices () const;
    inline Vector2f GetVortexCenter (int i) const;
    inline float GetVortexVariance (int i) const;
    inline float GetVortexAmplitude (int i) const;

    // Dynamic adjustment of vortices.
    inline int GetNumActiveVortices () const;
    void SetNumActiveVortices (int numActive);

    // Dynamic adjustment of gravity.
    inline float GetGravity () const;
    void SetGravity (float gravity);

    // Switch colors.
    void UseColor (bool useColor);

    // Toggle vortex overlay.
    void UseVortexOverlay (bool useVortexOverlay);

private:
    void ComputeSource ();
    void ComputeVortexOverlay ();

    void CreateBoundaryMixedEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreateCopyStateEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreateFluidUpdateEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreateDivergenceEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreatePoissonSolverEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreateAdjustVelocityEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    void CreateDrawDensityEffect (VisualEffect*& effect,
        VisualEffectInstance*& instance);

    inline int Index (int x, int y) const { return x + mIMaxP1*y; }

    // Constructor inputs.
    float mX0, mY0, mX1, mY1;
    float mDt;
    float mDenViscosity, mVelViscosity;
    int mIMax, mJMax;
    int mNumGaussSeidelIterations;  // must be even

    // Derived quantities;
    int mIMaxM1, mJMaxM1, mIMaxP1, mJMaxP1, mNumPixels;
    float mDx, mDy;
    float mDxDx, mDyDy;
    float mHalfDivDx, mHalfDivDy;
    float mDtDivDx, mDtDivDy, mDtDivDxDx, mDtDivDyDy;
    float mEpsilon0, mEpsilonX, mEpsilonY;
    float mDenLambdaX, mDenLambdaY;
    float mVelLambdaX, mVelLambdaY;
    float mDenGamma0, mDenGammaX, mDenGammaY;
    float mVelGamma0, mVelGammaX, mVelGammaY;

    // Current simulation time.
    float mTime;

    // Smoke-specific items.
    int mNumVortices;
    int mNumActive;
    Vector2f* mVortexCenter;
    float* mVortexVariance;
    float* mVortexAmplitude;

    // Time-invariant portion of density source.
    float** mTimelessDensity;

    // Time-invariant portion of velocity source.
    Vector2f*** mTimelessVortex;
    Vector2f** mTimelessWind;
    Vector2f** mTimelessVelocity;

    // Gravitational force in y-direction.
    Vector2f mGravity;

    // Support for GPU rendering of fluids.
    Renderer* mRenderer;
    ImageProcessing2* mIP;

    VisualEffectPtr mBoundaryDirichletEffect;
    VisualEffectInstancePtr mBoundaryDirichletInstance;

    VisualEffectPtr mBoundaryMixedEffect;
    VisualEffectInstancePtr mBoundaryMixedInstance;

    VisualEffectPtr mBoundaryNeumannEffect;
    VisualEffectInstancePtr mBoundaryNeumannInstance;

    VisualEffectPtr mCopyStateEffect;
    VisualEffectInstancePtr mCopyStateInstance;
    Texture2DPtr mInitialTexture;

    VisualEffectPtr mFluidUpdateEffect;
    VisualEffectInstancePtr mFluidUpdateInstance;
    Texture2DPtr mSourceTexture;

    VisualEffectPtr mDivergenceEffect;
    VisualEffectInstancePtr mDivergenceInstance;

    VisualEffectPtr mPoissonSolverEffect;
    VisualEffectInstancePtr mPoissonSolverInstance;

    VisualEffectPtr mAdjustVelocityEffect;
    VisualEffectInstancePtr mAdjustVelocityInstance;

    VisualEffectPtr mDrawDensityEffect;
    VisualEffectInstancePtr mDrawDensityInstance;
    Texture1DPtr mGrayTexture;
    Texture1DPtr mColorTexture;
    Texture2DPtr mVortexTexture;

    bool mUseVortexOverlay;

    // boundary mixed effect
    static int msAllBoundaryMixedPTextureUnits[3];
    static int* msBoundaryMixedPTextureUnits[Shader::MAX_PROFILES];
    static std::string msBoundaryMixedPPrograms[Shader::MAX_PROFILES];

    // copy state effect
    static int msAllCopyStatePTextureUnits[1];
    static int* msCopyStatePTextureUnits[Shader::MAX_PROFILES];
    static std::string msCopyStatePPrograms[Shader::MAX_PROFILES];

    // fluid update effect
    static int msAllFluidUpdatePRegisters[3];
    static int* msFluidUpdatePRegisters[Shader::MAX_PROFILES];
    static int msAllFluidUpdatePTextureUnits[3];
    static int* msFluidUpdatePTextureUnits[Shader::MAX_PROFILES];
    static std::string msFluidUpdatePPrograms[Shader::MAX_PROFILES];

    // divergence effect
    static int msAllDivergencePRegisters[1];
    static int* msDivergencePRegisters[Shader::MAX_PROFILES];
    static int msAllDivergencePTextureUnits[1];
    static int* msDivergencePTextureUnits[Shader::MAX_PROFILES];
    static std::string msDivergencePPrograms[Shader::MAX_PROFILES];

    // Poisson solver effect
    static int msAllPoissonSolverPRegisters[2];
    static int* msPoissonSolverPRegisters[Shader::MAX_PROFILES];
    static int msAllPoissonSolverPTextureUnits[2];
    static int* msPoissonSolverPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPoissonSolverPPrograms[Shader::MAX_PROFILES];

    // adjust velocity effect
    static int msAllAdjustVelocityPRegisters[1];
    static int* msAdjustVelocityPRegisters[Shader::MAX_PROFILES];
    static int msAllAdjustVelocityPTextureUnits[2];
    static int* msAdjustVelocityPTextureUnits[Shader::MAX_PROFILES];
    static std::string msAdjustVelocityPPrograms[Shader::MAX_PROFILES];

    // draw density effect
    static int msAllDrawDensityPTextureUnits[3];
    static int* msDrawDensityPTextureUnits[Shader::MAX_PROFILES];
    static std::string msDrawDensityPPrograms[Shader::MAX_PROFILES];
};

#include "Smoke2D.inl"

#endif
