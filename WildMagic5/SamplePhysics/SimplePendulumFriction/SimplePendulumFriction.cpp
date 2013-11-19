// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#include "SimplePendulumFriction.h"

WM5_WINDOW_APPLICATION(SimplePendulumFriction);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
SimplePendulumFriction::SimplePendulumFriction ()
    :
    WindowApplication3("SamplePhysics/SimplePendulumFriction", 0, 0, 640, 480,
        Float4(0.819607f, 0.909803f, 0.713725f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mLastIdle = 0.0f;
    mMotionType = 0;
}
//----------------------------------------------------------------------------
bool SimplePendulumFriction::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    float angle = 0.1f*Mathf::PI;
    float cs = Mathf::Cos(angle), sn = Mathf::Sin(angle);
    APoint camPosition(23.0f, 0.0f, 8.0f);
    AVector camDVector(-cs, 0.0f, -sn);
    AVector camUVector(-sn, 0.0f, cs);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    InitializeModule();
    CreateScene();

    // Initial update of objects.
    mScene->Update();

    PhysicsTick();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mPendulum = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

#ifndef SINGLE_STEP
    PhysicsTick();
#endif
    GraphicsTick();

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool SimplePendulumFriction::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    case '0': // pendulum oscillates, but slows down slowly
        mMotionType = 0;
        InitializeModule();
        return true;

    case '1': // pendulum slows to a vertical stop, no oscillation
        mMotionType = 1;
        InitializeModule();
        return true;

    case '2': // pendulum oscillates, but slows down quickly
        mMotionType = 2;
        InitializeModule();
        return true;

#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        PhysicsTick();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::InitializeModule ()
{
    if (mMotionType == 0)
    {
        // (c/m)^2 < 4*g/L (pendulum oscillates, but slows down slowly)
        mModule.CDivM = 0.1;
        mModule.GDivL = 1.0;
    }
    else if (mMotionType == 1)
    {
        // (c/m)^2 > 4*g/L (pendulum slows to a vertical stop, no oscillation)
        mModule.CDivM = 4.0;
        mModule.GDivL = 1.0;
    }
    else
    {
        // (c/m)^2 < 4*g/L (pendulum oscillates, but slows down quickly)
        mModule.CDivM = 1.0;
        mModule.GDivL = 1.0;
    }

    double time = 0.0;
    double deltaTime = 0.001;
    double theta = 0.75;
    double thetaDot = 0.0;
    mModule.Initialize(time, deltaTime, theta, thetaDot);
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mScene->AttachChild(CreateFloor());
    mScene->AttachChild(CreatePendulum());
}
//----------------------------------------------------------------------------
TriMesh* SimplePendulumFriction::CreateFloor ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    TriMesh* floor = StandardMesh(vformat).Rectangle(2, 2, 32.0f, 32.0f);

    std::string path = Environment::GetPathR("Wood.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    floor->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    return floor;
}
//----------------------------------------------------------------------------
Node* SimplePendulumFriction::CreatePendulum ()
{
    // The normals are duplicated to texture coordinates to avoid the AMD
    // lighting problems due to use of pre-OpenGL2.x extensions.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 1,  // normals
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0);

    StandardMesh sm(vformat);

    // Pendulum rod.
    TriMesh* rod = sm.Cylinder(2, 8, 0.05f, 12.0f, true);
    rod->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 10.0f));

    // The pendulum bulb.  Start with a sphere (to get the connectivity) and
    // then adjust the vertices to form a pair of joined cones.
    TriMesh* bulb = sm.Sphere(16, 32, 2.0f);
    VertexBufferAccessor vba(bulb);
    int numVertices = vba.GetNumVertices();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        Float3& pos = vba.Position<Float3>(i);
        float r = Mathf::Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
        float z = pos[2] + 2.0f;
        if (z >= 2.0f)
        {
            z = 4.0f - r;
        }
        else
        {
            z = r;
        }
        pos[2] = z;
    }

    // Translate the pendulum joint to the origin for the purpose of
    // rotation.
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Float3>(i)[2] -= 16.0f;
    }
    bulb->UpdateModelSpace(Visual::GU_NORMALS);
    for (int i = 0; i < numVertices; ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mRenderer->Update(bulb->GetVertexBuffer());

    vba.ApplyTo(rod);
    numVertices = vba.GetNumVertices();
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Float3>(i)[2] -= 16.0f;
    }
    rod->UpdateModelSpace(Visual::GU_NORMALS);
    for (int i = 0; i < numVertices; ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mRenderer->Update(rod->GetVertexBuffer());

    // Group the objects into a single subtree.
    mPendulum = new0 Node();
    mPendulum->AttachChild(rod);
    mPendulum->AttachChild(bulb);

    // Translate back to original model position.
    mPendulum->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 16.0f));

    // Add a material for coloring.
    Float4 black(0.0f, 0.0f, 0.0f, 1.0f);
    Float4 white(1.0f, 1.0f, 1.0f, 1.0f);
    Material* material = new0 Material();
    material->Emissive = black;
    material->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    material->Diffuse = Float4(0.99607f, 0.83920f, 0.67059f, 1.0f);
    material->Specular = black;

    // Use two lights to illuminate the pendulum.
    Light* light[2];
    light[0] = new0 Light(Light::LT_DIRECTIONAL);
    light[0]->Ambient = white;
    light[0]->Diffuse = white;
    light[0]->Specular = black;
    light[0]->SetDirection(AVector(-1.0f, -1.0f, 0.0f));

    light[1] = new0 Light(Light::LT_DIRECTIONAL);
    light[1]->Ambient = white;
    light[1]->Diffuse = white;
    light[1]->Specular = black;
    light[1]->SetDirection(AVector(+1.0f, -1.0f, 0.0f));

    // TODO:  The following code is used to piece together an effect with
    // two passes.  It is better to write an effect whose vertex shader
    // has constants corresponding to the two lights (for a single-pass
    // effect).
    LightDirPerVerEffect* effect = new0 LightDirPerVerEffect();
    VisualTechnique* technique = effect->GetTechnique(0);
    VisualPass* pass0 = technique->GetPass(0);
    VisualPass* pass1 = new0 VisualPass();
    pass1->SetVertexShader(pass0->GetVertexShader());
    pass1->SetPixelShader(pass0->GetPixelShader());
    AlphaState* astate = new0 AlphaState();
    astate->BlendEnabled = true;
    astate->SrcBlend = AlphaState::SBM_ONE;
    astate->DstBlend = AlphaState::DBM_ONE;
    pass1->SetAlphaState(astate);
    pass1->SetCullState(pass0->GetCullState());
    pass1->SetDepthState(pass0->GetDepthState());
    pass1->SetStencilState(pass0->GetStencilState());
    pass1->SetOffsetState(pass0->GetOffsetState());
    pass1->SetWireState(pass0->GetWireState());
    technique->InsertPass(pass1);

    VisualEffectInstance* instance = new0 VisualEffectInstance(effect, 0);
    for (int pass = 0; pass < 2; ++pass)
    {
        instance->SetVertexConstant(pass, 0,
            new0 PVWMatrixConstant());
        instance->SetVertexConstant(pass, 1,
            new0 CameraModelPositionConstant());
        instance->SetVertexConstant(pass, 2,
            new0 MaterialEmissiveConstant(material));
        instance->SetVertexConstant(pass, 3,
            new0 MaterialAmbientConstant(material));
        instance->SetVertexConstant(pass, 4,
            new0 MaterialDiffuseConstant(material));
        instance->SetVertexConstant(pass, 5,
            new0 MaterialSpecularConstant(material));
        instance->SetVertexConstant(pass, 6,
            new0 LightModelDVectorConstant(light[pass]));
        instance->SetVertexConstant(pass, 7,
            new0 LightAmbientConstant(light[pass]));
        instance->SetVertexConstant(pass, 8,
            new0 LightDiffuseConstant(light[pass]));
        instance->SetVertexConstant(pass, 9,
            new0 LightSpecularConstant(light[pass]));
        instance->SetVertexConstant(pass, 10,
            new0 LightAttenuationConstant(light[pass]));
    }

    rod->SetEffectInstance(instance);
    bulb->SetEffectInstance(instance);

    return mPendulum;
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::PhysicsTick ()
{
    mModule.Update();

    // Update the pendulum mechanism.  The pendulum rod is attached at
    // (x,y,z) = (0,0,16).  The update here has the 16 hard-coded.
    mPendulum->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        (float)mModule.GetTheta()));

    mPendulum->Update();
}
//----------------------------------------------------------------------------
void SimplePendulumFriction::GraphicsTick ()
{
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
