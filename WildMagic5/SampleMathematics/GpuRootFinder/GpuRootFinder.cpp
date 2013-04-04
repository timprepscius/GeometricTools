// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2010/09/08)

#include "GpuRootFinder.h"

WM5_WINDOW_APPLICATION(GpuRootFinder);

//----------------------------------------------------------------------------
GpuRootFinder::GpuRootFinder ()
    :
    WindowApplication3("SampleMathematics/GpuRootFinder", 0, 0, 64, 64,
        Float4(0.5f, 0.5f, 0.5f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
    mResults = 0;
}
//----------------------------------------------------------------------------
bool GpuRootFinder::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Create a screen-space camera to use with the render target.
    mScreenCamera = ScreenTarget::CreateCamera();
    mRenderer->SetCamera(mScreenCamera);

    // Create a screen polygon to use with the render target.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mScreenPolygon = ScreenTarget::CreateRectangle(vformat, 4096, 2048,
        0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // Create the render targets.
    Texture::Format tformat = Texture::TF_A8R8G8B8;
    mRenderTarget = new0 RenderTarget(1, tformat, 4096, 2048, false, false);

    // Create the results texture.
    mResults = new0 Texture2D(Texture::TF_A8R8G8B8, 4096, 2048, 1);

    // Create the root-finder effect.
    RootFinderEffect* effect = new0 RootFinderEffect();
    mScreenPolygon->SetEffectInstance(effect->CreateInstance());

    // Compute the roots.
    if (mRenderer->PreDraw())
    {
        int64_t start = GetTimeInMicroseconds();

        mRenderer->Enable(mRenderTarget);
        mRenderer->Draw(mScreenPolygon);
        mRenderer->Disable(mRenderTarget);

        mRenderer->ReadColor(0, mRenderTarget, mResults);
        unsigned char* data = (unsigned char*)mResults->GetData(0);
        const int jmax = 4096*2048;
        for (int j = 0; j < jmax; ++j, data += 4)
        {
            int negMin = data[0];
            int posMax = data[1];
            int posMin = data[2];
            int negMax = data[3];

            int e, unbiased;
            float unit0, unit1, x0, x1, y0, y1;

            if (posMin <= posMax)
            {
                for (e = posMin; e <= posMax; ++e)
                {
                    unbiased = e - 127;
                    unit0 = 1.0f + ldexp((float)j, -23);
                    unit1 = 1.0f + ldexp((float)(j+1), -23);
                    x0 = ldexp(unit0, unbiased);
                    x1 = ldexp(unit1, unbiased);
                    y0 = MyFunction(x0);
                    y1 = MyFunction(x1);
                    assertion(y0*y1 <= 0.0f, "Unexpected condition.\n");
                    mRoots.insert(fabs(y0) <= fabs(y1) ? x0 : x1);
                }
            }

            if (negMin <= negMax)
            {
                for (e = negMin; e <= negMax; ++e)
                {
                    unbiased = e - 127;
                    unit0 = 1.0f + ldexp((float)j, -23);
                    unit1 = 1.0f + ldexp((float)(j+1), -23);
                    x0 = -ldexp(unit0, unbiased);
                    x1 = -ldexp(unit1, unbiased);
                    y0 = MyFunction(x0);
                    y1 = MyFunction(x1);
                    assertion(y0*y1 <= 0.0f, "Unexpected condition.\n");
                    mRoots.insert(fabs(y0) <= fabs(y1) ? x0 : x1);
                }
            }
        }

        int64_t final = GetTimeInMicroseconds();

        FILE* output = fopen("results.txt", "wt");
        fprintf(output, "microseconds = %d\n", (int)(final-start));
        std::set<float>::iterator iter = mRoots.begin();
        std::set<float>::iterator end = mRoots.end();
        for (/**/; iter != end; ++iter)
        {
            fprintf(output, "root = %f\n", *iter);
        }
        fclose(output);

        mRenderer->PostDraw();
    }

    // Do not launch the application window.
    return false;
}
//----------------------------------------------------------------------------
void GpuRootFinder::OnTerminate ()
{
    mScreenCamera = 0;
    mRenderTarget = 0;
    mScreenPolygon = 0;
    delete0(mResults);

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
float GpuRootFinder::MyFunction (float x)
{
    return (x - 1.1f)*(x + 2.2f);
}
//----------------------------------------------------------------------------
