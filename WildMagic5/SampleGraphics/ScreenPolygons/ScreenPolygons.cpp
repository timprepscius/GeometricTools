// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#include "ScreenPolygons.h"

WM5_WINDOW_APPLICATION(ScreenPolygons);

//----------------------------------------------------------------------------
ScreenPolygons::ScreenPolygons ()
    :
    WindowApplication3("SampleGraphics/ScreenPolygons", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mAnimTime = 0.0;
    mAnimTimeDelta = 0.01;
}
//----------------------------------------------------------------------------
bool ScreenPolygons::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(80.0f, 0.0f, 23.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();
    CopyNormalToTCoord1(mScene);
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ScreenPolygons::OnTerminate ()
{
    mScene = 0;
    mScreenCamera = 0;
    mBackPoly = 0;
    mMidPoly = 0;
    mForePoly = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ScreenPolygons::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        mScene->Update();
        CopyNormalToTCoord1(mScene);
        mCuller.ComputeVisibleSet(mScene);
    }

    mRenderer->ClearDepthBuffer();
    mRenderer->ClearColorBuffer();
    if (mRenderer->PreDraw())
    {
        // Draw the background and middle polygons.
        mRenderer->SetCamera(mScreenCamera);
        mRenderer->Draw(mBackPoly);
        mRenderer->Draw(mMidPoly);

        // Draw the biped.
        mRenderer->SetCamera(mCamera);
        mRenderer->Draw(mCuller.GetVisibleSet());

        // Draw the foreground polygon last since it has transparency.
        mRenderer->SetCamera(mScreenCamera);
        mRenderer->Draw(mForePoly);

        // Text goes on top of foreground polygon.
        mRenderer->SetCamera(mCamera);
        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[128];
        sprintf(message, "linear z = %f , perspective z = %f", mLinearZ,
            mDepthZ);
        mRenderer->Draw(8, 16, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ScreenPolygons::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    const float delta = 0.00001f;
    float dMin, dMax, frustumZ;
    APoint trn;

    switch (key)
    {
    case 'g':
        mAnimTime += mAnimTimeDelta;
        mScene->Update(mAnimTime);
        CopyNormalToTCoord1(mScene);
        mCuller.ComputeVisibleSet(mScene);
        return true;
    case 'G':
        mAnimTime = 0.0;
        mScene->Update(mAnimTime);
        CopyNormalToTCoord1(mScene);
        mCuller.ComputeVisibleSet(mScene);
        return true;

    case '+':
    case '=':
        mDepthZ += delta;
        if (mDepthZ > 1.0f)
        {
            mDepthZ = 1.0f;
        }
        dMin = mCamera->GetDMin();
        dMax = mCamera->GetDMax();
        frustumZ = dMin*dMax/(dMax - (dMax - dMin)*mDepthZ);
        mLinearZ = (frustumZ - dMin)/(dMax - dMin);
        mMidPoly->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, mLinearZ));
        mMidPoly->Update();
        return true;

    case '-':
    case '_':
        mDepthZ -= delta;
        if (mDepthZ < 0.0f)
        {
            mDepthZ = 0.0f;
        }
        dMin = mCamera->GetDMin();
        dMax = mCamera->GetDMax();
        frustumZ = dMin*dMax/(dMax - (dMax - dMin)*mDepthZ);
        mLinearZ = (frustumZ - dMin)/(dMax - dMin);
        mMidPoly->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, mLinearZ));
        mMidPoly->Update();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ScreenPolygons::CreateScene ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    mScreenCamera = new0 Camera(false);
    mScreenCamera->SetFrustum(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    mScreenCamera->SetFrame(APoint::ORIGIN, AVector::UNIT_Z, AVector::UNIT_Y,
        AVector::UNIT_X);

    // Load the biped just for some model to display.
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("SkinnedBipedPNTC1.wmof");
#else
    std::string path = Environment::GetPathR("SkinnedBipedPN.be.wmof");
#endif
    InStream source;
    source.Load(path);
    mScene = DynamicCast<Node>(source.GetObjectAt(0));
    assertion(mScene != 0, "Error in biped stream.\n");

    // The background is a textured screen polygon (z = 1).
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(0.0f, 0.0f, 1.0f);
    vba.Position<Float3>(1) = Float3(1.0f, 0.0f, 1.0f);
    vba.Position<Float3>(2) = Float3(1.0f, 1.0f, 1.0f);
    vba.Position<Float3>(3) = Float3(0.0f, 1.0f, 1.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mBackPoly = new0 TriMesh(vformat, vbuffer, ibuffer);
    path = Environment::GetPathR("RedSky.wmtf");
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mBackPoly->SetEffectInstance(effect->CreateInstance(texture));

    // The middle polygon, which may be translated via '+' or '-'.
    vbuffer = new0 VertexBuffer(4, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(0.0f, 0.3f, 0.0f);
    vba.Position<Float3>(1) = Float3(1.0f, 0.3f, 0.0f);
    vba.Position<Float3>(2) = Float3(1.0f, 0.7f, 0.0f);
    vba.Position<Float3>(3) = Float3(0.0f, 0.7f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.3f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.3f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 0.7f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.7f);

    mMidPoly = new0 TriMesh(vformat, vbuffer, ibuffer);
    path = Environment::GetPathR("BallTexture.wmtf");
    texture = Texture2D::LoadWMTF(path);
    mMidPoly->SetEffectInstance(effect->CreateInstance(texture));

    mLinearZ = 1.0f;
    mDepthZ = 1.0f;
    mMidPoly->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, mLinearZ));

    // A portion of the foreground is a textured screen polygon (z = 0).
    vbuffer = new0 VertexBuffer(5, vstride);
    vba.ApplyTo(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(0.0f,  0.0f,  0.0f);
    vba.Position<Float3>(1) = Float3(0.5f,  0.0f,  0.0f);
    vba.Position<Float3>(2) = Float3(0.75f, 0.5f,  0.0f);
    vba.Position<Float3>(3) = Float3(0.5f,  0.75f, 0.0f);
    vba.Position<Float3>(4) = Float3(0.0f,  0.5f,  0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f,  0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(0.67f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f,  0.67f);
    vba.TCoord<Float2>(0, 3) = Float2(0.67f, 1.0f);
    vba.TCoord<Float2>(0, 4) = Float2(0.0f,  0.67f);

    ibuffer = new0 IndexBuffer(9, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;
    indices[6] = 0;  indices[7] = 3;  indices[8] = 4;

    mForePoly = new0 TriMesh(vformat, vbuffer, ibuffer);
    path = Environment::GetPathR("Flower.wmtf");
    Texture2DEffect* foreEffect = new0 Texture2DEffect(Shader::SF_LINEAR);
    texture = Texture2D::LoadWMTF(path);
    mForePoly->SetEffectInstance(foreEffect->CreateInstance(texture));

    // Make the foreground semitransparent.
    foreEffect->GetAlphaState(0, 0)->BlendEnabled = true;
}
//----------------------------------------------------------------------------
void ScreenPolygons::CopyNormalToTCoord1 (Object* object)
{
    TriMesh* mesh = DynamicCast<TriMesh>(object);
    if (mesh)
    {
        VertexBufferAccessor vba(mesh);
        for (int i = 0; i < vba.GetNumVertices(); ++i)
        {
            vba.TCoord<Vector3f>(1, i) = vba.Normal<Vector3f>(i);
        }
        mRenderer->Update(mesh->GetVertexBuffer());
    }

    Node* node = DynamicCast<Node>(object);
    if (node)
    {
        for (int i = 0; i < node->GetNumChildren(); ++i)
        {
            Spatial* child = node->GetChild(i);
            if (child)
            {
                CopyNormalToTCoord1(child);
            }
        }
    }
}
//----------------------------------------------------------------------------
