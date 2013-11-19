// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "CubeMaps.h"

WM5_WINDOW_APPLICATION(CubeMaps);

//----------------------------------------------------------------------------
CubeMaps::CubeMaps ()
    :
    WindowApplication3("SampleGraphics/CubeMaps", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool CubeMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    //CreateCubeTexture();

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 10.0f);
    APoint camPosition(0.0f, 0.0f, -0.85f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mSphere);
    return true;
}
//----------------------------------------------------------------------------
void CubeMaps::OnTerminate ()
{
    CubeMapEffect::DestroyUpdateMap();

    mScene = 0;
    mWireState = 0;
    mSphereCullState = 0;
    mSphere = 0;
    mCubeMapInstance = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CubeMaps::OnIdle ()
{
    MeasureTime();

    if (mRenderer->PreDraw())
    {
        if (MoveCamera())
        {
            mCuller.ComputeVisibleSet(mScene);

            if (CubeMapEffect::AllowDynamicUpdates(mCubeMapInstance))
            {
                // Cull the sphere object because it is the object that
                // reflects the environment.
                mSphere->Culling = Spatial::CULL_ALWAYS;

                // You can take a snapshot of the environment from any camera
                // position and camera orientation.  In this application, the
                // environment is always rendered from the center of the cube
                // object and using the axes of that cube for the orientation.
                CubeMapEffect::UpdateFaces(mCubeMapInstance, mRenderer, mScene,
                    APoint::ORIGIN, AVector::UNIT_Z, AVector::UNIT_Y,
                    -AVector::UNIT_X);

                // Restore the sphere object's culling state.
                mSphere->Culling = Spatial::CULL_DYNAMIC;
            }
        }

        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool CubeMaps::OnKeyDown (unsigned char key, int x, int y)
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

    case 'c':
    case 'C':
        mSphereCullState->Enabled = !mSphereCullState->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void CubeMaps::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create the walls of the cube room.  Each of the six texture images is
    // RGBA 64-by-64.
    Node* room = new0 Node();
    mScene->AttachChild(room);

    // Index buffer shared by the room walls.
    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 3;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 2;

    // The vertex format shared by the room walls.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();
    VertexBufferAccessor vba;

    // The texture effect shared by the room walls.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);

    VertexBuffer* vbuffer;
    TriMesh* wall;
    std::string textureName;

    // +x wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(+1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(1) = Float3(+1.0f, -1.0f, +1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, +1.0f, -1.0f);
    vba.Position<Float3>(3) = Float3(+1.0f, +1.0f, +1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("XpFace.wmtf");
    Texture2D* xpTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(xpTexture));

    // -x wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(-1.0f, -1.0f, +1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(2) = Float3(-1.0f, +1.0f, +1.0f);
    vba.Position<Float3>(3) = Float3(-1.0f, +1.0f, -1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("XmFace.wmtf");
    Texture2D* xmTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(xmTexture));

    // +y wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(+1.0f, +1.0f, +1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, +1.0f, +1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, +1.0f, -1.0f);
    vba.Position<Float3>(3) = Float3(-1.0f, +1.0f, -1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("YpFace.wmtf");
    Texture2D* ypTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(ypTexture));

    // -y wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(+1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, -1.0f, +1.0f);
    vba.Position<Float3>(3) = Float3(-1.0f, -1.0f, +1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("YmFace.wmtf");
    Texture2D* ymTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(ymTexture));

    // +z wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(+1.0f, -1.0f, +1.0f);
    vba.Position<Float3>(1) = Float3(-1.0f, -1.0f, +1.0f);
    vba.Position<Float3>(2) = Float3(+1.0f, +1.0f, +1.0f);
    vba.Position<Float3>(3) = Float3(-1.0f, +1.0f, +1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("ZpFace.wmtf");
    Texture2D* zpTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(zpTexture));

    // -z wall
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(-1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(1) = Float3(+1.0f, -1.0f, -1.0f);
    vba.Position<Float3>(2) = Float3(-1.0f, +1.0f, -1.0f);
    vba.Position<Float3>(3) = Float3(+1.0f, +1.0f, -1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(1.0f, 1.0f);
    wall = new0 TriMesh(vformat, vbuffer, ibuffer);
    room->AttachChild(wall);
    textureName = Environment::GetPathR("ZmFace.wmtf");
    Texture2D* zmTexture = Texture2D::LoadWMTF(textureName);
    wall->SetEffectInstance(effect->CreateInstance(zmTexture));

    // A sphere to reflect the environment via a cube map.  The colors will
    // be used to modulate the cube map texture.
    vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    vstride = vformat->GetStride();

    mSphere = StandardMesh(vformat).Sphere(64, 64, 0.125f);
    room->AttachChild(mSphere);

    // Generate random vertex colors for the sphere.  The StandardMesh class
    // produces a sphere with duplicated vertices along a longitude line.
    // This allows texture coordinates to be assigned in a manner that treats
    // the sphere as if it were a rectangle mesh.  For vertex colors, we want
    // the duplicated vertices to have the same color, so a hash table is used
    // to look up vertex colors for the duplicates.
    vba.ApplyTo(mSphere);
    std::map<Float3,Float3> dataMap;
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float3& position = vba.Position<Float3>(i);
        Float3& color = vba.Color<Float3>(0, i);
        std::map<Float3,Float3>::iterator iter = dataMap.find(position);
        if (iter != dataMap.end())
        {
            color = iter->second;
        }
        else
        {
            color[0] = 0.0f;
            color[1] = Mathf::IntervalRandom(0.5f, 0.75f);
            color[2] = Mathf::IntervalRandom(0.75f, 1.0f);
            dataMap.insert(std::make_pair(position, color));
        }
    }

    // Create the cube map and attach it to the sphere.
    std::string effectFile = Environment::GetPathR("CubeMap.wmfx");
    CubeMapEffect* cubeMapEffect = new0 CubeMapEffect(effectFile);

    ShaderFloat* reflectivity = new0 ShaderFloat(1);
    (*reflectivity)[0] = 0.5f;

    std::string cubeName = Environment::GetPathR("CubeMap.wmtf");
    TextureCube* cubeTexture = TextureCube::LoadWMTF(cubeName);
    cubeTexture->GenerateMipmaps();
    mCubeMapInstance = cubeMapEffect->CreateInstance(cubeTexture,
        reflectivity, false);

    mSphere->SetEffectInstance(mCubeMapInstance);

    // Allow culling to be disabled on the sphere so when you move inside
    // the sphere, you can see the previously hidden facets and verify that
    // the cube image for those facets is correctly oriented.
    mSphereCullState = cubeMapEffect->GetCullState(0, 0);
}
//----------------------------------------------------------------------------
void CubeMaps::CreateCubeTexture ()
{
    std::string name[6] =
    {
        "XpFace.wmtf",
        "XmFace.wmtf",
        "YpFace.wmtf",
        "YmFace.wmtf",
        "ZpFace.wmtf",
        "ZmFace.wmtf"
    };

    TextureCube* cubeTexture = new0 TextureCube(Texture::TF_A8R8G8B8, 64, 1);
    for (int face = 0; face < 6; ++face)
    {
        std::string textureName = Environment::GetPathR(name[face]);
        Texture2D* texture = Texture2D::LoadWMTF(textureName);
        char* cubeData = cubeTexture->GetData(face, 0);
        char* textData = texture->GetData(0);
        Utility::ReflectX(64, 64, 4, textData);
        Utility::ReflectY(64, 64, 4, textData);
        memcpy(cubeData, textData, texture->GetNumLevelBytes(0));
        delete0(texture);
    }

    cubeTexture->SaveWMTF("Data/CubeMap.wmtf");
    delete0(cubeTexture);
}
//----------------------------------------------------------------------------
