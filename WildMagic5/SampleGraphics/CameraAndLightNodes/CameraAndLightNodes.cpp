// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "CameraAndLightNodes.h"

WM5_WINDOW_APPLICATION(CameraAndLightNodes);

//----------------------------------------------------------------------------
CameraAndLightNodes::CameraAndLightNodes ()
    :
    WindowApplication3("SampleGraphics/CameraAndLightNodes", 0, 0, 640, 480,
        Float4(0.5f, 0.5f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool CameraAndLightNodes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // scene -+--> groundPoly
    //        |
    //        +--> cameraNode --+--> lightFixture0 +--> lightNode0
    //                          |                  |
    //                          |                  +--> lightTarget0
    //                          |
    //                          +--> lightFixture1 +--> lightNode1
    //                                             |
    //                                             +--> lightTarget0

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, -100.0f, 5.0f);
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

    InitializeCameraMotion(0.01f, 0.001f);
    return true;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mCNode = 0;
    mScreenCamera = 0;
    mSky = 0;
    mAdjustableLight0 = 0;
    mAdjustableLight1 = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->SetCamera(mScreenCamera);
        mRenderer->Draw(mSky);

        mRenderer->SetCamera(mCamera);
        mRenderer->Draw(mCuller.GetVisibleSet());

        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool CameraAndLightNodes::OnKeyDown (unsigned char key, int x, int y)
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

    case '+':  // increase light intensity
    case '=':
        mAdjustableLight0->Intensity += 0.1f;
        mAdjustableLight1->Intensity += 0.1f;
        return true;

    case '-':  // decrease light intensity
    case '_':
        if (mAdjustableLight0->Intensity >= 0.1f)
        {
            mAdjustableLight0->Intensity -= 0.1f;
        }
        if (mAdjustableLight1->Intensity >= 0.1f)
        {
            mAdjustableLight1->Intensity -= 0.1f;
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::CreateScene ()
{
    CreateScreenPolygon();

    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    TriMesh* ground = CreateGround();
    mScene->AttachChild(ground);
    mCNode = new0 CameraNode(mCamera);
    mScene->AttachChild(mCNode);

    Node* lightFixture0 = CreateLightFixture(mAdjustableLight0);
    lightFixture0->LocalTransform.SetTranslate(APoint(25.0f, -5.75f, 6.0f));
    lightFixture0->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        -Mathf::HALF_PI));
    mCNode->AttachChild(lightFixture0);

    Node* lightFixture1 = CreateLightFixture(mAdjustableLight1);
    lightFixture1->LocalTransform.SetTranslate(APoint(25.0f, -5.75f, -6.0f));
    lightFixture1->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        -Mathf::HALF_PI));
    mCNode->AttachChild(lightFixture1);
}
//----------------------------------------------------------------------------
TriMesh* CameraAndLightNodes::CreateGround ()
{
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-100.0f, -100.0f, 0.0f);
    vba.Position<Vector3f>(1) = Vector3f(+100.0f, -100.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(+100.0f, +100.0f, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(-100.0f, +100.0f, 0.0f);
    vba.Normal<Vector3f>(0) = Vector3f::UNIT_Z;
    vba.Normal<Vector3f>(1) = Vector3f::UNIT_Z;
    vba.Normal<Vector3f>(2) = Vector3f::UNIT_Z;
    vba.Normal<Vector3f>(3) = Vector3f::UNIT_Z;
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(8.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(8.0f, 8.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 8.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);

    // Create an ambient light for the ground.
    Light* light = new0 Light(Light::LT_AMBIENT);
    light->Ambient = Float4(1.0f, 1.0f, 1.0f, 1.0f);

    // Create a dark material for the ground;
    Material* material = new0 Material();
    material->Ambient = Float4(0.2f, 0.2f, 0.2f, 1.0f);

    // Load a texture for the ground.
    std::string path = Environment::GetPathR("Gravel.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    texture->GenerateMipmaps();

    // Create a multipass effect that multiplies the light-material color
    // with the texture color.  TODO:  Implement a single-pass effect that
    // does the same thing.
    LightAmbEffect* effect0 = new0 LightAmbEffect();
    VisualPass* pass0 = effect0->GetPass(0, 0);

    Texture2DEffect* effect1 = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR,
        Shader::SC_REPEAT, Shader::SC_REPEAT);
    VisualPass* pass1 = effect1->GetPass(0, 0);
    AlphaState* astate = pass1->GetAlphaState();
    astate->BlendEnabled = true;
    astate->SrcBlend = AlphaState::SBM_DST_COLOR;
    astate->DstBlend = AlphaState::DBM_ZERO;

    VisualEffect* effect = new0 VisualEffect();
    VisualTechnique* technique = new0 VisualTechnique();
    effect->InsertTechnique(technique);
    technique->InsertPass(pass0);
    technique->InsertPass(pass1);

    VisualEffectInstance* instance = new0 VisualEffectInstance(effect, 0);
    PVWMatrixConstant* pvwMatrixConstant = new0 PVWMatrixConstant();
    instance->SetVertexConstant(0, 0, pvwMatrixConstant);
    instance->SetVertexConstant(0, 1,
        new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 2,
        new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 3,
        new0 LightAmbientConstant(light));
    instance->SetVertexConstant(0, 4,
        new0 LightAttenuationConstant(light));
    instance->SetVertexConstant(1, 0, pvwMatrixConstant);
    instance->SetPixelTexture(1, 0, texture);

    mesh->SetEffectInstance(instance);

    delete0(effect0);
    delete0(effect1);

    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* CameraAndLightNodes::CreateLightTarget (Light* light)
{
    // Create a parabolic rectangle patch that is illuminated by the light.
    // To hide the artifacts of vertex normal lighting on a grid, the patch
    // is slightly bent so that the intersection with a plane is nearly
    // circular.  The patch is translated slightly below the plane of the
    // ground to hide the corners and the jaggies.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0);

    // Create a flat surface.
    TriMesh* mesh = StandardMesh(vformat).Rectangle(64, 64, 8.0f, 8.0f);

    // Adjust the heights to form a paraboloid.
    VertexBufferAccessor vba(mesh);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Vector3f& pos = vba.Position<Vector3f>(i);
        pos[2] = 1.0f - (pos[0]*pos[0] + pos[1]*pos[1])/128.0f;
    }
    mesh->UpdateModelSpace(Visual::GU_NORMALS);

    // Create a semitransparent material for the patch.
    Material* material = new0 Material();
    material->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    material->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    material->Diffuse = Float4(1.0f, 0.85f, 0.75f, 0.5f);
    material->Specular = Float4(0.8f, 0.8f, 0.8f, 1.0f);

    // The incoming light is a point light, so use an effect that combines
    // this light with the specified material.
    VisualEffectInstance* instance =
        LightPntPerVerEffect::CreateUniqueInstance(light, material);
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;
    mesh->SetEffectInstance(instance);

    return mesh;
}
//----------------------------------------------------------------------------
Node* CameraAndLightNodes::CreateLightFixture (LightPtr& adjustableLight)
{
    Node* lightFixture = new0 Node();

    // A point light illuminates the target.
    Float4 cream(1.0f, 1.0f, 0.5f, 1.0f);
    Light* light = new0 Light(Light::LT_POINT);
    light->Ambient = cream;
    light->Diffuse = cream;
    light->Specular = cream;
    adjustableLight = light;

    // The target itself.
    TriMesh* lightTarget = CreateLightTarget(light);

    // Encapsulate the light in a light node.  Rotate the light node so the
    // light points downward.
    LightNode* lightNode = new0 LightNode(light);

    lightFixture->AttachChild(lightNode);
    lightFixture->AttachChild(lightTarget);

    return lightFixture;
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::CreateScreenPolygon ()
{
    // The screen camera is designed to map (x,y,z) in [0,1]^3 to (x',y,'z')
    // in [-1,1]^2 x [0,1].
    mScreenCamera = new0 Camera(false);
    mScreenCamera->SetFrustum(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    mScreenCamera->SetFrame(APoint::ORIGIN, AVector::UNIT_Z, AVector::UNIT_Y,
        AVector::UNIT_X);

    // Create a background screen polygon.
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
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mSky = new0 TriMesh(vformat, vbuffer, ibuffer);
    std::string path = Environment::GetPathR("RedSky.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mSky->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    mSky->Update();
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveForward ()
{
    APoint position = mCNode->LocalTransform.GetTranslate();
    AVector direction;
    mCNode->LocalTransform.GetRotate().GetColumn(0, direction);
    position += mTrnSpeed*direction;
    mCNode->LocalTransform.SetTranslate(position);
    mCNode->Update();
    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveBackward ()
{
    APoint position = mCNode->LocalTransform.GetTranslate();
    AVector direction;
    mCNode->LocalTransform.GetRotate().GetColumn(0, direction);
    position -= mTrnSpeed*direction;
    mCNode->LocalTransform.SetTranslate(position);
    mCNode->Update();
    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::TurnLeft ()
{
    AVector uVector;
    mCNode->LocalTransform.GetRotate().GetColumn(1, uVector);
    mCNode->LocalTransform.SetRotate(
        HMatrix(uVector, mRotSpeed)*mCNode->LocalTransform.GetRotate());
    mCNode->Update();
    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::TurnRight ()
{
    AVector uVector;
    mCNode->LocalTransform.GetRotate().GetColumn(1, uVector);
    mCNode->LocalTransform.SetRotate(
        HMatrix(uVector, -mRotSpeed)*mCNode->LocalTransform.GetRotate());
    mCNode->Update();
    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveUp ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::MoveDown ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::LookUp ()
{
    // disabled
}
//----------------------------------------------------------------------------
void CameraAndLightNodes::LookDown ()
{
    // disabled
}
//----------------------------------------------------------------------------
