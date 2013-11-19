// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Smoke2D.h"

//----------------------------------------------------------------------------
Smoke2D::Smoke2D (Renderer* renderer, float x0, float y0, float x1, float y1,
    float dt, float denViscosity, float velViscosity, int imax, int jmax,
    int numGaussSeidelIterations, int numVortices)
    :
    mX0(x0),
    mY0(y0),
    mX1(x1),
    mY1(y1),
    mDt(dt),
    mDenViscosity(denViscosity),
    mVelViscosity(velViscosity),
    mIMax(imax),
    mJMax(jmax),
    mNumGaussSeidelIterations(numGaussSeidelIterations),
    mNumVortices(numVortices),
    mNumActive(0),
    mGravity(Vector2f::ZERO),
    mRenderer(renderer),
    mIP(0),
    mUseVortexOverlay(false)
{
    mIMaxM1 = mIMax - 1;
    mJMaxM1 = mJMax - 1;
    mIMaxP1 = mIMax + 1;
    mJMaxP1 = mJMax + 1;
    mNumPixels = mIMaxP1*mJMaxP1;
    mDx = (mX1 - mX0)/(float)mIMax;
    mDy = (mY1 - mY0)/(float)mJMax;
    mDxDx = mDx*mDx;
    mDyDy = mDy*mDy;
    mHalfDivDx = 0.5f/mDx;
    mHalfDivDy = 0.5f/mDy;
    mDtDivDx = mDt/mDx;
    mDtDivDy = mDt/mDy;
    mDtDivDxDx = mDt/mDxDx;
    mDtDivDyDy = mDt/mDyDy;
    mEpsilon0 = 0.5f*mDxDx*mDyDy/(mDxDx + mDyDy);
    mEpsilonX = mEpsilon0/mDxDx;
    mEpsilonY = mEpsilon0/mDyDy;
    mDenLambdaX = mDenViscosity*mDtDivDxDx;
    mDenLambdaY = mDenViscosity*mDtDivDyDy;
    mVelLambdaX = mVelViscosity*mDtDivDxDx;
    mVelLambdaY = mVelViscosity*mDtDivDyDy;
    mDenGamma0 = 1.0f/(1.0f + 2.0f*(mDenLambdaX + mDenLambdaY));
    mDenGammaX = mDenLambdaX*mDenGamma0;
    mDenGammaY = mDenLambdaY*mDenGamma0;
    mVelGamma0 = 1.0f/(1.0f + 2.0f*(mVelLambdaX + mVelLambdaY));
    mVelGammaX = mVelLambdaX*mVelGamma0;
    mVelGammaY = mVelLambdaY*mVelGamma0;
    mTime = 0.0f;

    mVortexCenter = new1<Vector2f>(mNumVortices);
    mVortexVariance = new1<float>(mNumVortices);
    mVortexAmplitude = new1<float>(mNumVortices);
    mTimelessDensity = new2<float>(mIMaxP1, mJMaxP1);
    mTimelessVortex = new3<Vector2f>(mNumVortices, mIMaxP1, mJMaxP1);
    mTimelessWind = new2<Vector2f>(mIMaxP1, mJMaxP1);
    mTimelessVelocity = new2<Vector2f>(mIMaxP1, mJMaxP1);

    int v;
    for (v = 0; v < mNumVortices; ++v)
    {
        mVortexCenter[v][0] = Mathf::UnitRandom();
        mVortexCenter[v][1] = Mathf::UnitRandom();
        mVortexVariance[v] = Mathf::IntervalRandom(0.001f, 0.01f);
        mVortexAmplitude[v] = Mathf::IntervalRandom(128.0f, 256.0f);
        if (Mathf::SymmetricRandom() < 0.0f)
        {
            mVortexAmplitude[v] *= -1.0f;
        }
    }

    for (int j = 0; j <= mJMax; ++j)
    {
        float y = mY0 + mDy*(float)j;

        for (int i = 0; i <= mIMax; ++i)
        {
            float x = mX0 + mDx*(float)i;

            // density source
            float dx = x - 0.25f;
            float dy = y - 0.75f;
            float arg = -(dx*dx + dy*dy)/0.01f;
            mTimelessDensity[j][i] = 2.0f*Mathf::Exp(arg);

            // density sink
            dx = x - 0.75f;
            dy = y - 0.25f;
            arg = -(dx*dx + dy*dy)/0.01f;
            mTimelessDensity[j][i] -= 2.0f*Mathf::Exp(arg);

            // velocity vortex source
            for (v = 0; v < mNumVortices; ++v)
            {
                dx = x - mVortexCenter[v][0];
                dy = y - mVortexCenter[v][1];
                arg = -(dx*dx + dy*dy)/mVortexVariance[v];
                Vector2f vortex(dy, -dx);
                vortex *= mVortexAmplitude[v]*Mathf::Exp(arg);
                mTimelessVortex[j][i][v] = vortex;
            }

            // velocity wind source
            float diff = y - 0.5f;
            float ampl = 32.0f*Mathf::Exp(-diff*diff/0.001f);
            Vector2f wind(ampl, 0.0f);
            mTimelessWind[j][i] = wind;
            mTimelessVelocity[j][i] = wind;
        }
    }
}
//----------------------------------------------------------------------------
Smoke2D::~Smoke2D ()
{
    delete1(mVortexCenter);
    delete1(mVortexVariance);
    delete1(mVortexAmplitude);
    delete2(mTimelessDensity);
    delete3(mTimelessVortex);
    delete2(mTimelessWind);
    delete2(mTimelessVelocity);
    delete0(mIP);
}
//----------------------------------------------------------------------------
void Smoke2D::CreateScene ()
{
    mIP = new0 ImageProcessing2(mIMaxP1, mJMaxP1, 5);

    for (int i = 0; i < 5; ++i)
    {
        mRenderer->Bind(mIP->GetTarget(i));
    }

    VisualEffect* effect;
    VisualEffectInstance* instance;

    // Create the boundary effects.
    mIP->CreateBoundaryDirichletEffect(effect, instance);
    mBoundaryDirichletEffect = effect;
    mBoundaryDirichletInstance = instance;

    CreateBoundaryMixedEffect(effect, instance);
    mBoundaryMixedEffect = effect;
    mBoundaryMixedInstance = instance;

    mIP->CreateBoundaryNeumannEffect(effect, instance);
    mBoundaryNeumannEffect = effect;
    mBoundaryNeumannInstance = instance;

    // Create the effect for initialization and for copying the advection
    // texture.
    CreateCopyStateEffect(effect, instance);
    mCopyStateEffect = effect;
    mCopyStateInstance = instance;

    // Create the fluid update effect.
    CreateFluidUpdateEffect(effect, instance);
    mFluidUpdateEffect = effect;
    mFluidUpdateInstance = instance;

    // Create the divergence effect.
    CreateDivergenceEffect(effect, instance);
    mDivergenceEffect = effect;
    mDivergenceInstance = instance;

    // Create the Poisson solver effects.
    CreatePoissonSolverEffect(effect, instance);
    mPoissonSolverEffect = effect;
    mPoissonSolverInstance = instance;

    // Create the divergence effect.
    CreateAdjustVelocityEffect(effect, instance);
    mAdjustVelocityEffect = effect;
    mAdjustVelocityInstance = instance;

    // Create the draw-density effect.
    CreateDrawDensityEffect(effect, instance);
    mDrawDensityEffect = effect;
    mDrawDensityInstance = instance;
}
//----------------------------------------------------------------------------
void Smoke2D::Initialize ()
{
    TriMesh* square = mIP->GetRectangle();
    RenderTarget* target0 = mIP->GetTarget(0);
    RenderTarget* target1 = mIP->GetTarget(1);
    RenderTarget* target2 = mIP->GetTarget(2);
    RenderTarget* target3 = mIP->GetTarget(3);

    mBoundaryNeumannInstance->SetPixelTexture(0, "StateSampler",
        target0->GetColorTexture(0));

    mBoundaryDirichletInstance->SetPixelTexture(0, "StateSampler",
        target3->GetColorTexture(0));

    if (mRenderer->PreDraw())
    {
        mRenderer->SetCamera(mIP->GetCamera());

        // Set the initial data.
        mCopyStateInstance->SetPixelTexture(0, "StateSampler",
            mInitialTexture);
        square->SetEffectInstance(mCopyStateInstance);
        mRenderer->Enable(target2);
        mRenderer->Draw(square);  // in: InitialTexture
        mRenderer->Disable(target2);  // out: Target2
        mCopyStateInstance->SetPixelTexture(0, "StateSampler",
            target1->GetColorTexture(0));

        // Set the mixed boundary conditions.
        square->SetEffectInstance(mBoundaryMixedInstance);
        mRenderer->Enable(target1);
        mRenderer->Draw(square);  // in: Target2, MaskMixed, OffsetMixed
        mRenderer->Disable(target1);  // out: Target1

        mRenderer->PostDraw();
    }
}
//----------------------------------------------------------------------------
void Smoke2D::DoSimulationStep ()
{
    TriMesh* square = mIP->GetRectangle();
    RenderTarget* target0 = mIP->GetTarget(0);
    RenderTarget* target1 = mIP->GetTarget(1);
    RenderTarget* target2 = mIP->GetTarget(2);
    RenderTarget* target3 = mIP->GetTarget(3);
    RenderTarget* target4 = mIP->GetTarget(4);

    mRenderer->SetCamera(mIP->GetCamera());

    // Copy the current state for use in advection.
    square->SetEffectInstance(mCopyStateInstance);
    mRenderer->Enable(target4);
    mRenderer->Draw(square);  // in: Target1
    mRenderer->Disable(target4);  // out: Target4

    // Update the fluid.
    square->SetEffectInstance(mFluidUpdateInstance);
    mRenderer->Enable(target2);
    mRenderer->Draw(square);  // in: Target1, Target4, Source
    mRenderer->Disable(target2);  // out: Target2

    // Set the mixed boundary conditions.
    square->SetEffectInstance(mBoundaryMixedInstance);
    mRenderer->Enable(target1);
    mRenderer->Draw(square);  // in: Target2, MaskMixed, OffsetMixed
    mRenderer->Disable(target1);  // out: Target1

    // Compute the divergence.
    square->SetEffectInstance(mDivergenceInstance);
    mRenderer->Enable(target0);
    mRenderer->Draw(square);  // in: Target1
    mRenderer->Disable(target0);  // out: Target0

    // Set the Neumann boundary conditions.
    square->SetEffectInstance(mBoundaryNeumannInstance);
    mRenderer->Enable(target2);
    mRenderer->Draw(square);  // in: Target0, OffsetNeumann
    mRenderer->Disable(target2);  // out: Target2

    // Solve Poisson's equation for the divergence.
    mRenderer->Enable(target0);
    Float4 saveClearColor = mRenderer->GetClearColor();
    mRenderer->SetClearColor(Float4(0.0f, 0.0f, 0.0f, 0.0f));
    mRenderer->ClearColorBuffer();
    mRenderer->SetClearColor(saveClearColor);
    mRenderer->Disable(target0);
    for (int i = 0; i < mNumGaussSeidelIterations; ++i)
    {
        // Take one step of the Poisson solver.
        square->SetEffectInstance(mPoissonSolverInstance);
        mRenderer->Enable(target3);
        mRenderer->Draw(square);  // in: Target2, Target0
        mRenderer->Disable(target3);  // out: Target3

        // Set the Dirichlet boundary conditions.
        square->SetEffectInstance(mBoundaryDirichletInstance);
        mRenderer->Enable(target0);
        mRenderer->Draw(square);  // in: Target3, MaskDirichlet
        mRenderer->Disable(target0);  // out: Target0
    }

    // Adjust the velocity vectors.
    square->SetEffectInstance(mAdjustVelocityInstance);
    mRenderer->Enable(target2);
    mRenderer->Draw(square);  // in: Target0, mTarget1
    mRenderer->Disable(target2);  // out: Target2

    // Set the mixed boundary conditions.
    square->SetEffectInstance(mBoundaryMixedInstance);
    mRenderer->Enable(target1);
    mRenderer->Draw(square);  // in: Target2, MaskMixed, OffsetMixed
    mRenderer->Disable(target1);  // out: Target1

    // Draw the density.
    square->SetEffectInstance(mDrawDensityInstance);
    mRenderer->Draw(square);

    mTime += mDt;
}
//----------------------------------------------------------------------------
void Smoke2D::SetNumActiveVortices (int numActive)
{
    if (0 <= numActive && numActive <= mNumVortices)
    {
        mNumActive = numActive;

    }
    else
    {
        mNumActive = 0;
    }

    memset(mTimelessVelocity[0], 0, mNumPixels*sizeof(Vector2f));
    for (int j = 0; j <= mJMax; ++j)
    {
        for (int i = 0; i <= mIMax; ++i)
        {
            mTimelessVelocity[j][i] += mGravity + mTimelessWind[j][i];
            for (int v = 0; v < mNumActive; ++v)
            {
                mTimelessVelocity[j][i] += mTimelessVortex[j][i][v];
            }
        }
    }

    ComputeSource();
    ComputeVortexOverlay();
}
//----------------------------------------------------------------------------
void Smoke2D::SetGravity (float gravity)
{
    if (gravity >= 0.0f)
    {
        mGravity[1] = -gravity;
        SetNumActiveVortices(mNumActive);
    }
}
//----------------------------------------------------------------------------
void Smoke2D::UseColor (bool useColor)
{
    Texture* texture = (useColor ? mColorTexture : mGrayTexture);
    mDrawDensityInstance->SetPixelTexture(0, "ColorTableSampler", texture);
}
//----------------------------------------------------------------------------
void Smoke2D::UseVortexOverlay (bool useVortexOverlay)
{
    mUseVortexOverlay = useVortexOverlay;
    ComputeVortexOverlay();
}
//----------------------------------------------------------------------------
void Smoke2D::ComputeSource ()
{
    // Compute the source data for the solver.

    if (PixelShader::GetProfile() == PixelShader::PP_ARBFP1)
    {
        float* data = (float*)mSourceTexture->GetData(0);
        for (int j = 0; j <= mJMax; ++j)
        {
            for (int i = 0; i <= mIMax; ++i)
            {
                float density = mTimelessDensity[j][i];
                Vector2f velocity = mTimelessVelocity[j][i];
                *data++ = velocity[0];
                *data++ = velocity[1];
                *data++ = 0.0f;
                *data++ = density;
            }
        }
    }
    else
    {
        Float4* data = (Float4*)mSourceTexture->GetData(0);
        for (int j = 0; j <= mJMax; ++j)
        {
            for (int i = 0; i <= mIMax; ++i)
            {
                float density = mTimelessDensity[j][i];
                Vector2f velocity = mTimelessVelocity[j][i];
                data[i + mIMaxP1*(mJMax-j)] = Float4(velocity[0],
                    -velocity[1], 0.0f, density);
            }
        }
    }

    mRenderer->Update(mSourceTexture, 0);
}
//----------------------------------------------------------------------------
void Smoke2D::ComputeVortexOverlay ()
{
    unsigned char* data = (unsigned char*)mVortexTexture->GetData(0);
    memset(data, 0, 4*mNumPixels);

    if (mUseVortexOverlay)
    {
        // Draw the vortex centers, magenta for counterclockwise and cyan for
        // clockwise.
        float invXRange = 1.0f/(mX1 - mX0);
        float invYRange = 1.0f/(mY1 - mY0);
        for (int v = 0; v < mNumActive; ++v)
        {
            Vector2f center = mVortexCenter[v];
            float x = (center.X() - mX0)*invXRange;
            float y = (center.Y() - mY0)*invYRange;
            int ix = (int)((mIMax-1)*x);
            int iy = (int)((mJMax-1)*y);

            unsigned char r, g, b, a;
            if (mVortexAmplitude[v] > 0.0f)
            {
                r = 255;
                g = 0;
                b = 255;
                a = 255;
            }
            else
            {
                r = 0;
                g = 255;
                b = 255;
                a = 255;
            }

            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    int index;
                    if (PixelShader::GetProfile() == PixelShader::PP_ARBFP1)
                    {
                        index = 4*((ix+dx) + mIMaxP1*(iy+dy));
                    }
                    else
                    {
                        index = 4*((ix+dx) + mIMaxP1*(mJMax-(iy+dy)));
                    }
                    data[index++] = b;
                    data[index++] = g;
                    data[index++] = r;
                    data[index++] = a;
                }
            }
        }
    }

    mRenderer->Update(mVortexTexture, 0);
}
//----------------------------------------------------------------------------
