// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BillboardNodes.h"

WM5_WINDOW_APPLICATION(BillboardNodes);

//----------------------------------------------------------------------------
BillboardNodes::BillboardNodes ()
    :
    WindowApplication3("SampleGraphics/BillboardNodes",0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool BillboardNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, -1.0f, 0.25f);
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

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BillboardNodes::OnTerminate ()
{
    mScene = 0;
    mCullState = 0;
    mWireState = 0;
    mGround = 0;
    mBillboard0 = 0;
    mRectangle = 0;
    mBillboard1 = 0;
    mTorus = 0;

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
    mSSCamera = 0;
    mSSRectangle = 0;
#endif

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BillboardNodes::OnIdle ()
{
    MeasureTime();

    // Update set of visible objects.
    if (MoveCamera())
    {
        mBillboard0->Update();
        mBillboard1->Update();
        mCuller.ComputeVisibleSet(mScene);
    }
    if (MoveObject())
    {
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
    }

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
    // Compute the bounding rectangle for the torus.
    VertexBuffer* vbuffer = mTorus->GetVertexBuffer();
    char* vertices = vbuffer->GetData();
    int numVertices = vbuffer->GetNumElements();
    int stride = mTorus->GetVertexFormat()->GetStride();
    const HMatrix& worldMatrix = mTorus->WorldTransform.Matrix();

    float xmin, xmax, ymin, ymax;
    mCamera->ComputeBoundingAABB(numVertices, vertices, stride,
        worldMatrix, xmin, xmax, ymin, ymax);

    // Convert the normalized display coordinates to [0,1]^2 window
    // coordinates.
    xmin = 0.5f*(1.0f + xmin);
    xmax = 0.5f*(1.0f + xmax);
    ymin = 0.5f*(1.0f + ymin);
    ymax = 0.5f*(1.0f + ymax);

    // Update the screen rectangle.
    VertexBufferAccessor vba(mSSRectangle);
    vba.Position<Float3>(0) = Float3(xmin, ymin, 0.0f);
    vba.Position<Float3>(1) = Float3(xmax, ymin, 0.0f);
    vba.Position<Float3>(2) = Float3(xmax, ymax, 0.0f);
    vba.Position<Float3>(3) = Float3(xmin, ymax, 0.0f);
    mRenderer->Update(mSSRectangle->GetVertexBuffer());
#endif

    // Draw the scene.
    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
        // Draw the screen rectangle.
        mRenderer->SetCamera(mSSCamera);
        mCullState->Enabled = false;
        mRenderer->Draw(mSSRectangle);
        mCullState->Enabled = true;
        mRenderer->SetCamera(mCamera);
#endif

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BillboardNodes::OnKeyDown (unsigned char key, int x, int y)
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
#ifdef DEMONSTRATE_POST_PROJECTION_REFLECTION
    case 'p':
    {
        HMatrix xReflect = HMatrix::IDENTITY;
        xReflect[0][0] = -1.0f;
        mCamera->SetPostProjectionMatrix(xReflect);
        mCullState->CCWOrder = false;
        return true;
    }
    case 'P':
    {
        mCamera->SetPostProjectionMatrix(HMatrix::IDENTITY);
        mCullState->CCWOrder = true;
        return true;
    }
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void BillboardNodes::CreateScene ()
{
    mScene = new0 Node();
    mCullState = new0 CullState();
    mRenderer->SetOverrideCullState(mCullState);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // All triangle meshes have this common vertex format.  Use StandardMesh
    // to create these meshes.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh stdMesh(vformat);

    // Create the ground.  It covers a square with vertices (1,1,0), (1,-1,0),
    // (-1,1,0), and (-1,-1,0).  Multiply the texture coordinates by a factor
    // to enhance the wrap-around.
    mGround = stdMesh.Rectangle(2, 2, 16.0f, 16.0f);
    VertexBufferAccessor vba(mGround);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 128.0f;
        tcoord[1] *= 128.0f;
    }

    // Create a texture effect for the ground.
    std::string path = Environment::GetPathR("Horizontal.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT,
        Shader::SC_REPEAT);
    mGround->SetEffectInstance(instance);
    mScene->AttachChild(mGround);

    // Create a rectangle mesh.  The mesh is in the xy-plane.  Do not apply
    // local transformations to the mesh.  Use the billboard node transforms
    // to control the mesh location and orientation.
    mRectangle = stdMesh.Rectangle(2, 2, 0.125f, 0.25f);

    // Create a texture effect for the rectangle and for the torus.
    Texture2DEffect* geomEffect = new0 Texture2DEffect(Shader::SF_LINEAR);
    path = Environment::GetPathR("RedSky.wmtf");
    texture = Texture2D::LoadWMTF(path);
    mRectangle->SetEffectInstance(geomEffect->CreateInstance(texture));

    // Create a billboard node that causes a rectangle to always be facing
    // the camera.  This is the type of billboard for an avatar.
    mBillboard0 = new0 BillboardNode(mCamera);
    mBillboard0->AttachChild(mRectangle);
    mScene->AttachChild(mBillboard0);

    // The billboard rotation is about its model-space up-vector (0,1,0).  In
    // this application, world-space up is (0,0,1).  Locally rotate the
    // billboard so it's up-vector matches the world's.
    mBillboard0->LocalTransform.SetTranslate(APoint(-0.25f, 0.0f, 0.25f));
    mBillboard0->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        Mathf::HALF_PI));

    // Create a torus mesh.  Do not apply local transformations to the mesh.
    // Use the billboard node transforms to control the mesh location and
    // orientation.
    mTorus = StandardMesh(vformat, false).Torus(16, 16, 1.0f, 0.25f);
    mTorus->LocalTransform.SetUniformScale(0.1f);

    // Create a texture effect for the torus.  It uses the RedSky image that
    // the rectangle uses.
    mTorus->SetEffectInstance(geomEffect->CreateInstance(texture));

    // Create a billboard node that causes an object to always be oriented
    // the same way relative to the camera.
    mBillboard1 = new0 BillboardNode(mCamera);
    mBillboard1->AttachChild(mTorus);
    mScene->AttachChild(mBillboard1);

    // The billboard rotation is about its model-space up-vector (0,1,0).  In
    // this application, world-space up is (0,0,1).  Locally rotate the
    // billboard so it's up-vector matches the world's.
    mBillboard1->LocalTransform.SetTranslate(APoint(0.25f, 0.0f, 0.25f));
    mBillboard1->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        Mathf::HALF_PI));

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    mSSCamera = new0 Camera(false);
    mSSCamera->SetFrustum(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    mSSCamera->SetFrame(APoint::ORIGIN, AVector::UNIT_Z, AVector::UNIT_Y,
        AVector::UNIT_X);

    // Create a semitransparent screen rectangle.
    VertexFormat* ssVFormat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
    int ssVStride = ssVFormat->GetStride();

    VertexBuffer* ssVBuffer = new0 VertexBuffer(4, ssVStride);
    VertexBufferAccessor ssVba(ssVFormat, ssVBuffer);
    Float4 ssColor(0.0f, 0.0f, 1.0f, 0.25f);
    ssVba.Position<Float3>(0) = Float3(0.0f, 0.0f, 0.0f);
    ssVba.Position<Float3>(1) = Float3(1.0f, 0.0f, 0.0f);
    ssVba.Position<Float3>(2) = Float3(1.0f, 1.0f, 0.0f);
    ssVba.Position<Float3>(3) = Float3(0.0f, 1.0f, 0.0f);
    ssVba.Color<Float4>(0, 0) = ssColor;
    ssVba.Color<Float4>(0, 1) = ssColor;
    ssVba.Color<Float4>(0, 2) = ssColor;
    ssVba.Color<Float4>(0, 3) = ssColor;

    IndexBuffer* ssIBuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ssIBuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mSSRectangle = new0 TriMesh(ssVFormat, ssVBuffer, ssIBuffer);
    mSSRectangle->Update();

    // Create a vertex color effect for the screen rectangle.
    VertexColor4Effect* ssEffect = new0 VertexColor4Effect();
    mSSRectangle->SetEffectInstance(ssEffect->CreateInstance());

    // Alpha blending must be enabled to obtain the semitransparency.
    ssEffect->GetAlphaState(0, 0)->BlendEnabled = true;
#endif
}
//----------------------------------------------------------------------------
