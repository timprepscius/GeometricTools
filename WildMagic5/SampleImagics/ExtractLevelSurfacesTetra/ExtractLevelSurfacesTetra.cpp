// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ExtractLevelSurfacesTetra.h"

WM5_WINDOW_APPLICATION(ExtractLevelSurfacesTetra);

//----------------------------------------------------------------------------
ExtractLevelSurfacesTetra::ExtractLevelSurfacesTetra ()
    :
    WindowApplication3("SampleImagics/ExtractLevelSurfacesTetra", 0, 0, 640,
        480, Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool ExtractLevelSurfacesTetra::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(5.0f, 0.1f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ExtractLevelSurfacesTetra::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ExtractLevelSurfacesTetra::OnIdle ()
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
bool ExtractLevelSurfacesTetra::OnKeyDown (unsigned char key, int x, int y)
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
    }

    return false;
}
//----------------------------------------------------------------------------
void ExtractLevelSurfacesTetra::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Extract a level surface (image is 97x97x116).
    std::string imageName = Environment::GetPathR("Molecule.im");
    ImageInt3D image(imageName.c_str());

    ExtractSurfaceTetra est(image.GetBound(0), image.GetBound(1),
        image.GetBound(2), (int*)image.GetData());

    std::vector<Vector3f> vertices, normals;
    std::vector<TriangleKey> triangles;

    est.ExtractContour(64, vertices, triangles);
    est.MakeUnique(vertices, triangles);
    est.OrientTriangles(vertices, triangles, false);
    est.ComputeNormals(vertices, triangles, normals);

    // Create a triangle mesh for the surface.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();
    const int numVertices = (int)vertices.size();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Vector3f>(i) = vertices[i];
        vba.Normal<Vector3f>(i) = normals[i];
    }

    const int numTriangles = (int)triangles.size();
    const int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        *indices++ = triangles[i].V[0];
        *indices++ = triangles[i].V[1];
        *indices++ = triangles[i].V[2];
    }

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mTrnNode->AttachChild(mesh);

    // Create and attach a light effect.
    Material* material = new0 Material();
    material->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    material->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    material->Diffuse = Float4(0.99607f, 0.83920f, 0.67059f, 1.0f);
    material->Specular = Float4(0.8f, 0.8f, 0.8f, 4.0f);
    Light* light = new0 Light(Light::LT_DIRECTIONAL);
    light->Ambient = Float4(0.25f, 0.25f, 0.25f, 1.0f);
    light->Diffuse = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    light->Specular = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    light->SetDirection(AVector::UNIT_Z);

    LightDirPerVerEffect* effect = new0 LightDirPerVerEffect();
    mesh->SetEffectInstance(effect->CreateInstance(light, material));
}
//----------------------------------------------------------------------------
