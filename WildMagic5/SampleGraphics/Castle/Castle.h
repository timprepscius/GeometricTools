// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef CASTLE_H
#define CASTLE_H

#include "Wm5WindowApplication3.h"
#include "DLitMatTexEffect.h"
using namespace Wm5;

class Castle : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Castle ();
    virtual ~Castle ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);

private:
    void CreateScene ();
    void CreateLights ();
    void CreateEffects ();
    void CreateTextures ();
    void CreateSharedMeshes ();

    Float4 mTextColor;
    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    Culler mCuller;

    // The scene has a directional light.
    NodePtr mDLightRoot;
    LightPtr mDLight;

    // Two basic effects are used, one for opaque geometry and one for
    // water (semitransparent geometry).
    DLitMatTexEffect* mDLitMatTexEffect;
    DLitMatTexEffect* mDLitMatTexAlphaEffect;
    Texture2DEffect* mTexEffect;
    MaterialEffect* mMatEffect;

    // Materials in the scene.
    MaterialPtr mOutWallMaterial;
    MaterialPtr mStoneMaterial;
    MaterialPtr mRiverMaterial;
    MaterialPtr mWallMaterial;
    MaterialPtr mStairsMaterial;
    MaterialPtr mInteriorMaterial;
    MaterialPtr mDoorMaterial;
    MaterialPtr mFloorMaterial;
    MaterialPtr mWoodCeilingMaterial;
    MaterialPtr mKeystoneMaterial;
    MaterialPtr mDrawBridgeMaterial;
    MaterialPtr mRoofMaterial;
    MaterialPtr mRampMaterial;
    MaterialPtr mWoodShieldMaterial;
    MaterialPtr mTorchHolderMaterial;
    MaterialPtr mTorchWoodMaterial;
    MaterialPtr mTorchHeadMaterial;
    MaterialPtr mBarrelBaseMaterial;
    MaterialPtr mBarrelMaterial;
    MaterialPtr mDoorFrameMaterial;
    MaterialPtr mBunkMaterial;
    MaterialPtr mBlanketMaterial;
    MaterialPtr mBenchMaterial;
    MaterialPtr mTableMaterial;
    MaterialPtr mBarrelRackMaterial;
    MaterialPtr mChestMaterial;
    MaterialPtr mLightwoodMaterial;
    MaterialPtr mMaterial26;
    MaterialPtr mRopeMaterial;
    MaterialPtr mSquareTableMaterial;
    MaterialPtr mSimpleChairMaterial;
    MaterialPtr mMugMaterial;
    MaterialPtr mPortMaterial;
    MaterialPtr mSkyMaterial;
    MaterialPtr mWaterMaterial;
    MaterialPtr mGravel1Material;
    MaterialPtr mGravel2Material;
    MaterialPtr mGravelCornerNEMaterial;
    MaterialPtr mGravelCornerNWMaterial;
    MaterialPtr mGravelCornerSEMaterial;
    MaterialPtr mGravelCornerSWMaterial;
    MaterialPtr mGravelCapNEMaterial;
    MaterialPtr mGravelCapNWMaterial;
    MaterialPtr mGravelSideNMaterial;
    MaterialPtr mGravelSideSMaterial;
    MaterialPtr mGravelSideWMaterial;
    MaterialPtr mStone1Material;
    MaterialPtr mStone2Material;
    MaterialPtr mStone3Material;
    MaterialPtr mLargeStone1Material;
    MaterialPtr mLargerStone1Material;
    MaterialPtr mLargerStone2Material;
    MaterialPtr mLargestStone1Material;
    MaterialPtr mLargestStone2Material;
    MaterialPtr mHugeStone1Material;
    MaterialPtr mHugeStone2Material;

    // Textures in the scene.
    Texture2DPtr mOutWall;
    Texture2DPtr mStone;
    Texture2DPtr mRiver;
    Texture2DPtr mWall;
    Texture2DPtr mWallLightMap;
    Texture2DPtr mSteps;
    Texture2DPtr mDoor;
    Texture2DPtr mFloor;
    Texture2DPtr mWoodCeiling;
    Texture2DPtr mKeystone;
    Texture2DPtr mTilePlanks;
    Texture2DPtr mRoof;
    Texture2DPtr mRamp;
    Texture2DPtr mShield;
    Texture2DPtr mMetal;
    Texture2DPtr mTorchWood;
    Texture2DPtr mTorchHead;
    Texture2DPtr mBarrelBase;
    Texture2DPtr mBarrel;
    Texture2DPtr mDoorFrame;
    Texture2DPtr mBunkwood;
    Texture2DPtr mBlanket;
    Texture2DPtr mBench;
    Texture2DPtr mTable;
    Texture2DPtr mBarrelRack;
    Texture2DPtr mChest;
    Texture2DPtr mLightwood;
    Texture2DPtr mRope;
    Texture2DPtr mSquareTable;
    Texture2DPtr mSimpleChair;
    Texture2DPtr mMug;
    Texture2DPtr mPort;
    Texture2DPtr mSky;
    Texture2DPtr mWater;
    Texture2DPtr mGravel1;
    Texture2DPtr mGravel2;
    Texture2DPtr mGravelCornerNE;
    Texture2DPtr mGravelCornerNW;
    Texture2DPtr mGravelCornerSE;
    Texture2DPtr mGravelCornerSW;
    Texture2DPtr mGravelCapNE;
    Texture2DPtr mGravelCapNW;
    Texture2DPtr mGravelSideN;
    Texture2DPtr mGravelSideS;
    Texture2DPtr mGravelSideW;
    Texture2DPtr mStone1;
    Texture2DPtr mStone2;
    Texture2DPtr mStone3;
    Texture2DPtr mLargeStone1;
    Texture2DPtr mLargerStone1;
    Texture2DPtr mLargerStone2;
    Texture2DPtr mLargestStone1;
    Texture2DPtr mLargestStone2;
    Texture2DPtr mHugeStone1;
    Texture2DPtr mHugeStone2;

    // Shared meshes.
    TriMeshPtr mWoodShieldMesh;
    TriMeshPtr mTorchMetalMesh;
    TriMeshPtr mTorchWoodMesh;
    TriMeshPtr mTorchHeadMesh;
    TriMeshPtr mVerticalSpoutMesh;
    TriMeshPtr mHorizontalSpoutMesh;
    TriMeshPtr mBarrelHolderMesh;
    TriMeshPtr mBarrelMesh;
    TriMeshPtr mDoorFrame01Mesh;
    TriMeshPtr mDoorFrame53Mesh;
    TriMeshPtr mDoorFrame61Mesh;
    TriMeshPtr mDoorFrame62Mesh;

    // Picking support to allow collision avoidance and to display mesh
    // information.
    bool AllowMotion (float sign);
    char mPickMessage[1024];
    Picker mPicker;
    int mNumRays;
    float mHalfAngle;
    float* mCos;
    float* mSin;
    float* mTolerance;

    // Arrange for the camera to be a fixed distance above the nearest object.
    void AdjustVerticalDistance ();
    virtual void MoveForward ();
    virtual void MoveBackward ();
    float mVerticalDistance;

