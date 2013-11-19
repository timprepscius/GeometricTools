// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2012/07/04)

#include "Castle.h"

WM5_WINDOW_APPLICATION(Castle);

//----------------------------------------------------------------------------
Castle::Castle ()
    :
    WindowApplication3("SampleGraphics/Castle", 0, 0, 800, 600,
        Float4(0.6f, 0.851f, 0.918f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Geometry/");
    Environment::InsertDirectory(ThePath + "Shaders/");
    Environment::InsertDirectory(ThePath + "Textures/");

    mPickMessage[0] = 0;
    mVerticalDistance = 5.0f;

    // Generate pick ray information.
    mNumRays = 5;
    mHalfAngle = 0.25f*Mathf::PI;
    mCos = new1<float>(mNumRays);
    mSin = new1<float>(mNumRays);
    mTolerance = new1<float>(mNumRays);
    float mult = 1.0f/(mNumRays/2);
    for (int i = 0; i < mNumRays; ++i)
    {
        float unit = i*mult - 1.0f;  // in [-1,1]
        float angle = Mathf::HALF_PI + mHalfAngle*unit;
        mCos[i] = Mathf::Cos(angle);
        mSin[i] = Mathf::Sin(angle);
        mTolerance[i] = 2.0f - 1.5f*Mathf::FAbs(unit);  // in [1/2,1]
    }
}
//----------------------------------------------------------------------------
Castle::~Castle ()
{
    delete1(mCos);
    delete1(mSin);
    delete1(mTolerance);
}
//----------------------------------------------------------------------------
bool Castle::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    mCamera->SetFrustum(45.0f, GetAspectRatio(), 1.0f, 44495.0f);
    CreateScene();
    mScene->Update();

    // Center-and-fit the scene.  The hard-coded center/position are based
    // on a priori knowledge of the data set.
    APoint worldCenter(1.3778250f,-0.70154405f,2205.9973f);
    mTrnNode->LocalTransform.SetTranslate(-worldCenter);
    APoint camPosition(527.394f, 86.8992f, -2136.00f);
    AVector camDVector(1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    mScene->Update();

    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.5f, 0.001f);
    InitializeObjectMotion(mScene);
    MoveForward();
    return true;
}
//----------------------------------------------------------------------------
void Castle::OnTerminate ()
{
    mPicker.Records.clear();

    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mDLightRoot = 0;
    mDLight = 0;

    mOutWallMaterial = 0;
    mStoneMaterial = 0;
    mRiverMaterial = 0;
    mWallMaterial = 0;
    mStairsMaterial = 0;
    mInteriorMaterial = 0;
    mDoorMaterial = 0;
    mFloorMaterial = 0;
    mWoodCeilingMaterial = 0;
    mKeystoneMaterial = 0;
    mDrawBridgeMaterial = 0;
    mRoofMaterial = 0;
    mRampMaterial = 0;
    mWoodShieldMaterial = 0;
    mTorchHolderMaterial = 0;
    mTorchWoodMaterial = 0;
    mTorchHeadMaterial = 0;
    mBarrelBaseMaterial = 0;
    mBarrelMaterial = 0;
    mDoorFrameMaterial = 0;
    mBunkMaterial = 0;
    mBlanketMaterial = 0;
    mBenchMaterial = 0;
    mTableMaterial = 0;
    mBarrelRackMaterial = 0;
    mChestMaterial = 0;
    mLightwoodMaterial = 0;
    mMaterial26 = 0;
    mRopeMaterial = 0;
    mSquareTableMaterial = 0;
    mSimpleChairMaterial = 0;
    mMugMaterial = 0;
    mPortMaterial = 0;
    mSkyMaterial = 0;
    mWaterMaterial = 0;
    mGravel1Material = 0;
    mGravel2Material = 0;
    mGravelCornerNEMaterial = 0;
    mGravelCornerNWMaterial = 0;
    mGravelCornerSEMaterial = 0;
    mGravelCornerSWMaterial = 0;
    mGravelCapNEMaterial = 0;
    mGravelCapNWMaterial = 0;
    mGravelSideNMaterial = 0;
    mGravelSideSMaterial = 0;
    mGravelSideWMaterial = 0;
    mStone1Material = 0;
    mStone2Material = 0;
    mStone3Material = 0;
    mLargeStone1Material = 0;
    mLargerStone1Material = 0;
    mLargerStone2Material = 0;
    mLargestStone1Material = 0;
    mLargestStone2Material = 0;
    mHugeStone1Material = 0;
    mHugeStone2Material = 0;

    mOutWall = 0;
    mStone = 0;
    mRiver = 0;
    mWall = 0;
    mWallLightMap = 0;
    mSteps = 0;
    mDoor = 0;
    mFloor = 0;
    mWoodCeiling = 0;
    mKeystone = 0;
    mTilePlanks = 0;
    mRoof = 0;
    mRamp = 0;
    mShield = 0;
    mMetal = 0;
    mTorchWood = 0;
    mTorchHead = 0;
    mBarrelBase = 0;
    mBarrel = 0;
    mDoorFrame = 0;
    mBunkwood = 0;
    mBlanket = 0;
    mBench = 0;
    mTable = 0;
    mBarrelRack = 0;
    mChest = 0;
    mLightwood = 0;
    mRope = 0;
    mSquareTable = 0;
    mSimpleChair = 0;
    mMug = 0;
    mPort = 0;
    mSky = 0;
    mWater = 0;
    mGravel1 = 0;
    mGravel2 = 0;
    mGravelCornerNE = 0;
    mGravelCornerNW = 0;
    mGravelCornerSE = 0;
    mGravelCornerSW = 0;
    mGravelCapNE = 0;
    mGravelCapNW = 0;
    mGravelSideN = 0;
    mGravelSideS = 0;
    mGravelSideW = 0;
    mStone1 = 0;
    mStone2 = 0;
    mStone3 = 0;
    mLargeStone1 = 0;
    mLargerStone1 = 0;
    mLargerStone2 = 0;
    mLargestStone1 = 0;
    mLargestStone2 = 0;
    mHugeStone1 = 0;
    mHugeStone2 = 0;

    mWoodShieldMesh = 0;
    mTorchMetalMesh = 0;
    mTorchWoodMesh = 0;
    mTorchHeadMesh = 0;
    mVerticalSpoutMesh = 0;
    mHorizontalSpoutMesh = 0;
    mBarrelHolderMesh = 0;
    mBarrelMesh = 0;
    mDoorFrame01Mesh = 0;
    mDoorFrame53Mesh = 0;
    mDoorFrame61Mesh = 0;
    mDoorFrame62Mesh = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Castle::OnIdle ()
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
        if (mPickMessage[0])
        {
            mRenderer->Draw(8, 16, mTextColor, mPickMessage);
        }

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Castle::OnKeyDown (unsigned char key, int x, int y)
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
    case '+':
    case '=':
        mVerticalDistance += 0.1f;
        AdjustVerticalDistance();
        return true;
    case '-':
    case '_':
        mVerticalDistance -= 0.1f;
        AdjustVerticalDistance();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Castle::OnMouseClick (int, int state, int x, int y, unsigned int)
{
    if (state != MOUSE_DOWN && state != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    // Do a picking operation.
    APoint pos;
    AVector dir;
    if (mRenderer->GetPickRay(x, GetHeight() - 1 - y, pos, dir))
    {
        mPicker.Execute(mScene, pos, dir, 0.0f, Mathf::MAX_REAL);
        if (mPicker.Records.size() > 0)
        {
            // Display the selected object's name.
            const PickRecord& record = mPicker.GetClosestNonnegative();
            const Spatial* object = record.Intersected;
            sprintf(mPickMessage, "%s", object->GetName().c_str());
        }
        else
        {
            mPickMessage[0] = 0;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
void Castle::AdjustVerticalDistance ()
{
    // Retain vertical distance above "ground".
    APoint pos = mCamera->GetPosition();
    AVector dir = -AVector::UNIT_Z;
    mPicker.Execute(mScene, pos, dir, 0.0f, Mathf::MAX_REAL);
    if (mPicker.Records.size() > 0)
    {
        const PickRecord& record = mPicker.GetClosestNonnegative();
        TriMesh* mesh = DynamicCast<TriMesh>(record.Intersected);
        APoint tri[3];
        mesh->GetWorldTriangle(record.Triangle, tri);
        APoint closest = record.Bary[0]*tri[0] + record.Bary[1]*tri[1] +
            record.Bary[2]*tri[2];

        closest[2] += mVerticalDistance;
        mCamera->SetPosition(closest);
    }
}
//----------------------------------------------------------------------------
bool Castle::AllowMotion (float sign)
{
    // Take a step forward or backward, depending on sign.  Check if objects
    // are far enough away.  If so take the step.  If not, stay where you are.
    APoint pos = mCamera->GetPosition() + sign*mTrnSpeed*mWorldAxis[0]
        - 0.5f*mVerticalDistance*mWorldAxis[1];

    for (int i = 0; i < mNumRays; ++i)
    {
        AVector dir = mCos[i]*mWorldAxis[2] + sign*mSin[i]*mWorldAxis[0];
        mPicker.Execute(mScene, pos, dir, 0.0f, Mathf::MAX_REAL);
        if (mPicker.Records.size() > 0)
        {
            const PickRecord& record = mPicker.GetClosestNonnegative();
            if (record.T <= mTolerance[i])
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Castle::MoveForward ()
{
    if (AllowMotion(1.0f))
    {
        WindowApplication3::MoveForward();
        AdjustVerticalDistance();
    }
}
//----------------------------------------------------------------------------
void Castle::MoveBackward ()
{
    if (AllowMotion(-1.0f))
    {
        WindowApplication3::MoveBackward();
        AdjustVerticalDistance();
    }
}
//----------------------------------------------------------------------------
void Castle::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateLights();
    CreateEffects();
    CreateTextures();
    CreateSharedMeshes();

    CreateWallTurret02();
    CreateWallTurret01();
    CreateWall02();
    CreateWall01();
    CreateQuadPatch01();
    CreateMainGate01();
    CreateMainGate();
    CreateExterior();
    CreateFrontHall();
    CreateFrontRamp();
    CreateDrawBridge();
    CreateCylinder02();
    CreateBridge();
    CreateLargePort();
    CreateSmallPort(1);
    CreateSmallPort(2);
    CreateRope(1);
    CreateRope(2);

    int i;
    for (i = 1; i <= 7; ++i)
    {
        CreateWoodShield(i);
    }
    for (i = 1; i <= 17; ++i)
    {
        CreateTorch(i);
    }
    for (i = 1; i <= 3; ++i)
    {
        CreateKeg(i);
    }
    for (i = 2; i <= 37; ++i)
    {
        CreateBarrel(i);
    }
    for (i = 1; i <= 48; ++i)
    {
        CreateDoorFrame(i);
    }
    for (i = 49; i <= 60; ++i)
    {
        CreateDoorFramePivotTrn(i);
    }
    CreateDoorFrame(61);
    CreateDoorFrameScalePivotTrn(62);
    CreateDoorFrameScalePivotTrn(63);
    for (i = 64; i <= 68; ++i)
    {
        CreateDoorFrame(i);
    }
    for (i = 69; i <= 78; ++i)
    {
        CreateDoorFramePivotTrn(i);
    }
    CreateDoorFrame(79);
    CreateDoorFrameScalePivotTrn(80);
    CreateDoorFrameScalePivotTrn(81);
    CreateDoorFramePivotTrn(82);
    CreateDoorFramePivotTrn(83);
    CreateDoorFramePivotTrn(73);

    CreateBunk(1);
    for (i = 4; i <= 20; ++i)
    {
        CreateBunk(i);
    }
    for (i = 1; i <= 36; ++i)
    {
        CreateBench(i);
    }
    for (i = 1; i <= 9; ++i)
    {
        CreateTable(i);
    }
    for (i = 1; i <= 4; ++i)
    {
        CreateBarrelRack(i);
    }
    for (i = 1; i <= 36; ++i)
    {
        CreateChest(i);
    }
    for (i = 1; i <= 3; ++i)
    {
        CreateCeilingLight(i);
    }
    for (i = 1; i <= 7; ++i)
    {
        CreateSquareTable(i);
    }
    for (i = 1; i <= 27; ++i)
    {
        CreateSimpleChair(i);
    }
    for (i = 1; i <= 42; ++i)
    {
        CreateMug(i);
    }
    for (i = 1; i <= 9; ++i)
    {
        CreateDoor(i);
    } 

    CreateTerrain();
    CreateSkyDome();
    CreateWater();
    CreateWater2();
}
//----------------------------------------------------------------------------
void Castle::CreateLights ()
{
    mDLight = new0 Light(Light::LT_DIRECTIONAL);
    mDLight->Ambient = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mDLight->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mDLight->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);

    LightNode* lightNode = new0 LightNode(mDLight);
    lightNode->LocalTransform.SetTranslate(APoint(1628.448730f, -51.877197f,
        0.0f));
    lightNode->LocalTransform.SetRotate(HMatrix(AVector(-1.0f, 0.0f, 0.0f),
        Mathf::HALF_PI));

    mDLightRoot = new0 Node();
    mDLightRoot->LocalTransform.SetTranslate(APoint(-1824.998657f,
        -1531.269775f, 3886.592773f));
    mDLightRoot->LocalTransform.SetRotate(HMatrix(AVector(-0.494124f,
        0.325880f, 0.806005f), 1.371538f));

    mDLightRoot->AttachChild(lightNode);
    mDLightRoot->Update();
}
//----------------------------------------------------------------------------
void Castle::CreateEffects ()
{
    std::string name = Environment::GetPathR("DLitMatTex.wmfx");
    mDLitMatTexEffect = new0 DLitMatTexEffect(name);
    mDLitMatTexAlphaEffect = new0 DLitMatTexEffect(name);
    mDLitMatTexAlphaEffect->GetAlphaState(0, 0)->BlendEnabled = true;

    mTexEffect = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR,
        Shader::SC_REPEAT, Shader::SC_REPEAT);

    mMatEffect = new0 MaterialEffect();

    Material* common0 = new0 Material();
    common0->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    common0->Ambient = Float4(0.588235f, 0.588235f, 0.588235f, 1.0f);
    common0->Diffuse = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    common0->Specular = Float4(0.0f, 0.0f, 0.0f, 2.0f);

    Material* common1 = new0 Material();
    common1->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    common1->Ambient = Float4(0.213070f, 0.183005f, 0.064052f, 1.0f);
    common1->Diffuse = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    common1->Specular = Float4(0.045f, 0.045f, 0.045f, 5.656854f);

    Material* water = new0 Material();
    water->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    water->Ambient = Float4(0.088888f, 0.064052f, 0.181698f, 1.0f);
    water->Diffuse = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    water->Specular = Float4(0.045f, 0.045f, 0.045f, 5.656854f);

    Material* roofsteps = new0 Material();
    roofsteps->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    roofsteps->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    roofsteps->Diffuse = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    roofsteps->Specular = Float4(0.045f, 0.045f, 0.045f, 5.656854f);

    // diffuse channel is outwall03.wmtf
    mOutWallMaterial = common1;

    // diffuse channel is stone01.wmtf
    mStoneMaterial = common1;

    // diffuse channel is river01.wmtf (has alpha)
    mRiverMaterial = water;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is wall02.wmtf
    mWallMaterial = common1;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is steps.wmtf
    mStairsMaterial = roofsteps;

    // diffuse channel is outwall03.wmtf
    mInteriorMaterial = common1;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is door.wmtf
    mDoorMaterial = common0;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is floor02.wmtf
    mFloorMaterial = common0;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is woodceiling.wmtf
    mWoodCeilingMaterial = common0;

    // diffuse channel is keystone.wmtf
    mKeystoneMaterial = common1;

    // diffuse channel is tileplanks.wmtf
    mDrawBridgeMaterial = common1;

    // diffuse channel is rooftemp.wmtf
    mRoofMaterial = roofsteps;

    // diffuse channel is ramp03.wmtf
    mRampMaterial = common1;

    // diffuse channel is shield01.wmtf
    mWoodShieldMaterial = common1;

    // diffuse channel is metal01.wmtf
    mTorchHolderMaterial = new0 Material();
    mTorchHolderMaterial->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mTorchHolderMaterial->Ambient = Float4(0.213070f, 0.183005f, 0.064052f, 1.0f);
    mTorchHolderMaterial->Diffuse = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mTorchHolderMaterial->Specular = Float4(0.216f, 0.216f, 0.216f, 11.313708f);

    // diffuse channel is torchwood.wmtf
    mTorchWoodMaterial = common1;

    // emissive channel is torchhead.tga (same as .wmtf ???)
    // diffuse channel is torchhead.wmtf
    mTorchHeadMaterial = common0;

    // diffuse channel is barrelbase.wmtf
    mBarrelBaseMaterial = common0;

    // diffuse channel is barrelbase.wmtf
    mBarrelMaterial = common0;

    // emissive channel is walllightmap.wmtf
    // diffuse channel is doorframe.wmtf
    mDoorFrameMaterial = common1;

    // diffuse channel is bunkwood.wmtf
    mBunkMaterial = common1;

    // diffuse channel is blanket.wmtf
    mBlanketMaterial = common0;

    // diffuse channel is bunkwood.wmtf
    mBenchMaterial = common0;

    // diffuse channel is bunkwood.wmtf
    mTableMaterial = common0;

    mBarrelRackMaterial = mDrawBridgeMaterial;

    // diffuse channel is chest01.wmtf
    mChestMaterial = common1;

    // diffuse channel is tileplanks.wmtf
    mLightwoodMaterial = common1;

    // part of ceiling lights
    mMaterial26 = new0 Material();
    mMaterial26->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mMaterial26->Ambient = Float4(0.588235f, 0.588235f, 0.588235f, 1.0f);
    mMaterial26->Diffuse = Float4(0.588235f, 0.588235f, 0.588235f, 1.0f);
    mMaterial26->Specular = Float4(0.0f, 0.0f, 0.0f, 2.0f);

    // diffuse channel is rope.wmtf
    mRopeMaterial = common0;

    // diffuse channel is rope.wmtf
    mSquareTableMaterial = common0;

    mSimpleChairMaterial = mDrawBridgeMaterial;

    // diffuse channel is mug.wmtf
    mMugMaterial = common0;

    // diffuse channel is port.wmtf
    mPortMaterial = common1;

    // diffuse channel is skyline.wmtf
    mSkyMaterial = common0;

    // diffuse channel is river02.wmtf (has alpha)
    mWaterMaterial = water;

    // TERRAIN
    // diffuse channel is gravel01.wmtf
    mGravel1Material = common1;

    // diffuse channel is gravel02.wmtf
    mGravel2Material = common1;

    // diffuse channel is gravel_corner_ne.wmtf
    mGravelCornerNEMaterial = common1;

    // diffuse channel is gravel_corner_nw.wmtf
    mGravelCornerNWMaterial = common1;

    // diffuse channel is gravel_corner_se.wmtf
    mGravelCornerSEMaterial = common1;

    // diffuse channel is gravel_corner_sw.wmtf
    mGravelCornerSWMaterial = common1;

    // diffuse channel is gravel_cap_ne.wmtf
    mGravelCapNEMaterial = common1;

    // diffuse channel is gravel_cap_nw.wmtf
    mGravelCapNWMaterial = common1;

    // diffuse channel is gravel_side_n.wmtf
    mGravelSideNMaterial = common1;

    // diffuse channel is gravel_side_s.wmtf
    mGravelSideSMaterial = common1;

    // diffuse channel is gravel_side_w.wmtf
    mGravelSideWMaterial = common1;

    // diffuse channel is stone01.wmtf
    mStone1Material = common1;

    // diffuse channel is stone02.wmtf
    mStone2Material = common1;

    // diffuse channel is stone03.wmtf
    mStone3Material = common1;

    // diffuse channel is largestone01.wmtf
    mLargeStone1Material = common1;

    // diffuse channel is largerstone01.wmtf
    mLargerStone1Material = common1;

    // diffuse channel is largerstone02.wmtf
    mLargerStone2Material = common1;

    // diffuse channel is largeststone01.wmtf
    mLargestStone1Material = common1;

    // diffuse channel is largeststone02.wmtf
    mLargestStone2Material = common1;

    // diffuse channel is hugestone01.wmtf
    mHugeStone1Material = common1;

    // diffuse channel is hugestone02.wmtf
    mHugeStone2Material = common1;
}
//----------------------------------------------------------------------------
void Castle::CreateTextures ()
{
    std::string name = Environment::GetPathR("outwall03.wmtf");
    mOutWall = Texture2D::LoadWMTF(name);
    mOutWall->GenerateMipmaps();

    name = Environment::GetPathR("stone01.wmtf");
    mStone = Texture2D::LoadWMTF(name);
    mStone->GenerateMipmaps();

    name = Environment::GetPathR("river01.wmtf");
    mRiver = Texture2D::LoadWMTF(name);
    mRiver->GenerateMipmaps();

    name = Environment::GetPathR("wall02.wmtf");
    mWall = Texture2D::LoadWMTF(name);
    mWall->GenerateMipmaps();

    name = Environment::GetPathR("walllightmap.wmtf");
    mWallLightMap = Texture2D::LoadWMTF(name);
    mWallLightMap->GenerateMipmaps();

    name = Environment::GetPathR("steps.wmtf");
    mSteps = Texture2D::LoadWMTF(name);
    mSteps->GenerateMipmaps();

    name = Environment::GetPathR("door.wmtf");
    mDoor = Texture2D::LoadWMTF(name);
    mDoor->GenerateMipmaps();

    name = Environment::GetPathR("floor02.wmtf");
    mFloor = Texture2D::LoadWMTF(name);
    mFloor->GenerateMipmaps();

    name = Environment::GetPathR("woodceiling.wmtf");
    mWoodCeiling = Texture2D::LoadWMTF(name);
    mWoodCeiling->GenerateMipmaps();

    name = Environment::GetPathR("keystone.wmtf");
    mKeystone = Texture2D::LoadWMTF(name);
    mKeystone->GenerateMipmaps();

    name = Environment::GetPathR("tileplanks.wmtf");
    mTilePlanks = Texture2D::LoadWMTF(name);
    mTilePlanks->GenerateMipmaps();

    name = Environment::GetPathR("rooftemp.wmtf");
    mRoof = Texture2D::LoadWMTF(name);
    mRoof->GenerateMipmaps();

    name = Environment::GetPathR("ramp03.wmtf");
    mRamp = Texture2D::LoadWMTF(name);
    mRamp->GenerateMipmaps();

    name = Environment::GetPathR("shield01.wmtf");
    mShield = Texture2D::LoadWMTF(name);
    mShield->GenerateMipmaps();

    name = Environment::GetPathR("metal01.wmtf");
    mMetal = Texture2D::LoadWMTF(name);
    mMetal->GenerateMipmaps();

    name = Environment::GetPathR("torchwood.wmtf");
    mTorchWood = Texture2D::LoadWMTF(name);
    mTorchWood->GenerateMipmaps();

    name = Environment::GetPathR("torchhead.wmtf");
    mTorchHead = Texture2D::LoadWMTF(name);
    mTorchHead->GenerateMipmaps();

    name = Environment::GetPathR("barrelbase.wmtf");
    mBarrelBase = Texture2D::LoadWMTF(name);
    mBarrelBase->GenerateMipmaps();

    name = Environment::GetPathR("barrel.wmtf");
    mBarrel = Texture2D::LoadWMTF(name);
    mBarrel->GenerateMipmaps();

    name = Environment::GetPathR("doorframe.wmtf");
    mDoorFrame = Texture2D::LoadWMTF(name);
    mDoorFrame->GenerateMipmaps();

    name = Environment::GetPathR("bunkwood.wmtf");
    mBunkwood = Texture2D::LoadWMTF(name);
    mBunkwood->GenerateMipmaps();

    name = Environment::GetPathR("blanket.wmtf");
    mBlanket = Texture2D::LoadWMTF(name);
    mBlanket->GenerateMipmaps();

    mBench = mBunkwood;
    mTable = mBunkwood;
    mBarrelRack = mTilePlanks;

    name = Environment::GetPathR("chest01.wmtf");
    mChest = Texture2D::LoadWMTF(name);
    mChest->GenerateMipmaps();

    mLightwood = mTilePlanks;

    name = Environment::GetPathR("rope.wmtf");
    mRope = Texture2D::LoadWMTF(name);
    mRope->GenerateMipmaps();

    mSquareTable = mTilePlanks;
    mSimpleChair = mTilePlanks;

    name = Environment::GetPathR("mug.wmtf");
    mMug = Texture2D::LoadWMTF(name);
    mMug->GenerateMipmaps();

    name = Environment::GetPathR("port.wmtf");
    mPort = Texture2D::LoadWMTF(name);
    mPort->GenerateMipmaps();

    name = Environment::GetPathR("skyline.wmtf");
    mSky = Texture2D::LoadWMTF(name);
    mSky->GenerateMipmaps();

    name = Environment::GetPathR("river02.wmtf");
    mWater = Texture2D::LoadWMTF(name);
    mWater->GenerateMipmaps();

    // TERRAIN
    name = Environment::GetPathR("gravel01.wmtf");
    mGravel1 = Texture2D::LoadWMTF(name);
    mGravel1->GenerateMipmaps();

    name = Environment::GetPathR("gravel02.wmtf");
    mGravel2 = Texture2D::LoadWMTF(name);
    mGravel2->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_se.wmtf");
    mGravelCornerSE = Texture2D::LoadWMTF(name);
    mGravelCornerSE->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_ne.wmtf");
    mGravelCornerNE = Texture2D::LoadWMTF(name);
    mGravelCornerNE->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_nw.wmtf");
    mGravelCornerNW = Texture2D::LoadWMTF(name);
    mGravelCornerNW->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_sw.wmtf");
    mGravelCornerSW = Texture2D::LoadWMTF(name);
    mGravelCornerSW->GenerateMipmaps();

    mStone1 = mStone;

    name = Environment::GetPathR("stone02.wmtf");
    mStone2 = Texture2D::LoadWMTF(name);
    mStone2->GenerateMipmaps();

    name = Environment::GetPathR("stone03.wmtf");
    mStone3 = Texture2D::LoadWMTF(name);
    mStone3->GenerateMipmaps();

    name = Environment::GetPathR("gravel_cap_ne.wmtf");
    mGravelCapNE = Texture2D::LoadWMTF(name);
    mGravelCapNE->GenerateMipmaps();

    name = Environment::GetPathR("gravel_cap_nw.wmtf");
    mGravelCapNW = Texture2D::LoadWMTF(name);
    mGravelCapNW->GenerateMipmaps();

    name = Environment::GetPathR("gravel_side_s.wmtf");
    mGravelSideS = Texture2D::LoadWMTF(name);
    mGravelSideS->GenerateMipmaps();

    name = Environment::GetPathR("gravel_side_n.wmtf");
    mGravelSideN = Texture2D::LoadWMTF(name);
    mGravelSideN->GenerateMipmaps();

    name = Environment::GetPathR("gravel_side_w.wmtf");
    mGravelSideW = Texture2D::LoadWMTF(name);
    mGravelSideW->GenerateMipmaps();

    name = Environment::GetPathR("largestone01.wmtf");
    mLargeStone1 = Texture2D::LoadWMTF(name);
    mLargeStone1->GenerateMipmaps();

    name = Environment::GetPathR("largerstone01.wmtf");
    mLargerStone1 = Texture2D::LoadWMTF(name);
    mLargerStone1->GenerateMipmaps();

    name = Environment::GetPathR("largerstone02.wmtf");
    mLargerStone2 = Texture2D::LoadWMTF(name);
    mLargerStone2->GenerateMipmaps();

    name = Environment::GetPathR("largeststone01.wmtf");
    mLargestStone1 = Texture2D::LoadWMTF(name);
    mLargestStone1->GenerateMipmaps();

    name = Environment::GetPathR("largeststone02.wmtf");
    mLargestStone2 = Texture2D::LoadWMTF(name);
    mLargestStone2->GenerateMipmaps();

    name = Environment::GetPathR("hugestone01.wmtf");
    mHugeStone1 = Texture2D::LoadWMTF(name);
    mHugeStone1->GenerateMipmaps();

    name = Environment::GetPathR("hugestone02.wmtf");
    mHugeStone2 = Texture2D::LoadWMTF(name);
    mHugeStone2->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_nw.wmtf");
    mGravelCornerNW = Texture2D::LoadWMTF(name);
    mGravelCornerNW->GenerateMipmaps();

    name = Environment::GetPathR("gravel_corner_sw.wmtf");
    mGravelCornerSW = Texture2D::LoadWMTF(name);
    mGravelCornerSW->GenerateMipmaps();
}
//----------------------------------------------------------------------------
void Castle::CreateSharedMeshes ()
{
    std::vector<TriMesh*> meshes;

    mWoodShieldMesh = LoadMeshPNT1("WoodShield01.txt");

    mTorchMetalMesh = LoadMeshPNT1("Tube01.txt");
    meshes = LoadMeshPNT1Multi("Sphere01.txt");
    mTorchWoodMesh = meshes[0];
    mTorchHeadMesh = meshes[1];

    mVerticalSpoutMesh = LoadMeshPNT1("Cylinder03.txt");
    mHorizontalSpoutMesh = LoadMeshPNT1("Cylinder02NCL.txt");
    mBarrelHolderMesh = LoadMeshPNT1("Box01.txt");
    mBarrelMesh = LoadMeshPNT1("Barrel01.txt");
}
//----------------------------------------------------------------------------
