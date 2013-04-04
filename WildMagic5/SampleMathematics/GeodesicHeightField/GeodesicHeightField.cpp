// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GeodesicHeightField.h"

WM5_WINDOW_APPLICATION(GeodesicHeightField);

//----------------------------------------------------------------------------
GeodesicHeightField::GeodesicHeightField ()
    :
    WindowApplication3("SampleMathematics/GeodesicHeightField", 0, 0, 640,
        480, Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mSurface = 0;
    mGeodesic = 0;
    mSelected = 0;
    mPoint[0].SetSize(2);
    mPoint[1].SetSize(2);
    mPath = 0;
    mDistance = 1.0;
    mCurvature = 0.0;
}
//----------------------------------------------------------------------------
bool GeodesicHeightField::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, -4.0f, 1.0f);
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

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void GeodesicHeightField::OnTerminate ()
{
    delete0(mSurface);
    delete0(mGeodesic);
    delete1(mPath);

    mPicker.Records.clear();

    mScene = 0;
    mWireState = 0;
    mCullState = 0;
    mMesh = 0;
    mTexture = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GeodesicHeightField::OnIdle ()
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

    if (mSelected == 2)
    {
        int currQuantity = mGeodesic->GetCurrentQuantity();
        if (currQuantity == 0)
        {
            currQuantity = mPQuantity;
        }

        // Clear the texture image to white.
        memset(mTexture->GetData(0), 0xFF, mTexture->GetNumLevelBytes(0));

        // Draw the approximate path.
        int bound0 = mTexture->GetDimension(0, 0);
        int bound1 = mTexture->GetDimension(1, 0);
        int x0 = (int)(bound0*mPath[0][0] + 0.5);
        int y0 = (int)(bound1*mPath[0][1] + 0.5);
        for (int i = 1; i < currQuantity; ++i)
        {
            int x1 = (int)(bound0*mPath[i][0] + 0.5);
            int y1 = (int)(bound1*mPath[i][1] + 0.5);
            Line2D(x0, y0, x1, y1, DrawCallback);
            x0 = x1;
            y0 = y1;
        }

        mTexture->GenerateMipmaps();
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);

        char message[256];
        sprintf(message,
            "sub = %d, ref = %d, len = %15.12lf, avgcrv = %15.12lf",
            mGeodesic->GetSubdivisionStep(),
            mGeodesic->GetRefinementStep(),
            mDistance, mCurvature/mDistance);
        mRenderer->Draw(8, 16, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool GeodesicHeightField::OnKeyDown (unsigned char key, int x, int y)
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
bool GeodesicHeightField::OnMouseClick (int button, int state, int x, int y,
    unsigned int modifiers)
{
    if ((modifiers & MODIFIER_SHIFT) == 0)
    {
        return WindowApplication3::OnMouseClick(button, state, x, y,
            modifiers);
    }

    if (state != MOUSE_DOWN || button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    // Convert to right-handed coordinates.
    y = GetHeight() - 1 - y;

    // Do a picking operation.
    APoint origin;
    AVector direction;
    mRenderer->GetPickRay(x, y, origin, direction);
    mPicker.Execute(mScene, origin, direction, 0.0f, Mathf::MAX_REAL);
    if (mPicker.Records.size() > 0)
    {
        const PickRecord& record = mPicker.GetClosestNonnegative();

        // Get the vertex indices for the picked triangle.
        int triangle = record.Triangle;
        int i0 = 3*triangle, i1 = i0 + 1, i2 = i0 + 2;
        int* indices = (int*)mMesh->GetIndexBuffer()->GetData();
        int v0 = indices[i0];
        int v1 = indices[i1];
        int v2 = indices[i2];

        // Get the texture coordinates for the point of intersection.
        VertexBufferAccessor vba(mMesh);
        Vector2f intr =
            vba.TCoord<Vector2f>(0, v0)*record.Bary[0] +
            vba.TCoord<Vector2f>(0, v1)*record.Bary[1] +
            vba.TCoord<Vector2f>(0, v2)*record.Bary[2];

        // Save the point.
        mPoint[mSelected][0] = (double)intr.X();
        mPoint[mSelected][1] = (double)intr.Y();

        // Clear the texture image to white.
        unsigned char* data = (unsigned char*)mTexture->GetData(0);
        memset(data, 0xFF, mTexture->GetNumLevelBytes(0));

        // Get an endpoint.
        int bound0 = mTexture->GetDimension(0, 0);
        int bound1 = mTexture->GetDimension(1, 0);
        int x = (int)(bound0*intr.X() + 0.5);
        int y = (int)(bound1*intr.Y() + 0.5);
        mXIntr[mSelected] = x;
        mYIntr[mSelected] = y;
        ++mSelected;

        // Mark the endpoints in black.
        for (int i = 0; i < mSelected; ++i)
        {
            int index = 4*(mXIntr[i] + bound0*mYIntr[i]);
            data[index++] = 0;
            data[index++] = 0;
            data[index++] = 0;
            data[index++] = 255;
        }

        mTexture->GenerateMipmaps();

        if (mSelected == 2)
        {
            delete1(mPath);
            mPath = 0;

            mGeodesic->ComputeGeodesic(mPoint[0], mPoint[1], mPQuantity,
                mPath);

            mSelected = 0;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void GeodesicHeightField::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    // Create the ground.  It covers a square with vertices (1,1,0), (1,-1,0),
    // (-1,1,0), and (-1,-1,0).
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    const int xSize = 64;
    const int ySize = 64;
    const float xExtent = 1.0f;
    const float yExtent = 1.0f;
    mMesh = StandardMesh(vformat).Rectangle(xSize, ySize, xExtent, yExtent);

    // Create a B-Spline height field.  The heights of the control point are
    // defined in an input file.  The input file is structured as
    //
    // numUCtrlPoints numVCtrlPoints UDegree VDegree
    // z[0][0] z[0][1] ... z[0][numV-1]
    // z[1][0] z[1][1] ... z[1][numV_1]
    // :
    // z[numU-1][0] z[numU-1][1] ... z[numU-1][numV-1]

    std::string path = Environment::GetPathR("ControlPoints.txt");
    std::ifstream inFile(path.c_str());
    int numUCtrlPoints, numVCtrlPoints, uDegree, vDegree;
    double height;
    inFile >> numUCtrlPoints;
    inFile >> numVCtrlPoints;
    inFile >> uDegree;
    inFile >> vDegree;
    Vector3d** ctrlPoints = new2<Vector3d>(numUCtrlPoints, numVCtrlPoints);

    int i;
    for (i = 0; i < numUCtrlPoints; ++i)
    {
        double u = (double)(xExtent*(-1.0f + 2.0f*i/(numUCtrlPoints-1)));
        for (int j = 0; j < numVCtrlPoints; ++j)
        {
            double v = (double)(yExtent*(-1.0f + 2.0f*j/(numVCtrlPoints-1)));
            inFile >> height;
            ctrlPoints[i][j] = Vector3d(u, v, height);
        }
    }
    inFile.close();

    mSurface = new0 BSplineRectangled(numUCtrlPoints, numVCtrlPoints,
        ctrlPoints, uDegree, vDegree, false, false, true, true);

    delete2(ctrlPoints);

    VertexBufferAccessor vba(mMesh);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        Vector3f& position = vba.Position<Vector3f>(i);
        double u = (double)((position.X() + xExtent)/(2.0f*xExtent));
        double v = (double)((position.Y() + yExtent)/(2.0f*yExtent));
        position.Z() = (float)mSurface->P(u,v).Z();
    }
    mMesh->UpdateModelSpace(Visual::GU_NORMALS);

    // Attach an effect that uses lights, material, and texture.
    mMesh->SetEffectInstance(CreateEffectInstance());

    mScene->AttachChild(mMesh);

    // Create the geodesic calculator.
    mGeodesic = new0 BSplineGeodesicd(*mSurface);
    mGeodesic->Subdivisions = 6;
    mGeodesic->Refinements = 1;
    mGeodesic->SearchRadius = 0.1;
    mGeodesic->RefineCallback = &GeodesicHeightField::RefineCallback;
    mPQuantity = (1 << mGeodesic->Subdivisions) + 1;
}
//----------------------------------------------------------------------------
VisualEffectInstance* GeodesicHeightField::CreateEffectInstance ()
{
    // Create the vertex shader.
    VertexShader* vshader = new0 VertexShader("Wm5.DLight2MatTex",
        3, 3, 16, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelNormal", Shader::VT_FLOAT3,
        Shader::VS_NORMAL);
    vshader->SetInput(2, "modelTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    vshader->SetOutput(2, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetConstant( 0, "PVWMatrix", 4);
    vshader->SetConstant( 1, "CameraModelPosition", 1);
    vshader->SetConstant( 2, "MaterialEmissive", 1);
    vshader->SetConstant( 3, "MaterialAmbient", 1);
    vshader->SetConstant( 4, "MaterialDiffuse", 1);
    vshader->SetConstant( 5, "MaterialSpecular", 1);
    vshader->SetConstant( 6, "Light0ModelDirection", 1);
    vshader->SetConstant( 7, "Light0Ambient", 1);
    vshader->SetConstant( 8, "Light0Diffuse", 1);
    vshader->SetConstant( 9, "Light0Specular", 1);
    vshader->SetConstant(10, "Light0Attenuation", 1);
    vshader->SetConstant(11, "Light1ModelDirection", 1);
    vshader->SetConstant(12, "Light1Ambient", 1);
    vshader->SetConstant(13, "Light1Diffuse", 1);
    vshader->SetConstant(14, "Light1Specular", 1);
    vshader->SetConstant(15, "Light1Attenuation", 1);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    // Create the pixel shader.
    PixelShader* pshader = new0 PixelShader("Wm5.DLight2MatTex",
        2, 1, 0, 1, false);
    pshader->SetInput(0, "vertexColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetInput(1, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "BaseSampler", Shader::ST_2D);
    pshader->SetFilter(0, Shader::SF_LINEAR /*_LINEAR */);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
    pshader->SetTextureUnits(msPTextureUnits);
    pshader->SetPrograms(msPPrograms);

    VisualPass* pass = new0 VisualPass();
    pass->SetVertexShader(vshader);
    pass->SetPixelShader(pshader);
    pass->SetAlphaState(new0 AlphaState());
    pass->SetCullState(new0 CullState());
    pass->SetDepthState(new0 DepthState());
    pass->SetOffsetState(new0 OffsetState());
    pass->SetStencilState(new0 StencilState());
    pass->SetWireState(new0 WireState());

    // Create the effect.
    VisualTechnique* technique = new0 VisualTechnique();
    technique->InsertPass(pass);
    VisualEffect* effect = new0 VisualEffect();
    effect->InsertTechnique(technique);

    // Create the material for the effect.
    Float4 black(0.0f, 0.0f, 0.0f, 1.0f);
    Float4 white(1.0f, 1.0f, 1.0f, 1.0f);
    Material* material = new0 Material();
    material->Emissive = black;
    material->Ambient = Float4(0.24725f, 0.2245f, 0.0645f, 1.0f);
    material->Diffuse = Float4(0.34615f, 0.3143f, 0.0903f, 1.0f);
    material->Specular = Float4(0.797357f, 0.723991f, 0.208006f, 83.2f);

    // Create the lights for the effect.
    Light* light0 = new0 Light(Light::LT_DIRECTIONAL);
    light0->SetDirection(AVector(0.0f, 0.0f, -1.0f));
    light0->Ambient = white;
    light0->Diffuse = white;
    light0->Specular = black;

    Light* light1 = new0 Light(Light::LT_DIRECTIONAL);
    light1->SetDirection(AVector(0.0f, 0.0f, 1.0f));
    light1->Ambient = white;
    light1->Diffuse = white;
    light1->Specular = black;

    // Create a texture for the effect.
    mTexture = new0 Texture2D(Texture::TF_A8R8G8B8, 512, 512, 0);
    unsigned char* data = (unsigned char*)mTexture->GetData(0);
    memset(data, 0xFF, mTexture->GetNumLevelBytes(0));

    // Create an instance of the effect.
    VisualEffectInstance* instance = new0 VisualEffectInstance(effect, 0);
    instance->SetVertexConstant(0, 0,
        new0 PVWMatrixConstant());
    instance->SetVertexConstant(0, 1,
        new0 CameraModelPositionConstant());
    instance->SetVertexConstant(0, 2,
        new0 MaterialEmissiveConstant(material));
    instance->SetVertexConstant(0, 3,
        new0 MaterialAmbientConstant(material));
    instance->SetVertexConstant(0, 4,
        new0 MaterialDiffuseConstant(material));
    instance->SetVertexConstant(0, 5,
        new0 MaterialSpecularConstant(material));
    instance->SetVertexConstant(0, 6,
        new0 LightModelDVectorConstant(light0));
    instance->SetVertexConstant(0, 7,
        new0 LightAmbientConstant(light0));
    instance->SetVertexConstant(0, 8,
        new0 LightDiffuseConstant(light0));
    instance->SetVertexConstant(0, 9,
        new0 LightSpecularConstant(light0));
    instance->SetVertexConstant(0, 10,
        new0 LightAttenuationConstant(light0));
    instance->SetVertexConstant(0, 11,
        new0 LightModelDVectorConstant(light1));
    instance->SetVertexConstant(0, 12,
        new0 LightAmbientConstant(light1));
    instance->SetVertexConstant(0, 13,
        new0 LightDiffuseConstant(light1));
    instance->SetVertexConstant(0, 14,
        new0 LightSpecularConstant(light1));
    instance->SetVertexConstant(0, 15,
        new0 LightAttenuationConstant(light1));

    instance->SetPixelTexture(0, 0, mTexture);

    return instance;
}
//----------------------------------------------------------------------------
void GeodesicHeightField::ComputeDistanceCurvature ()
{
    int currQuantity = mGeodesic->GetCurrentQuantity();
    if (currQuantity == 0)
    {
        currQuantity = mPQuantity;
    }

    mDistance = mGeodesic->ComputeTotalLength(currQuantity, mPath);
    mCurvature = mGeodesic->ComputeTotalCurvature(currQuantity, mPath);
}
//----------------------------------------------------------------------------
void GeodesicHeightField::DrawCallback (int x, int y)
{
    GeodesicHeightField* app = (GeodesicHeightField*)TheApplication;
    int bound0 = app->mTexture->GetDimension(0, 0);
    unsigned char* data = (unsigned char*)app->mTexture->GetData(0);

    int index = 4*(x + bound0*y);
    data[index++] = 0;
    data[index++] = 0;
    data[index++] = 0;
    data[index++] = 255;
}
//----------------------------------------------------------------------------
void GeodesicHeightField::RefineCallback ()
{
    GeodesicHeightField* app = (GeodesicHeightField*)TheApplication;
    app->ComputeDistanceCurvature();
    app->OnIdle();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int GeodesicHeightField::msDx9VRegisters[16] =
    { 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

int GeodesicHeightField::msOglVRegisters[16] =
    { 1, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

int* GeodesicHeightField::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    msDx9VRegisters,
    msDx9VRegisters,
    msDx9VRegisters,
    msOglVRegisters
};

std::string GeodesicHeightField::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "vs_1_1\n"
    "def c19, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "dcl_texcoord0 v2\n"
    "add r0.xyz, -v0, c4\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mad r1.xyz, r0.w, r0, -c9\n"
    "mad r0.xyz, r0.w, r0, -c14\n"
    "dp3 r1.w, r1, r1\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mul r0.xyz, r0.w, r0\n"
    "dp3 r0.y, v1, r0\n"
    "rsq r1.w, r1.w\n"
    "mul r1.xyz, r1.w, r1\n"
    "dp3 r1.y, v1, r1\n"
    "dp3 r1.x, v1, c9\n"
    "dp3 r0.x, v1, c14\n"
    "mov r1.z, c8.w\n"
    "mov r1.x, -r1\n"
    "lit r2.yz, r1.xyzz\n"
    "mul r1.xyz, r2.y, c7\n"
    "mov r2.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r2.xyww, r1\n"
    "mul r2.xyz, r2.z, c8\n"
    "mad r1.xyz, r2, c12, r1\n"
    "mul r1.xyz, r1, c13.w\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r2.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r2.xyz, r2, c16\n"
    "mad r2.xyz, c15, r0.xyww, r2\n"
    "mul r0.xyz, r0.z, c8\n"
    "mov r0.w, c19.x\n"
    "mad r0.xyz, r0, c17, r2\n"
    "add r1.xyz, r1, c5\n"
    "mad oD0.xyz, r0, c18.w, r1\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xy, v2\n"
    "mov oD0.w, c7\n",

    // VP_VS_2_0
    "vs_2_0\n"
    "def c19, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "dcl_texcoord0 v2\n"
    "add r0.xyz, -v0, c4\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mad r1.xyz, r0.w, r0, -c9\n"
    "mad r0.xyz, r0.w, r0, -c14\n"
    "dp3 r1.w, r1, r1\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mul r0.xyz, r0.w, r0\n"
    "dp3 r0.y, v1, r0\n"
    "rsq r1.w, r1.w\n"
    "mul r1.xyz, r1.w, r1\n"
    "dp3 r1.y, v1, r1\n"
    "dp3 r1.x, v1, c9\n"
    "dp3 r0.x, v1, c14\n"
    "mov r1.z, c8.w\n"
    "mov r1.x, -r1\n"
    "lit r2.yz, r1.xyzz\n"
    "mul r1.xyz, r2.y, c7\n"
    "mov r2.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r2.xyww, r1\n"
    "mul r2.xyz, r2.z, c8\n"
    "mad r1.xyz, r2, c12, r1\n"
    "mul r1.xyz, r1, c13.w\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r2.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r2.xyz, r2, c16\n"
    "mad r2.xyz, c15, r0.xyww, r2\n"
    "mul r0.xyz, r0.z, c8\n"
    "mov r0.w, c19.x\n"
    "mad r0.xyz, r0, c17, r2\n"
    "add r1.xyz, r1, c5\n"
    "mad oD0.xyz, r0, c18.w, r1\n"
    "mov r0.xyz, v0\n"
    "dp4 oPos.w, r0, c3\n"
    "dp4 oPos.z, r0, c2\n"
    "dp4 oPos.y, r0, c1\n"
    "dp4 oPos.x, r0, c0\n"
    "mov oT0.xy, v2\n"
    "mov oD0.w, c7\n",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_color0 o1\n"
    "dcl_texcoord0 o2\n"
    "def c19, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_normal0 v1\n"
    "dcl_texcoord0 v2\n"
    "add r0.xyz, -v0, c4\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mad r1.xyz, r0.w, r0, -c9\n"
    "mad r0.xyz, r0.w, r0, -c14\n"
    "dp3 r1.w, r1, r1\n"
    "dp3 r0.w, r0, r0\n"
    "rsq r0.w, r0.w\n"
    "mul r0.xyz, r0.w, r0\n"
    "dp3 r0.y, v1, r0\n"
    "rsq r1.w, r1.w\n"
    "mul r1.xyz, r1.w, r1\n"
    "dp3 r1.y, v1, r1\n"
    "dp3 r1.x, v1, c9\n"
    "dp3 r0.x, v1, c14\n"
    "mov r1.z, c8.w\n"
    "mov r1.x, -r1\n"
    "lit r2.yz, r1.xyzz\n"
    "mul r1.xyz, r2.y, c7\n"
    "mov r2.xyw, c6.xyzz\n"
    "mul r1.xyz, r1, c11\n"
    "mad r1.xyz, c10, r2.xyww, r1\n"
    "mul r2.xyz, r2.z, c8\n"
    "mad r1.xyz, r2, c12, r1\n"
    "mul r1.xyz, r1, c13.w\n"
    "mov r0.x, -r0\n"
    "mov r0.z, c8.w\n"
    "lit r0.yz, r0.xyzz\n"
    "mul r2.xyz, r0.y, c7\n"
    "mov r0.xyw, c6.xyzz\n"
    "mul r2.xyz, r2, c16\n"
    "mad r2.xyz, c15, r0.xyww, r2\n"
    "mul r0.xyz, r0.z, c8\n"
    "mov r0.w, c19.x\n"
    "mad r0.xyz, r0, c17, r2\n"
    "add r1.xyz, r1, c5\n"
    "mad o1.xyz, r0, c18.w, r1\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o2.xy, v2\n"
    "mov o1.w, c7\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "PARAM c[20] = { { 1 },\n"
    "		program.local[1..19] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "ADD R0.xyz, -vertex.position, c[5];\n"
    "DP3 R0.w, R0, R0;\n"
    "RSQ R0.w, R0.w;\n"
    "MAD R1.xyz, R0.w, R0, -c[10];\n"
    "MAD R0.xyz, R0.w, R0, -c[15];\n"
    "DP3 R1.w, R1, R1;\n"
    "DP3 R0.w, R0, R0;\n"
    "RSQ R0.w, R0.w;\n"
    "MUL R0.xyz, R0.w, R0;\n"
    "DP3 R0.y, vertex.normal, R0;\n"
    "RSQ R1.w, R1.w;\n"
    "MUL R1.xyz, R1.w, R1;\n"
    "DP3 R1.y, vertex.normal, R1;\n"
    "DP3 R1.x, vertex.normal, c[10];\n"
    "DP3 R0.x, vertex.normal, c[15];\n"
    "MOV R1.z, c[9].w;\n"
    "MOV R1.x, -R1;\n"
    "LIT R2.yz, R1.xyzz;\n"
    "MUL R1.xyz, R2.y, c[8];\n"
    "MOV R2.xyw, c[11].xyzz;\n"
    "MUL R1.xyz, R1, c[12];\n"
    "MAD R1.xyz, R2.xyww, c[7], R1;\n"
    "MUL R2.xyz, R2.z, c[9];\n"
    "MAD R1.xyz, R2, c[13], R1;\n"
    "MUL R1.xyz, R1, c[14].w;\n"
    "MOV R0.x, -R0;\n"
    "MOV R0.z, c[9].w;\n"
    "LIT R0.yz, R0.xyzz;\n"
    "MUL R2.xyz, R0.y, c[8];\n"
    "MOV R0.xyw, c[16].xyzz;\n"
    "MUL R2.xyz, R2, c[17];\n"
    "MAD R2.xyz, R0.xyww, c[7], R2;\n"
    "MUL R0.xyz, R0.z, c[9];\n"
    "MOV R0.w, c[0].x;\n"
    "MAD R0.xyz, R0, c[18], R2;\n"
    "ADD R1.xyz, R1, c[6];\n"
    "MAD result.color.xyz, R0, c[19].w, R1;\n"
    "MOV R0.xyz, vertex.position;\n"
    "DP4 result.position.w, R0, c[4];\n"
    "DP4 result.position.z, R0, c[3];\n"
    "DP4 result.position.y, R0, c[2];\n"
    "DP4 result.position.x, R0, c[1];\n"
    "MOV result.texcoord[0].xy, vertex.texcoord[0];\n"
    "MOV result.color.w, c[8];\n"
    "END\n"
};

int GeodesicHeightField::msAllPTextureUnits[1] = { 0 };
int* GeodesicHeightField::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string GeodesicHeightField::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "ps.1.1\n"
    "tex t0\n"
    "mul r0, t0, v0\n",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "dcl v0\n"
    "texld r0, t0, s0\n"
    "mul r0, r0, v0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v1.xy\n"
    "dcl_color0 v0\n"
    "texld r0, v1, s0\n"
    "mul oC0, r0, v0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "MUL result.color, R0, fragment.color.primary;\n"
    "END\n"
};
//----------------------------------------------------------------------------
