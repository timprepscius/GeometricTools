// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MaterialTextures.h"

WM5_WINDOW_APPLICATION(MaterialTextures);

//----------------------------------------------------------------------------
MaterialTextures::MaterialTextures ()
    :
    WindowApplication3("SampleGraphics/MaterialTextures", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool MaterialTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f,1.0f,0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void MaterialTextures::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;
    mMaterial = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MaterialTextures::OnIdle ()
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
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MaterialTextures::OnKeyDown (unsigned char key, int x, int y)
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
    case '+':  // Increment the alpha value.
    case '=':
        mMaterial->Diffuse[3] += 0.1f;
        if (mMaterial->Diffuse[3] > 1.0f)
        {
            mMaterial->Diffuse[3] = 1.0f;
        }
        return true;
    case '-':  // Decrement the alpha value.
    case '_':
        mMaterial->Diffuse[3] -= 0.1f;
        if (mMaterial->Diffuse[3] < 0.0f)
        {
            mMaterial->Diffuse[3] = 0.0f;
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void MaterialTextures::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create a square object.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Float3>(0) = Float3(-1.0f, -1.0f, 0.0f);
    vba.Position<Float3>(1) = Float3(-1.0f,  1.0f, 0.0f);
    vba.Position<Float3>(2) = Float3( 1.0f,  1.0f, 0.0f);
    vba.Position<Float3>(3) = Float3( 1.0f, -1.0f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1; indices[2] = 3;
    indices[3] = 3;  indices[4] = 1; indices[5] = 2;

    // Create a square with a door texture.
    TriMesh* door = new0 TriMesh(vformat, vbuffer, ibuffer);
    std::string path = Environment::GetPathR("Door.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    door->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));
    mTrnNode->AttachChild(door);

    // Material-texture effect shared by two objects.  The material is
    // semitransparent, so alpha blending must be enabled.
    MaterialTextureEffect* effect =
        new0 MaterialTextureEffect(Shader::SF_LINEAR);
    effect->GetAlphaState(0, 0)->BlendEnabled = true;

    // Create a square with a material-texture effect. The texture is combined
    // with the material to produce a semitransparenteffect on the sand.  You
    // should be able to see the door through it.
    TriMesh* sand = new0 TriMesh(vformat, vbuffer, ibuffer);
    sand->LocalTransform.SetTranslate(APoint(0.25f, 0.25f, -0.25f));
    mTrnNode->AttachChild(sand);

    mMaterial = new0 Material();
    mMaterial->Diffuse = Float4(1.0f, 0.0f, 0.0f, 0.5f);
    path = Environment::GetPathR("Sand.wmtf");
    texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance =
        effect->CreateInstance(mMaterial, texture);
    sand->SetEffectInstance(instance);

    // The material alpha is adjustable during run time, so we must enable
    // the corresponding shader constant to automatically update.
    instance->GetVertexConstant(0, "MaterialDiffuse")->EnableUpdater();

    // Create another square with a material-texture effect.
    TriMesh* water = new0 TriMesh(vformat, vbuffer, ibuffer);
    water->LocalTransform.SetTranslate(APoint(0.5f, 0.5f, -0.5f));
    mTrnNode->AttachChild(water);

    Material* material = new0 Material();
    material->Diffuse = Float4(0.0f, 0.0f, 1.0f, 0.5f);
    path = Environment::GetPathR("Water.wmtf");
    texture = Texture2D::LoadWMTF(path);
    water->SetEffectInstance(effect->CreateInstance(material, texture));
}
//----------------------------------------------------------------------------
