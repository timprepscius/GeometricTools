// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BlendedTerrain.h"

WM5_WINDOW_APPLICATION(BlendedTerrain);

//----------------------------------------------------------------------------
BlendedTerrain::BlendedTerrain ()
    :
    WindowApplication3("SampleGraphics/BlendedTerrain", 0, 0, 640, 480,
        Float4(0.0f, 0.5f, 0.75f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");

    mFlowDirection = 0;
    mPowerFactor = 0;
    mZAngle = 0.0f;
    mFlowDelta = 0.00002f;
    mZDeltaAngle = 0.00002f;
}
//----------------------------------------------------------------------------
bool BlendedTerrain::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    APoint camPosition(0.0f, -7.0f, 1.5f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.005f, 0.002f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BlendedTerrain::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mSkyDome = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BlendedTerrain::OnIdle ()
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

    UpdateClouds();

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BlendedTerrain::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    case '+':
    case '=':
    {
        mPowerFactor[0] += 0.01f;
        return true;
    }

    case '-':
    case '_':
    {
        mPowerFactor[0] -= 0.01f;
        if (mPowerFactor[0] < 0.01f)
        {
            mPowerFactor[0] = 0.01f;
        }
        return true;
    }
    }

    return false;
}
//----------------------------------------------------------------------------
void BlendedTerrain::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create the height field.
    VertexFormat* vformat = VertexFormat::Create(4,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT1, 1,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 2);
    int vstride = vformat->GetStride();

    const int numXSamples = 64, numYSamples = 64;
    const float xExtent = 8.0f, yExtent = 8.0f;
    const int numVertices = numXSamples*numYSamples;
    int numIndices = 6*(numXSamples - 1)*(numYSamples - 1);

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);

    // Generate the geometry for a flat height field.
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 position;
    Float2 tcoord;
    position[2] = 0.0f;
    float inv0 = 1.0f/(numXSamples - 1.0f);
    float inv1 = 1.0f/(numYSamples - 1.0f);
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < numYSamples; ++i1)
    {
        tcoord[1] = i1*inv1;
        position[1] = (2.0f*tcoord[1] - 1.0f)*yExtent;
        for (i0 = 0; i0 < numXSamples; ++i0, ++i)
        {
            tcoord[0] = i0*inv0;
            position[0] = (2.0f*tcoord[0] - 1.0f)*xExtent;
            vba.Position<Float3>(i) = position;
            vba.TCoord<Float2>(0 ,i) = tcoord;
            vba.TCoord<float>(1, i) = 0.0f;
            vba.TCoord<Float2>(2, i) = tcoord;
        }
    }

    // Generate the index array for a regular grid of squares, each square a
    // pair of triangles.
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (i1 = 0, i = 0; i1 < numYSamples - 1; ++i1)
    {
        for (i0 = 0; i0 < numXSamples - 1; ++i0)
        {
            int v0 = i0 + numXSamples*i1;
            int v1 = v0 + 1;
            int v2 = v1 + numXSamples;
            int v3 = v0 + numXSamples;
            *indices++ = v0;
            *indices++ = v1;
            *indices++ = v2;
            *indices++ = v0;
            *indices++ = v2;
            *indices++ = v3;
        }
    }

    // Set the heights based on a precomputed height field.
    std::string path = Environment::GetPathR("BTHeightField.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    unsigned char* data = (unsigned char*)texture->GetData(0);
    for (i = 0; i < numVertices; i++, data += 4)
    {
        unsigned char value = *data;
        float height = ((float)value)/255.0f;
        float perturb = 0.05f*Mathf::SymmetricRandom();
        vba.Position<Float3>(i)[2] = 3.0f*height + perturb;
        vba.TCoord<Float2>(0, i)[0] *= 8.0f;
        vba.TCoord<Float2>(0, i)[1] *= 8.0f;
        vba.TCoord<float>(1, i) = height;
    }
    delete0(texture);

    TriMesh* heightField = new0 TriMesh(vformat, vbuffer, ibuffer);
    mScene->AttachChild(heightField);

    // Create the shader constants.
    ShaderFloat* flowDirection = new0 ShaderFloat(1);
    mFlowDirection = flowDirection->GetData();
    mFlowDirection[0] = 0.0f;
    mFlowDirection[1] = 0.0f;

    ShaderFloat* powerFactor = new0 ShaderFloat(1);
    mPowerFactor = powerFactor->GetData();
    mPowerFactor[0] = 0.5f;

    // Create the textures.
    std::string grassName = Environment::GetPathR("BTGrass.wmtf");
    Texture2D* grassTexture = Texture2D::LoadWMTF(grassName);
    grassTexture->GenerateMipmaps();

    std::string stoneName = Environment::GetPathR("BTStone.wmtf");
    Texture2D* stoneTexture = Texture2D::LoadWMTF(stoneName);
    stoneTexture->GenerateMipmaps();

    std::string cloudName = Environment::GetPathR("BTCloud.wmtf");
    Texture2D* cloudTexture = Texture2D::LoadWMTF(cloudName);
    cloudTexture->GenerateMipmaps();

    std::string effectFile = Environment::GetPathR("BlendedTerrain.wmfx");
    BlendedTerrainEffect* effect = new0 BlendedTerrainEffect(effectFile);
    heightField->SetEffectInstance(effect->CreateInstance(flowDirection,
        powerFactor, grassTexture, stoneTexture, cloudTexture));

    // Create the skydome.
    path = Environment::GetPathR("SkyDomePNT2.wmvf");
    Visual::PrimitiveType type;
    Visual::LoadWMVF(path, type, vformat, vbuffer, ibuffer);

    mSkyDome = new0 TriMesh(vformat, vbuffer, ibuffer);
    mSkyDome->LocalTransform.SetTranslate(APoint::ORIGIN);
    mSkyDome->LocalTransform.SetUniformScale(7.9f);

    path = Environment::GetPathR("SkyDome.wmtf");
    texture = Texture2D::LoadWMTF(path);
    mSkyDome->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    mScene->AttachChild(mSkyDome);
}
//----------------------------------------------------------------------------
void BlendedTerrain::UpdateClouds ()
{
    // Animate the cloud layer.
    mFlowDirection[0] += mFlowDelta;
    if (mFlowDirection[0] < 0.0f)
    {
        mFlowDirection[0] += 1.0f;
    }
    else if (mFlowDirection[0] > 1.0f )
    {
        mFlowDirection[0] -= 1.0f;
    }

    // Rotate the sky dome.
    mZAngle += mZDeltaAngle;
    if (mZAngle > Mathf::TWO_PI)
    {
        mZAngle -= Mathf::TWO_PI;
    }
    mSkyDome->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, mZAngle));
    mSkyDome->Update();
}
//----------------------------------------------------------------------------
