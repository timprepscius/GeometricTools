// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Terrains.h"
#include "TerrainEffect.h"

WM5_WINDOW_APPLICATION(Terrains);

//----------------------------------------------------------------------------
Terrains::Terrains ()
    :
    WindowApplication3("SampleGraphics/Terrains", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");

    mHeightAboveTerrain = 20.0f;
}
//----------------------------------------------------------------------------
bool Terrains::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.  Position the camera in the middle of page[0][0].
    // Orient it to look diagonally across the terrain pages.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1500.0f);
    APoint camPosition(64.0f, 64.0f, mHeightAboveTerrain);
    AVector camDVector(Mathf::INV_SQRT_2, Mathf::INV_SQRT_2, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(1.0f, 0.01f);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void Terrains::OnTerminate ()
{
    mScene = 0;
    mSkyDome = 0;
    mTerrain = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Terrains::OnIdle ()
{
    MeasureTime();

    APoint camPosition = mCamera->GetPosition();
    float x = camPosition[0], y = camPosition[1];
    if (MoveCamera())
    {
        // The sky dome moves with the camera so that it is always in view.
        APoint skyPosition(x, y, 0.0f);
        mSkyDome->LocalTransform.SetTranslate(skyPosition);
        mSkyDome->Update();

        // Update the active terrain pages.
        mTerrain->OnCameraMotion();
        mCuller.ComputeVisibleSet(mScene);
    }

    // Get the terrain height and normal vector and report it to the user.
    float height = mTerrain->GetHeight(x, y);
    AVector normal = mTerrain->GetNormal(x, y);
    char message[512];
    sprintf(message, "height = %f , normal = (%f,%f,%f)",
        height, normal[0], normal[1], normal[2]);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->Draw(128, GetHeight()-8, mTextColor, message);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void Terrains::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();

    // Load and initialize the sky dome.  It follows the camera.
    std::string skyMeshName = Environment::GetPathR("SkyDomePNT2.wmvf");
    Visual::PrimitiveType type;
    VertexFormat* vformat;
    VertexBuffer* vbuffer;
    IndexBuffer* ibuffer;
    Visual::LoadWMVF(skyMeshName, type, vformat, vbuffer, ibuffer);
    mSkyDome = new0 TriMesh(vformat, vbuffer, ibuffer);
    mScene->AttachChild(mSkyDome);

    APoint skyPosition = mCamera->GetPosition();
    skyPosition[2] = 0.0f;
    mSkyDome->LocalTransform.SetTranslate(skyPosition);
    mSkyDome->LocalTransform.SetUniformScale(mCamera->GetDMax());

    Texture2DEffect* skyEffect = new0 Texture2DEffect(
        Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT);
    std::string skyTextureName = Environment::GetPathR("SkyDome.wmtf");
    Texture2D* skyTexture = Texture2D::LoadWMTF(skyTextureName);
    skyTexture->GenerateMipmaps();
    mSkyDome->SetEffectInstance(skyEffect->CreateInstance(skyTexture));

    // Load the height field and create the terrain.
    vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 1);

    // For lower-resolution terrain, change the paths to Height64/Color64 or
    // Height32/Color32.
    std::string heightName = ThePath + "Data/Height128/height";
    std::string colorName = ThePath + "Data/Color128/color";

    mTerrain = new0 Terrain(heightName, vformat, mCamera);
    mScene->AttachChild(mTerrain);

    // The effect that is shared across all pages.
    std::string effectFile =
        Environment::GetPathR("BaseMulDetailFogExpSqr.wmfx");
    TerrainEffect* terrainEffect = new0 TerrainEffect(effectFile);

    std::string detailName = Environment::GetPathR("Detail.wmtf");
    Texture2D* detailTexture = Texture2D::LoadWMTF(detailName);
    detailTexture->GenerateMipmaps();

    ShaderFloat* fogColorDensity = new0 ShaderFloat(1);
    (*fogColorDensity)[0] = 0.5686f;
    (*fogColorDensity)[1] = 0.7255f;
    (*fogColorDensity)[2] = 0.8353f;
    (*fogColorDensity)[3] = 0.0015f;

    // Attach an effect to each page.  Preload all resources to video memory.
    // This will avoid frame rate stalls when new terrain pages are
    // encountered as the camera moves.
    const int numRows = mTerrain->GetRowQuantity();
    const int numCols = mTerrain->GetColQuantity();
    for (int r = 0; r < numRows; ++r)
    {
        for (int c = 0; c < numCols; ++c)
        {
            TerrainPage* page = mTerrain->GetPage(r, c);

            char suffix[32];
            sprintf(suffix, ".%d.%d.wmtf", r, c);
            std::string colorTextureName = colorName + std::string(suffix);
            Texture2D* colorTexture = Texture2D::LoadWMTF(colorTextureName);
            colorTexture->GenerateMipmaps();

            VisualEffectInstance* instance = terrainEffect->CreateInstance(
                colorTexture, detailTexture, fogColorDensity);

            page->SetEffectInstance(instance);

            mRenderer->Bind(page->GetVertexBuffer());
            mRenderer->Bind(page->GetVertexFormat());
            mRenderer->Bind(page->GetIndexBuffer());
            mRenderer->Bind(colorTexture);
        }
    }
}
//----------------------------------------------------------------------------
void Terrains::MoveForward ()
{
    WindowApplication3::MoveForward();

    APoint camPosition = mCamera->GetPosition();
    float height = mTerrain->GetHeight(camPosition[0], camPosition[1]);
    camPosition[2] = height + mHeightAboveTerrain;
    mCamera->SetPosition(camPosition);
}
//----------------------------------------------------------------------------
void Terrains::MoveBackward ()
{
    WindowApplication3::MoveBackward();

    APoint camPosition = mCamera->GetPosition();
    float height = mTerrain->GetHeight(camPosition[0], camPosition[1]);
    camPosition[2] = height + mHeightAboveTerrain;
    mCamera->SetPosition(camPosition);
}
//----------------------------------------------------------------------------
void Terrains::MoveDown ()
{
    if (mHeightAboveTerrain >= mTrnSpeed)
    {
        mHeightAboveTerrain -= mTrnSpeed;
    }

    APoint camPosition = mCamera->GetPosition();
    float height = mTerrain->GetHeight(camPosition[0], camPosition[1]);
    camPosition[2] = height + mHeightAboveTerrain;
    mCamera->SetPosition(camPosition);
}
//----------------------------------------------------------------------------
void Terrains::MoveUp ()
{
    mHeightAboveTerrain += mTrnSpeed;

    APoint camPosition = mCamera->GetPosition();
    float height = mTerrain->GetHeight(camPosition[0], camPosition[1]);
    camPosition[2] = height + mHeightAboveTerrain;
    mCamera->SetPosition(camPosition);
}
//----------------------------------------------------------------------------