private:
    // Support for loading data sets.
    class VertexPNT1
    {
    public:
        VertexPNT1 ();
        bool operator< (const VertexPNT1& vertex) const;
        int PIndex, NIndex, TIndex;
    };

    class VertexPNT2
    {
    public:
        VertexPNT2 ();
        bool operator< (const VertexPNT2& vertex) const;
        int PIndex, NIndex, T0Index, T1Index;
    };

    TriMesh* LoadMeshPNT1 (const std::string& name);
    TriMesh* LoadMeshPNT2 (const std::string& name);
    std::vector<TriMesh*> LoadMeshPNT1Multi (const std::string& name);

    void GetFloat3 (std::ifstream& inFile, int& numElements,
        Float3*& elements);

    void GetFloat2 (std::ifstream& inFile, int& numElements,
        Float2*& elements);

private:
    // The list of exported objects.
    void CreateWallTurret02 ();
    void CreateWallTurret01 ();
    void CreateWall02 ();
    void CreateWall01 ();
    void CreateQuadPatch01 ();
    void CreateWater ();
    void CreateWater2 ();
    void CreateMainGate01 ();
    void CreateMainGate ();
    void CreateFrontHall ();
    void CreateFrontRamp ();
    void CreateExterior ();
    void CreateDrawBridge ();
    void CreateCylinder02 ();
    void CreateBridge ();
    void CreateLargePort ();
    void CreateSmallPort (int i);
    void CreateRope (int i);
    void CreateSkyDome ();
    void CreateTerrain ();

    enum { MAX_WOODSHIELDS = 8 };  // 0 unused
    static APoint msWoodShieldTrn[MAX_WOODSHIELDS];
    static float msWoodShieldYRotate[MAX_WOODSHIELDS];
    static float msWoodShieldXRotate[MAX_WOODSHIELDS];
    void CreateWoodShield (int i);

    enum { MAX_TORCHES = 18 };  // 0 unused
    static APoint msTorchTrn[MAX_TORCHES];
    static float msTorchZAngle[MAX_TORCHES];
    void CreateTorch (int i);

    enum { MAX_KEGS = 4 };  // 0 unused
    static APoint msKegTrn[MAX_KEGS];
    static float msKegZAngle[MAX_KEGS];
    void CreateKeg (int i);

    enum { MAX_BARRELS = 38 };  // 0 and 1 not used
    static APoint msBarrelTrn[MAX_BARRELS];
    static float msBarrelZAngle[MAX_BARRELS];
    static float msBarrelYAngle[MAX_BARRELS];
    static float msBarrelXAngle[MAX_BARRELS];
    void CreateBarrel (int i);

    enum { MAX_DOORFRAMES = 84 };  // 0 unused
    static APoint msDoorFrameTrn[MAX_DOORFRAMES];
    static float msDoorFrameZAngle[MAX_DOORFRAMES];
    static APoint msDoorFramePivotTrn[35];  // index i-49
    std::string GetDoorFrameFilename (int i);
    void CreateDoorFrame (int i);  // 1 <= i <= 48, i = 61, 64 <= i <= 68, 79
    void CreateDoorFramePivotTrn (int i);  // 49 <= i <= 60, 69 <= i <= 78, 82, 83
    void CreateDoorFrameScalePivotTrn (int i);  // 62, 63, 80, 81

    enum { MAX_BUNKS = 21 };  // 0, 2, 3 unused
    static APoint msBunkTrn[MAX_BUNKS];
    static float msBunkZAngle[MAX_BUNKS];
    void CreateBunk (int i);

    enum { MAX_BENCHES = 37 };  // 0 unused
    static APoint msBenchTrn[MAX_BENCHES];
    static float msBenchZAngle[MAX_BENCHES];
    void CreateBench (int i);

    enum { MAX_TABLES = 10 };  // 0 unused
    static APoint msTableTrn[MAX_TABLES];
    static float msTableZAngle[MAX_TABLES];
    void CreateTable (int i);

    enum { MAX_BARREL_RACKS = 5 };  // 0 unused
    static APoint msBarrelRackTrn[MAX_BARREL_RACKS];
    void CreateBarrelRack (int i);

    enum { MAX_CHESTS = 37 };  // 0 unused
    static APoint msChestTrn[MAX_CHESTS];
    static float msChestZAngle[MAX_CHESTS];
    void CreateChest (int i);

    enum { MAX_CEILING_LIGHTS = 4 };  // 0 unused
    static APoint msCeilingLightTrn[MAX_CEILING_LIGHTS];
    void CreateCeilingLight (int i);

    enum { MAX_SQUARE_TABLES = 8 };  // 0 unused
    static APoint msSquareTableTrn[MAX_SQUARE_TABLES];
    static float msSquareTableZAngle[MAX_SQUARE_TABLES];
    void CreateSquareTable (int i);

    enum { MAX_SIMPLE_CHAIRS = 28 };  // 0 unused
    static APoint msSimpleChairTrn[MAX_SIMPLE_CHAIRS];
    static float msSimpleChairZAngle[MAX_SIMPLE_CHAIRS];
    void CreateSimpleChair (int i);

    enum { MAX_MUGS = 43 };  // 0 unused
    static APoint msMugTrn[MAX_MUGS];
    static float msMugZAngle[MAX_MUGS];
    void CreateMug (int i);

    enum { MAX_DOORS = 10 };  // 0 unused
    static APoint msDoorTrn[MAX_DOORS];
    static float msDoorZAngle[MAX_DOORS];
    void CreateDoor (int i);
};

WM5_REGISTER_INITIALIZE(Castle);
WM5_REGISTER_TERMINATE(Castle);

#endif
