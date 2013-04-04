// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.1 (2012/07/07)

#include "NonlocalBlowup.h"
#include "DisplacementEffect.h"
#include "SaveBMP32.h"

WM5_WINDOW_APPLICATION(NonlocalBlowup);

//#define RUN_CONSOLE
#ifdef RUN_CONSOLE
extern void RunConsole ();
#endif

//#define SINGLE_STEP

//----------------------------------------------------------------------------
NonlocalBlowup::NonlocalBlowup ()
    :
    WindowApplication3("SampleMathematics/NonlocalBlowup", 0, 0, 512, 512,
        Float4(0.4f, 0.5f, 0.6f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f),
    mInitialData(DIMENSION, DIMENSION),
    mDomain(DIMENSION, DIMENSION),
    mHeightTexture(0),
    mDomainTexture(0),
    mSolver(0),
    mIteration(0),
    mIsFinite(true),
    mPrefix(""),
    mTakeSnapshot(true)
{
    Environment::InsertDirectory(ThePath + "Data/");
}
//----------------------------------------------------------------------------
bool NonlocalBlowup::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }
#ifdef RUN_CONSOLE
    RunConsole();
    return false;
#endif

    mScene = new0 Node();
    mScene->LocalTransform.SetRotate(HMatrix(
        0.80475128f, 0.59107417f, -0.054833174f, 0.0f,
        -0.17529237f, 0.32487807f, 0.92936903f, 0.0f,
        0.56714010f, -0.73829913f, 0.36505684f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f));
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* mesh = StandardMesh(vformat).Rectangle(256, 256, 16.0f, 16.0f);
    mScene->AttachChild(mesh);

    std::string gridName = Environment::GetPathR("Grid.wmtf");
    Texture2D* gridTexture = Texture2D::LoadWMTF(gridName);
    gridTexture->GenerateMipmaps();

    Texture1D* colorTexture = new0 Texture1D(Texture::TF_A8R8G8B8, 8, 1);
    unsigned char* color = (unsigned char*)colorTexture->GetData(0);
    color[ 0] = 128;  color[ 1] = 128;  color[ 2] = 128;  color[ 3] = 255;
    color[ 4] = 255;  color[ 5] =   0;  color[ 6] = 128;  color[ 7] = 255;
    color[ 8] = 255;  color[ 9] =   0;  color[10] =   0;  color[11] = 255;
    color[12] =   0;  color[13] = 255;  color[14] =   0;  color[15] = 255;
    color[16] =   0;  color[17] = 255;  color[18] = 255;  color[19] = 255;
    color[20] =   0;  color[21] = 128;  color[22] = 255;  color[23] = 255;
    color[24] =   0;  color[25] =   0;  color[26] = 255;  color[27] = 255;
    color[28] = 255;  color[29] = 255;  color[30] = 255;  color[31] = 255;

    float dt = 0.01f, dx = 1.0f, dy = 1.0f;
    // Uncomment only one of these at a time.
    NonconvexDomain0p50(dt, dx, dy);
    //SquareSymmetric0p01(dt, dx, dy);
    //SquareSymmetric0p50(dt, dx, dy);
    //SquareSymmetric0p99(dt, dx, dy);
    //SquareGaussX0p50(dt, dx, dy);
    //SquareGaussXY0p50(dt, dx, dy);
    //SquareGaussFour0p50(dt, dx, dy);

    DisplacementEffect* effect = new0 DisplacementEffect();
    mesh->SetEffectInstance(effect->CreateInstance(mHeightTexture,
        gridTexture, colorTexture, mDomainTexture));

    // Set up the camera so that it looks at the graph from slightly above
    // the xy-plane and at a skewed angle/direction of view.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 10000.0f);
    APoint camPosition(0.0f, 3.46f, 42.97f);
    AVector camDVector(0.0f, 0.0f, -1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void NonlocalBlowup::OnTerminate ()
{
    delete0(mSolver);
    mScene = 0;
    mWireState = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        mRenderer->PostDraw();
        if (mTakeSnapshot)
        {
            GetSnapshot();
            mTakeSnapshot = false;
        }
        mRenderer->DisplayColorBuffer();
    }

#ifndef SINGLE_STEP
    ExecuteSolver();
#endif

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool NonlocalBlowup::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
#ifdef SINGLE_STEP
    case '+':
    case '=':
        ExecuteSolver();
        return true;
#endif
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildSquareDomain ()
{
    // Create a texture representing the domain.  This will be used by the
    // shader for drawing the domain and graph of temperature.
    mDomainTexture = new0 Texture2D(Texture::TF_A8R8G8B8, DIMENSION,
        DIMENSION, 1);
    mDomainTexture->GenerateMipmaps();
    unsigned char* data = (unsigned char*)mDomainTexture->GetData(0);
    memset(data, 0, NUMSAMPLES*sizeof(unsigned char));

    // Create a domain mask that is 1 at interior samples of the square and 0
    // at boundary samples of the square.
    mDomain.ClearPixels();
    for (int y = 1; y < DIMENSION-1; ++y)
    {
        for (int x = 1; x < DIMENSION-1; ++x)
        {
            mDomain(x, y) = 255;
            data[4*(x + DIMENSION*y) + 0] = 255;
            data[4*(x + DIMENSION*y) + 1] = 255;
            data[4*(x + DIMENSION*y) + 2] = 255;
            data[4*(x + DIMENSION*y) + 3] = 255;
        }
    }
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildPolygonDomain ()
{
    // Load a texture that represents a nonconvex polygonal domain.  This will
    // be used by the shader for drawing the domain and graph of temperature.
    std::string domainName = Environment::GetPathR("Domain.wmtf");
    mDomainTexture = Texture2D::LoadWMTF(domainName);
    mDomainTexture->GenerateMipmaps();
    unsigned char* data = (unsigned char*)mDomainTexture->GetData(0);

    // Create a domain mask that is 1 at interior samples of the polygon and 0
    // at boundary samples of the polygon.
    mDomain.ClearPixels();
    for (int i = 0; i < NUMSAMPLES; ++i, data += 4)
    {
        mDomain[i] = *data;
    }
}
//----------------------------------------------------------------------------
void NonlocalBlowup::CreateInitialHeight ()
{
    mHeightTexture = new0 Texture2D(Texture::TF_R32F, DIMENSION,
        DIMENSION, 1);
    memcpy(mHeightTexture->GetData(0), mInitialData.GetPixels1D(),
        NUMSAMPLES*sizeof(float));
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildInitialData0 ()
{
    for (int y = 0; y < DIMENSION; ++y)
    {
        float fy = -1.0f + 2.0f*y/(DIMENSION - 1.0f);
        float yvalue = 1.0f - fy*fy;
        for (int x = 0; x < DIMENSION; ++x)
        {
            float fx = -1.0f + 2.0f*x/(DIMENSION - 1.0f);
            float xvalue = 1.0f - fx*fx;
            mInitialData(x, y) = xvalue*yvalue;
        }
    }

    CreateInitialHeight();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildInitialDataGaussX ()
{
    for (int y = 0; y < DIMENSION; ++y)
    {
        float fy = -1.0f + 2.0f*y/(DIMENSION - 1.0f);
        float yvalue = 1.0f - fy*fy;
        for (int x = 0; x < DIMENSION; ++x)
        {
            float fx = -1.0f + 2.0f*x/(DIMENSION - 1.0f);
            float xvalue = 1.0f - fx*fx;
            float xratio = (fx - 0.5f)/0.01f;
            float gauss = 0.25f*exp(-(xratio*xratio));
            mInitialData(x, y) = xvalue*yvalue*gauss;
        }
    }

    CreateInitialHeight();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildInitialDataGaussXY ()
{
    for (int y = 0; y < DIMENSION; ++y)
    {
        float fy = -1.0f + 2.0f*y/(DIMENSION - 1.0f);
        float yvalue = 1.0f - fy*fy;
        float yratio = (fy - 0.5f)/0.01f;
        for (int x = 0; x < DIMENSION; ++x)
        {
            float fx = -1.0f + 2.0f*x/(DIMENSION - 1.0f);
            float xvalue = 1.0f - fx*fx;
            float xratio = (fx - 0.5f)/0.01f;
            float gauss = 0.25f*exp(-(xratio*xratio + yratio*yratio));
            mInitialData(x, y) = xvalue*yvalue*gauss;
        }
    }

    CreateInitialHeight();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildInitialDataGaussFour ()
{
    for (int y = 0; y < DIMENSION; ++y)
    {
        float fy = -1.0f + 2.0f*y/(DIMENSION - 1.0f);
        float yvalue = 1.0f - fy*fy;
        float yratio0 = (fy - 0.5f)/0.01f;
        float yratio1 = (fy + 0.5f)/0.01f;
        for (int x = 0; x < DIMENSION; ++x)
        {
            float fx = -1.0f + 2.0f*x/(DIMENSION - 1.0f);
            float xvalue = 1.0f - fx*fx;
            float xratio0 = (fx - 0.5f)/0.01f;
            float xratio1 = (fx + 0.5f)/0.01f;
            float gauss00 = exp(-xratio0*xratio0-yratio0*yratio0);
            float gauss10 = exp(-xratio1*xratio1-yratio0*yratio0);
            float gauss01 = exp(-xratio0*xratio0-yratio1*yratio1);
            float gauss11 = exp(-xratio1*xratio1-yratio1*yratio1);
            float gauss = 0.25f*(gauss00 + gauss10 + gauss01 + gauss11);
            mInitialData(x, y) = xvalue*yvalue*gauss;
        }
    }

    CreateInitialHeight();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::BuildInitialDataGaussXYOff ()
{
    for (int y = 0; y < DIMENSION; ++y)
    {
        float fy = -1.0f + 2.0f*y/(DIMENSION - 1.0f);
        float yvalue = 1.0f - fy*fy;
        float yratio = (fy - 0.75f)/0.01f;
        for (int x = 0; x < DIMENSION; ++x)
        {
            float fx = -1.0f + 2.0f*x/(DIMENSION - 1.0f);
            float xvalue = 1.0f - fx*fx;
            float xratio = (fx - 0.5f)/0.01f;
            float gauss = 0.25f*exp(-(xratio*xratio + yratio*yratio));
            mInitialData(x, y) = xvalue*yvalue*gauss;
        }
    }

    CreateInitialHeight();
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareSymmetric0p01 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialData0();

    float p = 0.01f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareSymmetric0p01_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareSymmetric0p50 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialData0();

    float p = 0.50f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareSymmetric0p50_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareSymmetric0p99 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialData0();

    float p = 0.99f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareSymmetric0p99_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareGaussX0p50 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialDataGaussX();

    float p = 0.50f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareGaussX0p50_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareGaussXY0p50 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialDataGaussXY();

    float p = 0.50f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareGaussXY0p50_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::SquareGaussFour0p50 (float dt, float dx, float dy)
{
    BuildSquareDomain();
    BuildInitialDataGaussFour();

    float p = 0.50f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/SquareGaussFour0p50_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::NonconvexDomain0p50 (float dt, float dx, float dy)
{
    BuildPolygonDomain();

    //BuildInitialData0();
    //BuildInitialDataGaussXY();  // Appears to blow up everywhere.
    BuildInitialDataGaussXYOff();

    float p = 0.50f;
    bool success = false;
    mSolver = new0 NonlocalSolver2(DIMENSION, DIMENSION, &mInitialData,
        &mDomain, dt, dx, dy, p, success);
    assertion(success, "Failed to create solver.\n");
    WM5_UNUSED(success);

    mPrefix = ThePath + "Movies/Nonconvex0p50_";
}
//----------------------------------------------------------------------------
void NonlocalBlowup::ExecuteSolver ()
{
    if (mIsFinite)
    {
        if (mSolver->Enable())
        {
            const int numGaussSeidel = 8;
            if (mSolver->Execute(mIteration++, numGaussSeidel))
            {
                float* readBack = mSolver->GetReadBack();
                if (readBack)
                {
                    mIsFinite = true;
                    memcpy(mHeightTexture->GetData(0), readBack,
                        mHeightTexture->GetNumTotalBytes());
                    mRenderer->Update(mHeightTexture, 0);
                    mTakeSnapshot = true;
                }
                else
                {
                    mIsFinite = false;
                }
            }
            mSolver->Disable();
        }
    }
}
//----------------------------------------------------------------------------
void NonlocalBlowup::GetSnapshot ()
{
    char name[256];
    if (mIteration < 10)
    {
        sprintf(name, "%s00%d.bmp", mPrefix.c_str(), mIteration);
    }
    else if (mIteration < 100)
    {
        sprintf(name, "%s0%d.bmp", mPrefix.c_str(), mIteration);
    }
    else
    {
        sprintf(name, "%s%d.bmp", mPrefix.c_str(), mIteration);
    }

    Image2<PixelBGRA8> image(512, 512);
    glReadPixels(0, 0, 512, 512, GL_BGRA, GL_UNSIGNED_BYTE,
        image.GetPixels1D());

    SaveBMP32(std::string(name), image);
}
//----------------------------------------------------------------------------
