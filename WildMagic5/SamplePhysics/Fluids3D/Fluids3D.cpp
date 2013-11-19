// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Fluids3D.h"

WM5_WINDOW_APPLICATION(Fluids3D);

//----------------------------------------------------------------------------
Fluids3D::Fluids3D ()
    :
    WindowApplication3("SamplePhysics/Fluids3D",0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mNumIndices = 0;
    mIndices = 0;
    mSmoke = 0;
    mSingleStep = true;
    mUseColor = false;
    mIndexBufferNeedsUpdate = false;
}
//----------------------------------------------------------------------------
bool Fluids3D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Create the pseudocoloring for display.
    Vector3f key[9] =
    {
        Vector3f(  0.0f/255.0f,   0.0f/255.0f,   0.0f/255.0f),  // black
        Vector3f(128.0f/255.0f,  64.0f/255.0f,  64.0f/255.0f),  // brown
        Vector3f(128.0f/255.0f,   0.0f/255.0f, 255.0f/255.0f),  // violet
        Vector3f(  0.0f/255.0f,   0.0f/255.0f, 255.0f/255.0f),  // blue
        Vector3f(  0.0f/255.0f, 255.0f/255.0f,   0.0f/255.0f),  // green
        Vector3f(255.0f/255.0f, 255.0f/255.0f,   0.0f/255.0f),  // yellow
        Vector3f(255.0f/255.0f, 128.0f/255.0f,   0.0f/255.0f),  // orange
        Vector3f(255.0f/255.0f,   0.0f/255.0f,   0.0f/255.0f),  // red
        Vector3f(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f)   // white
    };

    for (int i = 0, j = 0; i < 8; ++i)
    {
        for (int k = 0; k < 32; ++k, ++j)
        {
            float t = k/32.0f;
            float omt = 1.0f - t;
            mColor[j][0] = omt*key[i][0] + t*key[i+1][0];
            mColor[j][1] = omt*key[i][1] + t*key[i+1][1];
            mColor[j][2] = omt*key[i][2] + t*key[i+1][2];
        }
    }

    // Create the fluid solver.
    float x0 = -0.5f, y0 = -0.5f, z0 = -0.5f;
    float x1 = 0.5f, y1 = 0.5f, z1 = 0.5f;
    float dt = 0.001f;
    float denViscosity = 0.0001f, velViscosity = 0.0001f;
    int imax = 23, jmax = 23, kmax = 23;
    int numGaussSeidelIterations = 16;
    bool densityDirichlet = true;
    int numVortices = 8;
    mSmoke = new0 Smoke3D<float>(x0, y0, z0, x1, y1, z1, dt, denViscosity,
        velViscosity, imax, jmax, kmax, numGaussSeidelIterations,
        densityDirichlet, numVortices);

    mSmoke->Initialize();

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(1.0f, 0.0f, 0.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Create the scene.
    CreateScene();
    mScene->Update();
#ifdef USE_PARTICLES
    mCube->GenerateParticles(mCamera);
#endif
    UpdateVertexBuffer();
    UpdateIndexBuffer();

    // The scene is controlled by a virtual trackball.
    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void Fluids3D::OnTerminate ()
{
    delete0(mSmoke);
    mScene = 0;
    mCube = 0;

#ifdef USE_PARTICLES
    delete1(mIndices);
#endif

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Fluids3D::OnIdle ()
{
    MeasureTime();

#ifdef USE_PARTICLES
    bool particlesNeedUpdate = false;
#endif
    mIndexBufferNeedsUpdate = false;

    if (MoveCamera())
    {
#ifdef USE_PARTICLES
        particlesNeedUpdate = true;
#endif
        mIndexBufferNeedsUpdate = true;
    }

    if (MoveObject())
    {
#ifdef USE_PARTICLES
        particlesNeedUpdate = true;
#endif
        mIndexBufferNeedsUpdate = true;
        mScene->Update();
    }

#ifdef USE_PARTICLES
    if (particlesNeedUpdate)
    {
        mCube->GenerateParticles(mCamera);
    }
#endif

    if (mIndexBufferNeedsUpdate)
    {
        UpdateIndexBuffer();
        mIndexBufferNeedsUpdate = false;
    }

    if (!mSingleStep)
    {
        PhysicsTick();
    }

    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Fluids3D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        mSmoke->Initialize();
        UpdateVertexBuffer();
        return true;
    case ' ':
        if (mSingleStep)
        {
            PhysicsTick();
        }
        return true;
    case 's':
    case 'S':
        mSingleStep = !mSingleStep;
        return true;
    case 'c':
    case 'C':
        mUseColor = !mUseColor;
        UpdateVertexBuffer();
        return true;
    case '+':
    case '=':
    {
        int numActive = mSmoke->GetNumActiveVortices();
        if (numActive < mSmoke->GetNumVortices())
        {
            mSmoke->SetNumActiveVortices(numActive + 1);
        }
        return true;
    }
    case '-':
    case '_':
    {
        int numActive = mSmoke->GetNumActiveVortices();
        if (numActive > 0)
        {
            mSmoke->SetNumActiveVortices(numActive - 1);
        }
        return true;
    }
    case 'g':
    {
        float gravity = mSmoke->GetGravity() - 0.5f;
        if (gravity < 0.0f)
        {
            gravity = 0.0f;
        }
        mSmoke->SetGravity(gravity);
        return true;
    }
    case 'G':
        mSmoke->SetGravity(mSmoke->GetGravity() + 0.5f);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Fluids3D::PhysicsTick ()
{
    mSmoke->DoSimulationStep();
    UpdateVertexBuffer();
    UpdateIndexBuffer();
}
//----------------------------------------------------------------------------
void Fluids3D::GraphicsTick ()
{
    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCube);
        DrawFrameRate(8, GetHeight()-8, mTextColor);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
void Fluids3D::CreateScene ()
{
    // Get fluid solver parameters.
    const int bound0M1 = mSmoke->GetIMax();
    const int bound1M1 = mSmoke->GetJMax();
    const int bound2M1 = mSmoke->GetKMax();
    const int bound0 = bound0M1 + 1;
    const int bound1 = bound1M1 + 1;
    const int bound2 = bound2M1 + 1;
    const int quantity = bound0*bound1*bound2;
    const float* x = mSmoke->GetX();
    const float* y = mSmoke->GetY();
    const float* z = mSmoke->GetZ();

#ifdef USE_PARTICLES
    // Create the vertex format.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
#else
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
#endif

    // Create the vertex buffer for the cube.
#ifdef USE_PARTICLES
    const int numVertices = 4*quantity;
#else
    const int numVertices = quantity;
#endif

    int vstride = vformat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);

    int i0, i1, i2, index;

#ifdef USE_PARTICLES
    const float delta = mSmoke->GetDx();
    Float4* posSize = new1<Float4>(quantity);
    for (i2 = 0, index = 0; i2 < bound2; ++i2)
    {
        for (i1 = 0; i1 < bound1; ++i1)
        {
            for (i0 = 0; i0 < bound0; ++i0, ++index)
            {
                posSize[index] = Float4(x[i0], y[i1], z[i2], delta);
            }
        }
    }

    mCube = new0 Particles(vformat, vbuffer, 4, posSize, 1.0f);
    UpdateVertexBuffer();

    IndexBuffer* ibuffer = mCube->GetIndexBuffer();
#else
    VertexBufferAccessor vba(vformat, vbuffer);
    for (i2 = 0, index = 0; i2 < bound2; ++i2)
    {
        for (i1 = 0; i1 < bound1; ++i1)
        {
            for (i0 = 0; i0 < bound0; ++i0, ++index)
            {
                vba.Position<Float3>(index) = Float3(x[i0], y[i1], z[i2]);
            }
        }
    }

    // Create the index buffer for the cube.
    const int numIndices =
        6*bound0M1*bound1M1*bound2 +
        6*bound0M1*bound1*bound2M1 +
        6*bound0*bound1M1*bound2M1;

    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();

    const int bound01 = bound0*bound1;
    int j0, j1, j2, j3;
    for (i2 = 0; i2 < bound2; ++i2)
    {
        for (i1 = 0; i1 < bound1M1; ++i1)
        {
            for (i0 = 0; i0 < bound0M1; ++i0)
            {
                j0 = i0 + bound0*(i1 + bound1*i2);
                j1 = j0 + 1;
                j2 = j1 + bound0;
                j3 = j2 - 1;
                *indices++ = j0;
                *indices++ = j1;
                *indices++ = j2;
                *indices++ = j0;
                *indices++ = j2;
                *indices++ = j3;
            }
        }
    }

    for (i1 = 0; i1 < bound1; ++i1)
    {
        for (i2 = 0; i2 < bound2M1; ++i2)
        {
            for (i0 = 0; i0 < bound0M1; ++i0)
            {
                j0 = i0 + bound0*(i1 + bound1*i2);
                j1 = j0 + 1;
                j2 = j1 + bound01;
                j3 = j2 - 1;
                *indices++ = j0;
                *indices++ = j1;
                *indices++ = j2;
                *indices++ = j0;
                *indices++ = j2;
                *indices++ = j3;
            }
        }
    }

    for (i0 = 0; i0 < bound0; ++i0)
    {
        for (i1 = 0; i1 < bound1M1; ++i1)
        {
            for (i2 = 0; i2 < bound2M1; ++i2)
            {
                j0 = i0 + bound0*(i1 + bound1*i2);
                j1 = j0 + bound0;
                j2 = j1 + bound01;
                j3 = j2 - bound0;
                *indices++ = j0;
                *indices++ = j1;
                *indices++ = j2;
                *indices++ = j0;
                *indices++ = j2;
                *indices++ = j3;
            }
        }
    }

    mCube = new0 TriMesh(vformat, vbuffer, ibuffer);
    UpdateVertexBuffer();
#endif

    mNumIndices = ibuffer->GetNumElements();
    mIndices = new1<int>(mNumIndices);
    memcpy(mIndices, ibuffer->GetData(), mNumIndices*sizeof(int));

    // Create the cube effect.
#ifdef USE_PARTICLES
    std::string path = Environment::GetPathR("Disk.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance =
        VertexColor4TextureEffect::CreateUniqueInstance(texture,
        Shader::SF_NEAREST, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE);
#else
    VertexColor4Effect* effect = new0 VertexColor4Effect();
    VisualEffectInstance* instance = effect->CreateInstance();
#endif

    const VisualPass* pass = instance->GetPass(0);
    AlphaState* astate = pass->GetAlphaState();
    astate->BlendEnabled = true;

    CullState* cstate = pass->GetCullState();
    cstate->Enabled = false;

    DepthState* dstate = pass->GetDepthState();
    dstate->Enabled = false;
    dstate->Writable = false;

    mCube->SetEffectInstance(instance);

    mScene = new0 Node();
    mScene->AttachChild(mCube);
}
//----------------------------------------------------------------------------
void Fluids3D::UpdateVertexBuffer ()
{
    VertexBufferAccessor vba(mCube);
    const int bound0 = mSmoke->GetIMax() + 1;
    const int bound1 = mSmoke->GetJMax() + 1;
    const int bound2 = mSmoke->GetKMax() + 1;
    float*** density = mSmoke->GetDensity();

    for (int i2 = 0, index = 0; i2 < bound2; ++i2)
    {
        for (int i1 = 0; i1 < bound1; ++i1)
        {
            for (int i0 = 0; i0 < bound0; ++i0)
            {
                const float scaleDown = 0.5f;
                float value = density[i2][i1][i0];
                if (value > 1.0f)
                {
                    value = 1.0f;
                }

                Float4 color;
                if (mUseColor)
                {
                    Vector3f key = mColor[(int)(255.0f*value)];
                    color[0] = key[0];
                    color[1] = key[1];
                    color[2] = key[2];
                    color[3] = scaleDown*value;
                }
                else
                {
                    color[0] = scaleDown*value;
                    color[1] = color[0];
                    color[2] = color[0];
                    color[3] = color[0];
                }

                const float alphaThreshold = 0.01f;
                if (color[3] < alphaThreshold)
                {
                    color[3] = 0.0f;
                }

#ifdef USE_PARTICLES
                vba.Color<Float4>(0, index++) = color;
                vba.Color<Float4>(0, index++) = color;
                vba.Color<Float4>(0, index++) = color;
                vba.Color<Float4>(0, index++) = color;
#else
                vba.Color<Float4>(0, index++) = color;
#endif
            }
        }
    }

    mRenderer->Update(mCube->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void Fluids3D::UpdateIndexBuffer ()
{
    VertexBufferAccessor vba(mCube);
    APoint camPos = mCamera->GetPosition();
    const int numTriangles = mNumIndices/3;
    int* currentIndex = mIndices;

    mTriangles.clear();
    for (int t = 0; t < numTriangles; ++t)
    {
        Triangle tri;
        tri.mIndex0 = *currentIndex++;
        tri.mIndex1 = *currentIndex++;
        tri.mIndex2 = *currentIndex++;

#ifdef USE_PARTICLES
        float alpha = vba.Color<Float4>(0, tri.mIndex0)[3];
        if (alpha == 0.0f)
        {
            continue;
        }
#else
        float alpha0 = vba.Color<Float4>(0, tri.mIndex0)[3];
        float alpha1 = vba.Color<Float4>(0, tri.mIndex1)[3];
        float alpha2 = vba.Color<Float4>(0, tri.mIndex2)[3];
        if (alpha0 == 0.0f && alpha1 == 0.0f && alpha2 == 0.0f)
        {
            continue;
        }
#endif

        Vector3f scaledCenter =
            vba.Position<Vector3f>(tri.mIndex0) +
            vba.Position<Vector3f>(tri.mIndex1) +
            vba.Position<Vector3f>(tri.mIndex2);

        APoint output = mCube->WorldTransform*APoint(scaledCenter);
        AVector diff = output - camPos;
        tri.mNegSqrDistance = -diff.SquaredLength();

        mTriangles.insert(tri);
    }

    IndexBuffer* ibuffer = mCube->GetIndexBuffer();
    int* indices = (int*)ibuffer->GetData();
    ibuffer->SetNumElements(3*(int)mTriangles.size());

    std::multiset<Triangle>::iterator iter = mTriangles.begin();
    std::multiset<Triangle>::iterator end = mTriangles.end();
    for (/**/; iter != end; ++iter)
    {
        *indices++ = iter->mIndex0;
        *indices++ = iter->mIndex1;
        *indices++ = iter->mIndex2;
    }

    mRenderer->Update(ibuffer);
}
//----------------------------------------------------------------------------
