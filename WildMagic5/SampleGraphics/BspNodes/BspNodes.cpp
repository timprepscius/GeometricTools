// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BspNodes.h"

WM5_WINDOW_APPLICATION(BspNodes);

//----------------------------------------------------------------------------
BspNodes::BspNodes ()
    :
    WindowApplication3("SampleGraphics/BspNodes", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f ,1.0f)
{
}
//----------------------------------------------------------------------------
bool BspNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, -1.0f, 0.1f);
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
void BspNodes::OnTerminate ()
{
    mScene = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BspNodes::OnIdle ()
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
void BspNodes::CreateScene ()
{
    // Create the scene graph.
    //
    // 1. The rectangles represent the BSP planes of the BSP tree.  They
    //    share a VertexColor3Effect.  You can see a plane from either side
    //    (backface culling disabled).  The planes do not interfere with view
    //    of the solid objects (wirestate enabled).
    //
    // 2. The sphere, tetrahedron, and cube share a TextureEffect.  These
    //    objects are convex.  The backfacing triangles are discarded
    //    (backface culling enabled).  The front facing triangles are drawn
    //    correctly by convexity, so depthbuffer reads are disabled and
    //    depthbuffer writes are enabled.  The BSP-based sorting of objects
    //    guarantees that front faces of convex objects in the foreground
    //    are drawn after the front faces of convex objects in the background,
    //    which allows us to set the depthbuffer state as we have.  That is,
    //    BSPNode sorts from back to front.
    //
    // 3. The torus has backface culling enabled and depth buffering enabled.
    //    This is necessary, because the torus is not convex.
    //
    // 4. Generally, if all objects are opaque, then you want to draw from
    //    front to back with depth buffering fully enabled.  You need to
    //    reverse-order the elements of the visible set before drawing.  If
    //    any of the objects are semitransparent, then drawing back to front
    //    is the correct order to handle transparency.  However, you do not
    //    get the benefit of early z-rejection for opaque objects.  A better
    //    BSP sorter needs to be built to produce a visible set with opaque
    //    objects listed first (front-to-back order) and semitransparent
    //    objects listed last (back-to-front order).
    //
    // scene
    //     ground
    //     bsp0
    //         bsp1
    //             bsp3
    //                 torus
    //                 rectangle3
    //                 sphere
    //             rectangle1
    //             tetrahedron
    //         rectangle0
    //         bsp2
    //             cube
    //             rectangle2
    //             octahedron

    mScene = new0 Node();

    // Create the ground.  It covers a square with vertices (1,1,0), (1,-1,0),
    // (-1,1,0), and (-1,-1,0).  Multiply the texture coordinates by a factor
    // to enhance the wrap-around.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);
    VertexBufferAccessor vba;

    TriMesh* ground = sm.Rectangle(2, 2, 16.0f, 16.0f);
    vba.ApplyTo(ground);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 128.0f;
        tcoord[1] *= 128.0f;
    }

    std::string path = Environment::GetPathR("Horizontal.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    ground->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));
    mScene->AttachChild(ground);

    // Partition the region above the ground into 5 convex pieces.  Each plane
    // is perpendicular to the ground (not required generally).
    VertexColor3Effect* vceffect = new0 VertexColor3Effect();
    vceffect->GetCullState(0, 0)->Enabled = false;
    vceffect->GetWireState(0, 0)->Enabled = true;

    Vector2f v0(-1.0f, 1.0f);
    Vector2f v1(1.0f, -1.0f);
    Vector2f v2(-0.25f, 0.25f);
    Vector2f v3(-1.0f, -1.0f);
    Vector2f v4(0.0f, 0.0f);
    Vector2f v5(1.0f, 0.5f);
    Vector2f v6(-0.75f, -7.0f/12.0f);
    Vector2f v7(-0.75f, 0.75f);
    Vector2f v8(1.0f, 1.0f);

    BspNode* bsp0 = CreateNode(v0, v1, vceffect, Float3(1.0f, 0.0f, 0.0f));
    BspNode* bsp1 = CreateNode(v2, v3, vceffect, Float3(0.0f, 0.5f, 0.0f));
    BspNode* bsp2 = CreateNode(v4, v5, vceffect, Float3(0.0f, 0.0f, 1.0f));
    BspNode* bsp3 = CreateNode(v6, v7, vceffect, Float3(0.0f, 0.0f, 0.0f));

    bsp0->AttachPositiveChild(bsp1);
    bsp0->AttachNegativeChild(bsp2);
    bsp1->AttachPositiveChild(bsp3);

    // Attach an object in each convex region.
    float height = 0.1f;
    Vector2f center;
    TriMesh* mesh;

    // The texture effect for the convex objects.
    Texture2DEffect* cvxeffect =
        new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR);
    cvxeffect->GetDepthState(0, 0)->Enabled = false;
    cvxeffect->GetDepthState(0, 0)->Writable = true;

    // The texture effect for the torus.
    Texture2DEffect* toreffect =
        new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR);

    // The texture image shared by the objects.
    path = Environment::GetPathR("Flower.wmtf");
    texture = Texture2D::LoadWMTF(path);

    // Region 0: Create a torus mesh.
    mesh = sm.Torus(16, 16, 1.0f, 0.25f);
    mesh->SetEffectInstance(toreffect->CreateInstance(texture));
    mesh->LocalTransform.SetUniformScale(0.1f);
    center = (v2 + v6 + v7)/3.0f;
    mesh->LocalTransform.SetTranslate(APoint(center[0], center[1], height));
    bsp3->AttachPositiveChild(mesh);

    // Region 1: Create a sphere mesh.
    mesh = sm.Sphere(32, 16, 1.0f);
    mesh->SetEffectInstance(cvxeffect->CreateInstance(texture));
    mesh->LocalTransform.SetUniformScale(0.1f);
    center = (v0 + v3 + v6 + v7)/4.0f;
    mesh->LocalTransform.SetTranslate(APoint(center[0], center[1], height));
    bsp3->AttachNegativeChild(mesh);

    // Region 2: Create a tetrahedron.
    mesh = sm.Tetrahedron();
    mesh->SetEffectInstance(cvxeffect->CreateInstance(texture));
    mesh->LocalTransform.SetUniformScale(0.1f);
    center = (v1 + v2 + v3)/3.0f;
    mesh->LocalTransform.SetTranslate(APoint(center[0], center[1], height));
    bsp1->AttachNegativeChild(mesh);

    // Region 3: Create a hexahedron (cube).
    mesh = sm.Hexahedron();
    mesh->SetEffectInstance(cvxeffect->CreateInstance(texture));
    mesh->LocalTransform.SetUniformScale(0.1f);
    center = (v1 + v4 + v5)/3.0f;
    mesh->LocalTransform.SetTranslate(APoint(center[0], center[1], height));
    bsp2->AttachPositiveChild(mesh);

    // Region 4: Create an octahedron.
    mesh = sm.Octahedron();
    mesh->SetEffectInstance(cvxeffect->CreateInstance(texture));
    mesh->LocalTransform.SetUniformScale(0.1f);
    center = (v0 + v4 + v5 + v8)/4.0f;
    mesh->LocalTransform.SetTranslate(APoint(center[0], center[1], height));
    bsp2->AttachNegativeChild(mesh);

    mScene->AttachChild(bsp0);
}
//----------------------------------------------------------------------------
BspNode* BspNodes::CreateNode (const Vector2f& v0, const Vector2f& v1,
    VertexColor3Effect* effect, const Float3& color)
{
    // Create the model-space separating plane.
    Vector2f dir = v1 - v0;
    AVector normal(dir[1], -dir[0], 0.0f);
    normal.Normalize();
    float constant = normal[0]*v0[0] + normal[1]*v0[1];
    HPlane modelPlane(normal, constant);

    // Create the BSP node.
    BspNode* bsp = new0 BspNode(modelPlane);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    // Create the rectangle representation of the model plane and set the
    // vertex colors to the specified color.
    float xExtent = 0.5f*dir.Length();
    float yExtent = 0.125f;
    TriMesh* rect = StandardMesh(vformat).Rectangle(2, 2, xExtent, yExtent);
    VertexBufferAccessor vba(rect);
    for (int i = 0; i < 4; ++i)
    {
        vba.Color<Float3>(0, i) = color;
    }
    rect->SetEffectInstance(effect->CreateInstance());

    // Set the position and orientation for the world-space plane.
    APoint trn(0.5f*(v0[0] + v1[0]), 0.5f*(v0[1] + v1[1]), yExtent + 0.001f);
    HMatrix zRotate(AVector::UNIT_Z, Mathf::ATan2(dir.Y(),dir.X()));
    HMatrix xRotate(AVector::UNIT_X, Mathf::HALF_PI);
    HMatrix rotate = zRotate*xRotate;
    rect->LocalTransform.SetTranslate(trn);
    rect->LocalTransform.SetRotate(rotate);

    bsp->AttachCoplanarChild(rect);
    return bsp;
}
//----------------------------------------------------------------------------
