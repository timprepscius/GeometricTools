// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2010/04/14)

#include "Castle.h"

//----------------------------------------------------------------------------
void Castle::CreateWallTurret02 ()
{
    TriMesh* mesh = LoadMeshPNT1("WallTurret02.txt");
    mesh->SetName("WallTurret02");
    mesh->LocalTransform.SetTranslate(
        APoint(1538.876343f, -309.239685f, 0.000023f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateWallTurret01 ()
{
    // TODO.  This data set is *nearly* the same as WallTurret02.txt.  Try
    // using WallTurret01 in both cases and see whether it works (visually).
    // There is one exta vertex and a few extra normals in WallTurret02.txt.
    TriMesh* mesh = LoadMeshPNT1("WallTurret01.txt");
    mesh->SetName("WallTurret01");
    mesh->LocalTransform.SetTranslate(
        APoint(1539.422119f, 184.323593f, 0.000023f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateWall02 ()
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(
        APoint(1482.001709f, -112.375885f, 0.000023f));
    node->LocalTransform.SetUniformScale(0.083333f);
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("Wall02.txt");
    mesh->SetName("Wall02");
    mesh->LocalTransform.SetTranslate(APoint(0.0f, -1188.0f, 0.0f));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateWall01 ()
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(
        APoint(1482.001709f, -12.375895f, 0.000023f));
    node->LocalTransform.SetUniformScale(0.083333f);
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("Wall01.txt");
    mesh->SetName("Wall01");
    mesh->LocalTransform.SetTranslate(APoint(0.0f, 1188.0f, 0.0f));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateQuadPatch01 ()
{
    TriMesh* mesh = LoadMeshPNT1("QuadPatch01.txt");
    mesh->SetName("QuadPatch01");
    mesh->LocalTransform.SetTranslate(
        APoint(2127.324951f, -844.650757f, 0.000023f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mStoneMaterial, mStone));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateWater ()
{
    // The object has two sets of texture coordinates, but only one texture
    // image (river01.wmtf).
    TriMesh* mesh = LoadMeshPNT2("Water.txt");
    mesh->SetName("Water");
    mesh->LocalTransform.SetTranslate(
        APoint(1633.769775f, -487.659180f, -12.000000f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexAlphaEffect->CreateInstance(mDLight,
        mRiverMaterial, mRiver));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateWater2 ()
{
    TriMesh* mesh = LoadMeshPNT1("Water2.txt");
    mesh->SetName("Water2");
    mesh->LocalTransform.SetTranslate(
        APoint(1633.769775f, -487.659180f, -10.000000f));
    //mesh->LocalTransform.SetUniformScale(0.083333f);  // No scale?
    mesh->SetEffectInstance(mDLitMatTexAlphaEffect->CreateInstance(mDLight,
        mWaterMaterial, mWater));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateMainGate01 ()
{
    TriMesh* mesh = LoadMeshPNT1("MainGate01.txt");
    mesh->SetName("MainGate01");
    mesh->LocalTransform.SetTranslate(
        APoint(1174.400269f, -62.375893f, 0.000023f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateMainGate ()
{
    TriMesh* mesh = LoadMeshPNT1("MainGate.txt");
    mesh->SetName("MainGate");
    mesh->LocalTransform.SetTranslate(
        APoint(1494.214722f, -62.375893f, 0.000023f));
    mesh->LocalTransform.SetUniformScale(0.083333f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateFrontHall ()
{
    // TODO.  This is a large mesh and needs to be factored to allow
    // portalizing.
    Transform local;
    local.SetTranslate(APoint(1616.844116f, -59.090065f, 0.0f));
    local.SetUniformScale(0.083333f);
    local.SetRotate(
        HMatrix(AVector::UNIT_Z, 0.000004f*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_X, -0.000004f*Mathf::DEG_TO_RAD));

    std::string name[7] =
    {
        "FrontHall.Wall",
        "FrontHall.Steps",
        "FrontHall.OutWall",
        "FrontHall.Door",
        "FrontHall.Floor",
        "FrontHall.WoodCeiling",
        "FrontHall.Keystone"
    };

    Texture2D* textures[7] =
    {
        mWall, mSteps, mOutWall, mDoor, mFloor, mWoodCeiling, mKeystone
    };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("FrontHall.txt");

    for (int i = 0; i < 7; ++i)
    {
        meshes[i]->SetName(name[i]);
        meshes[i]->LocalTransform = local;
        meshes[i]->SetEffectInstance(mTexEffect->CreateInstance(textures[i]));
        mTrnNode->AttachChild(meshes[i]);
    }
}
//----------------------------------------------------------------------------
void Castle::CreateFrontRamp ()
{
    // TODO.  This is a large mesh and needs to be factored to allow
    // portalizing.  NOTE:  Same local transformation as FrontHall.
    Transform local;
    local.SetTranslate(APoint(1616.844116f, -59.090065f, 0.0f));
    local.SetUniformScale(0.083333f);
    local.SetRotate(
        HMatrix(AVector::UNIT_Z, 0.000004f*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_X, -0.000004f*Mathf::DEG_TO_RAD));

    Material* materials[7] =
    {
        mOutWallMaterial, mRoofMaterial, mRampMaterial, mKeystoneMaterial,
        mWallMaterial, mStairsMaterial, mInteriorMaterial
    };

    Texture2D* textures[7] =
    {
        mOutWall, mRoof, mRamp, mKeystone, mWall, mSteps, mOutWall
    };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("FrontRamp.txt");

    for (int i = 0; i < 7; ++i)
    {
        meshes[i]->LocalTransform = local;
        meshes[i]->SetEffectInstance(mDLitMatTexEffect->CreateInstance(
            mDLight, materials[i], textures[i]));
        mTrnNode->AttachChild(meshes[i]);
    }

    meshes[0]->SetName("FrontRamp.OutWall");
    meshes[1]->SetName("FrontRamp.Roof");
    meshes[2]->SetName("FrontRamp.Ramp");
    meshes[3]->SetName("FrontRamp.Keystone");
    meshes[4]->SetName("FrontRamp.Wall");
    meshes[5]->SetName("FrontRamp.Steps");
    meshes[6]->SetName("FrontRamp.Interior");
}
//----------------------------------------------------------------------------
void Castle::CreateExterior ()
{
    Transform local;
    local.SetTranslate(APoint(1616.844116f, -59.090065f, 0.000023f));
    local.SetUniformScale(0.083333f);
    local.SetRotate(HMatrix(AVector::UNIT_Z, 0.000004f*Mathf::DEG_TO_RAD));

    Material* materials[2] =
    {
        mOutWallMaterial, mRoofMaterial
    };

    Texture2D* textures[2] =
    {
        mOutWall, mRoof
    };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("Exterior.txt");
    for (int i = 0; i < 2; ++i)
    {
        meshes[i]->LocalTransform = local;
        meshes[i]->SetEffectInstance(mDLitMatTexEffect->CreateInstance(
            mDLight, materials[i], textures[i]));
        mTrnNode->AttachChild(meshes[i]);
    }

    meshes[0]->SetName("Exterior.Wall");
    meshes[1]->SetName("Exterior.Roof");
}
//----------------------------------------------------------------------------
void Castle::CreateDrawBridge ()
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(
        APoint(1474.214722f, -62.328590f, 0.0f));
    node->LocalTransform.SetUniformScale(0.083333f);
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("DrawBridge.txt");
    mesh->SetName("DrawBridge");
    mesh->LocalTransform.SetTranslate(
        APoint(-623.466858f, 0.000000f, -35.999718f));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mDrawBridgeMaterial, mTilePlanks));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateCylinder02 ()
{
    Transform local;
    local.SetTranslate(APoint(1779.677124f, -154.748062f, 119.166679f));
    local.SetUniformScale(0.083333f);
    local.SetRotate(
        HMatrix(AVector::UNIT_Z, 0.000004f*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_X, -0.000004f*Mathf::DEG_TO_RAD));

    Material* materials[2] =
    {
        mRampMaterial, mRoofMaterial
    };

    Texture2D* textures[2] =
    {
        mRamp, mRoof
    };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("Cylinder02.txt");
    for (int i = 0; i < 2; ++i)
    {
        meshes[i]->LocalTransform = local;
        meshes[i]->SetEffectInstance(mDLitMatTexEffect->CreateInstance(
            mDLight, materials[i], textures[i]));
        mTrnNode->AttachChild(meshes[i]);
    }

    meshes[0]->SetName("Cylinder02.Ramp");
    meshes[1]->SetName("Cylinder02.Roof");
}
//----------------------------------------------------------------------------
void Castle::CreateBridge ()
{
    TriMesh* mesh = LoadMeshPNT1("Bridge.txt");
    mesh->SetName("Bridge");
    mesh->LocalTransform.SetTranslate(
        APoint(1277.351440f, -62.214615f, -108.688896f));
    mesh->LocalTransform.SetScale(APoint(0.140000f, 0.176400f, 0.140000f));
    mesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, 90.0f*Mathf::DEG_TO_RAD));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mOutWallMaterial, mOutWall));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateLargePort ()
{
    TriMesh* mesh = LoadMeshPNT1("LargePort.txt");
    mesh->SetName("LargePort");
    mesh->LocalTransform.SetTranslate(
        APoint(1510.238281f, -62.375916f, 37.700836f));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mPortMaterial, mPort));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateSmallPort (int i)
{
    TriMesh* mesh = LoadMeshPNT1("SmallPort.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "SmallPort[" + std::string(number) + "]";
    mesh->SetName(name);
    if (i == 1)
    {
        mesh->LocalTransform.SetTranslate(
            APoint(1592.221924f, -59.090084f, 15.256536f));
    }
    else  // i == 2
    {
        mesh->LocalTransform.SetTranslate(
            APoint(1642.302490f, -59.090084f, 15.256536f));
    }
    mesh->LocalTransform.SetUniformScale(0.5f);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mPortMaterial, mPort));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateRope (int i)
{
    TriMesh* mesh = LoadMeshPNT1("Rope.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "Rope[" + std::string(number) + "]";
    mesh->SetName(name);
    if (i == 1)
    {
        mesh->LocalTransform.SetTranslate(
            APoint(1375.757080f, -91.799149f, -0.500000f));
    }
    else  // i == 2
    {
        mesh->LocalTransform.SetTranslate(
            APoint(1375.757080f, -33.001854f, -0.500000f));
    }
    mesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Y, 45.0f*Mathf::DEG_TO_RAD));
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mRopeMaterial, mRope));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateSkyDome ()
{
    TriMesh* mesh = LoadMeshPNT1("SkyDome.txt");
    mesh->SetName("SkyDome");
    mesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 200.0f));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mSky));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateTerrain ()
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(
        APoint(1696.189697f, -59.821838f, 0.5f));
    mTrnNode->AttachChild(node);

    std::string name[21] =
    {
        "Gravel1",
        "Gravel2",
        "GravelCornerSE",
        "GravelCornerNE",
        "Stone1",
        "GravelCapNE",
        "Stone2",
        "Stone3",
        "GravelSideS",
        "LargeStone1",
        "LargerStone1",
        "LargerStone2",
        "LargestStone1",
        "LargestStone2",
        "HugeStone1",
        "HugeStone2",
        "GravelCapNW",
        "GravelSideN",
        "GravelCornerNW",
        "GravelSideW",
        "GravelCornerSW"
    };

    Material* materials[21] =
    {
        mGravel1Material,
        mGravel2Material,
        mGravelCornerSEMaterial,
        mGravelCornerNEMaterial,
        mStone1Material,
        mGravelCapNEMaterial,
        mStone2Material,
        mStone3Material,
        mGravelSideSMaterial,
        mLargeStone1Material,
        mLargerStone1Material,
        mLargerStone2Material,
        mLargestStone1Material,
        mLargestStone2Material,
        mHugeStone1Material,
        mHugeStone2Material,
        mGravelCapNWMaterial,
        mGravelSideNMaterial,
        mGravelCornerNWMaterial,
        mGravelSideWMaterial,
        mGravelCornerSWMaterial
    };

    Texture2D* textures[21] =
    {
        mGravel1,
        mGravel2,
        mGravelCornerSE,
        mGravelCornerNE,
        mStone1,
        mGravelCapNE,
        mStone2,
        mStone3,
        mGravelSideS,
        mLargeStone1,
        mLargerStone1,
        mLargerStone2,
        mLargestStone1,
        mLargestStone2,
        mHugeStone1,
        mHugeStone2,
        mGravelCapNW,
        mGravelSideN,
        mGravelCornerNW,
        mGravelSideW,
        mGravelCornerSW
    };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("Terrain.txt");
    for (int i = 0; i < 21; ++i)
    {
        meshes[i]->SetName("Terrain." + name[i]);
        meshes[i]->SetEffectInstance(mDLitMatTexEffect->CreateInstance(
            mDLight, materials[i], textures[i]));
        node->AttachChild(meshes[i]);
    }
}
//----------------------------------------------------------------------------
APoint Castle::msWoodShieldTrn[MAX_WOODSHIELDS] =
{
    APoint::ORIGIN, // 0
    APoint(1900.215942f,  -19.275658f, 43.502869f), // 1
    APoint(1910.416260f,  -50.737694f, 43.502869f), // 2
    APoint(1900.215942f,  -41.892868f, 43.502869f), // 3
    APoint(1694.538574f,   35.087994f, 43.502869f), // 4
    APoint(1688.330688f,   77.849670f, 43.502869f), // 5
    APoint(1694.538574f, -153.268188f, 43.502869f), // 6 // neg scale
    APoint(1688.330688f, -196.029861f, 43.502869f)  // 7 // neg scale
};
//----------------------------------------------------------------------------
float Castle::msWoodShieldYRotate[MAX_WOODSHIELDS] =
{
    0.0f,    // 0
    -90.0f,  // 1
    -90.0f,  // 2
    -90.0f,  // 3
    -90.0f,  // 4
    -90.0f,  // 5
    90.0f,  // 6 // neg scale
    90.0f   // 7 // neg scale
};
//----------------------------------------------------------------------------
float Castle::msWoodShieldXRotate[MAX_WOODSHIELDS] =
{
    0.0f,     // 0
    0.0f,     // 1
    90.0f,    // 2
    0.0f,     // 3
    -135.0f,  // 4
    45.0f,    // 5
    -135.0f,  // 6 // neg scale
    45.0f     // 7 // neg scale
};
//----------------------------------------------------------------------------
void Castle::CreateWoodShield (int i)
{
    TriMesh* mesh = new0 TriMesh(
        mWoodShieldMesh->GetVertexFormat(),
        mWoodShieldMesh->GetVertexBuffer(),
        mWoodShieldMesh->GetIndexBuffer());

    char number[4];
    sprintf(number, "%d", i);
    std::string name = "WoodShield[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(msWoodShieldTrn[i]);
    mesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Y, msWoodShieldYRotate[i]*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_X, msWoodShieldXRotate[i]*Mathf::DEG_TO_RAD));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mShield));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msTorchTrn[MAX_TORCHES] =
{
    APoint::ORIGIN,  // 0
    APoint(1672.982910f,   57.190659f, 41.584717f), // 1
    APoint(1709.405029f,   38.335674f, 41.584717f), // 2
    APoint(1743.079346f,   67.204247f, 41.584717f), // 3
    APoint(1763.364746f,   67.204247f, 41.584717f), // 4
    APoint(1847.904907f,   66.716202f, 41.584717f), // 5
    APoint(1806.618164f,   81.815605f, 41.584717f), // 6
    APoint(1883.548096f,   69.864510f, 41.584717f), // 7
    APoint(1883.548096f,   13.333618f, 41.584717f), // 8
    APoint(1787.077148f,    8.270692f, 41.584717f), // 9
    APoint(1787.077148f,  -41.190777f, 41.584717f), // 10
    APoint(1825.526367f,  -99.599823f, 41.584717f), // 11
    APoint(1855.522949f,  -99.599823f, 41.584717f), // 12
    APoint(1904.187622f, -114.684425f, 41.584717f), // 13
    APoint(1921.073242f,  -79.277817f, 41.584717f), // 14
    APoint(1852.265869f,    0.889043f, 41.584717f), // 15
    APoint(1672.982910f, -175.370850f, 41.584717f), // 16  // neg scale
    APoint(1709.405029f, -156.515869f, 41.584717f)  // 17  // neg scale
};
//----------------------------------------------------------------------------
float Castle::msTorchZAngle[MAX_TORCHES] =
{
    0.0f,    // 0
    45.0f,   // 1
    135.0f,  // 2
    90.0f,   // 3
    90.0f,   // 4
    90.0f,   // 5
    -90.0f,  // 6
    180.0f,  // 7
    180.0f,  // 8
    0.0f,    // 9
    0.0f,    // 10
    -90.0f,  // 11
    -90.0f,  // 12
    90.0f,   // 13
    0.0f,    // 14
    -90.0f,  // 15
    -45.0f,  // 16  // neg scale
    -135.0f  // 17  // neg scale
};
//----------------------------------------------------------------------------
void Castle::CreateTorch (int i)
{
    // Node<torchNode>
    //     TriMesh<torchMetal>
    //     Node<sphereParent>
    //         TriMesh<torchWood>
    //         TriMesh<torchHead>

    Node* torchNode = new0 Node();
    torchNode->LocalTransform.SetTranslate(msTorchTrn[i]);
    torchNode->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msTorchZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(torchNode);

    TriMesh* torchMetal = new0 TriMesh(
        mTorchMetalMesh->GetVertexFormat(),
        mTorchMetalMesh->GetVertexBuffer(),
        mTorchMetalMesh->GetIndexBuffer());
    char number[4];
    sprintf(number, "%d", i);
    std::string prefix = "Torch[" + std::string(number) + "].";
    torchMetal->SetName(prefix + "Metal");
    torchMetal->LocalTransform.SetTranslate(
        APoint(-0.453491f, 0.0f, -0.771839f));
    torchMetal->LocalTransform.SetUniformScale(0.5f);
    torchMetal->SetEffectInstance(mTexEffect->CreateInstance(mMetal));
    torchNode->AttachChild(torchMetal);

    Node* sphereParent = new0 Node();
    sphereParent->LocalTransform.SetTranslate(
        APoint(-0.453491f, 0.0f, -1.878212f));
    sphereParent->LocalTransform.SetUniformScale(0.5f);
    sphereParent->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Y, 9.0f*Mathf::DEG_TO_RAD));
    torchNode->AttachChild(sphereParent);

    Transform local;
    local.SetTranslate(APoint(0.0f, 0.0f, 5.608833f));

    TriMesh* torchWood = new0 TriMesh(
        mTorchWoodMesh->GetVertexFormat(),
        mTorchWoodMesh->GetVertexBuffer(),
        mTorchWoodMesh->GetIndexBuffer());
    torchWood->SetName(prefix + "TorchWood");
    torchWood->SetEffectInstance(mTexEffect->CreateInstance(mTorchWood));
    torchWood->LocalTransform = local;
    sphereParent->AttachChild(torchWood);

    TriMesh* torchHead = new0 TriMesh(
        mTorchHeadMesh->GetVertexFormat(),
        mTorchHeadMesh->GetVertexBuffer(),
        mTorchHeadMesh->GetIndexBuffer());
    torchHead->SetName(prefix + "TorchHead");
    torchHead->SetEffectInstance(mTexEffect->CreateInstance(mTorchHead));
    torchHead->LocalTransform = local;
    sphereParent->AttachChild(torchHead);
}
//----------------------------------------------------------------------------
APoint Castle::msKegTrn[MAX_KEGS] =
{
    APoint::ORIGIN,  // 0
    APoint(1667.679810f,   69.014206f, 38.540501f),  // 1
    APoint(1929.555298f,  -20.859455f, 38.540501f),  // 2
    APoint(1667.679810f, -187.194397f, 38.540501f)   // 3
};
//----------------------------------------------------------------------------
float Castle::msKegZAngle[MAX_KEGS] =
{
    0.0f,    // 0
    45.0f,   // 1
    -90.0f,  // 2
    135.0f   // 3
};
//----------------------------------------------------------------------------
void Castle::CreateKeg (int i)
{
    // Node<kegNode>
    //     Node<verticalParent>
    //         TriMesh<verticalSpout>
    //     TriMesh<horizontalSpout>
    //     Node<holderParent>
    //         TriMesh<barrelHolder>
    //     Node<barrelParent>
    //         TriMesh<barrel>

    char number[4];
    sprintf(number, "%d", i);
    std::string prefix = "Keg[" + std::string(number) + "].";

    Node* kegNode = new0 Node();
    kegNode->LocalTransform.SetTranslate(msKegTrn[i]);
    kegNode->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msKegZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(kegNode);

    // vertical spout
    Node* verParent = new0 Node();
    verParent->LocalTransform.SetTranslate(
        APoint(0.000122f, -2.056274f, -0.176224f));
    kegNode->AttachChild(verParent);

    TriMesh* verMesh = new0 TriMesh(
        mVerticalSpoutMesh->GetVertexFormat(),
        mVerticalSpoutMesh->GetVertexBuffer(),
        mVerticalSpoutMesh->GetIndexBuffer());
    verMesh->SetName(prefix + "VerticalSpout");
    verMesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrelBase));
    verMesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, -0.541667f));
    verParent->AttachChild(verMesh);

    // horizontal spout
    TriMesh* horMesh = new0 TriMesh(
        mHorizontalSpoutMesh->GetVertexFormat(),
        mHorizontalSpoutMesh->GetVertexBuffer(),
        mHorizontalSpoutMesh->GetIndexBuffer());
    horMesh->SetName(prefix + "HorizontalSpout");
    horMesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrelBase));
    horMesh->LocalTransform.SetTranslate(
        APoint(0.000000f, -1.458740f, -0.156971f));
    horMesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_X, 89.999990f*Mathf::DEG_TO_RAD));
    kegNode->AttachChild(horMesh);

    // barrel holder
    Node* holderParent = new0 Node();
    holderParent->LocalTransform.SetTranslate(
        APoint(0.000000f, 0.295288f, -2.540508f));
    holderParent->LocalTransform.SetScale(
        APoint(0.659961f, 0.781250f, 0.884956f));
    kegNode->AttachChild(holderParent);

    TriMesh* holderMesh = new0 TriMesh(
        mBarrelHolderMesh->GetVertexFormat(),
        mBarrelHolderMesh->GetVertexBuffer(),
        mBarrelHolderMesh->GetIndexBuffer());
    holderMesh->SetName(prefix + "BarrelHolder");
    holderMesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrelBase));
    holderMesh->LocalTransform.SetTranslate(APoint(0.0f, -1.5f, 0.0f));
    holderParent->AttachChild(holderMesh);

    // barrel
    Node* barrelParent = new0 Node();
    barrelParent->LocalTransform.SetTranslate(
        APoint(0.0f, 0.295288f, -0.863632f));
    barrelParent->LocalTransform.SetScale(
        APoint(0.677369f, 0.677369f, 0.637836f));
    barrelParent->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_X, 89.999990f*Mathf::DEG_TO_RAD));
    kegNode->AttachChild(barrelParent);

    TriMesh* barrelMesh = new0 TriMesh(
        mBarrelMesh->GetVertexFormat(),
        mBarrelMesh->GetVertexBuffer(),
        mBarrelMesh->GetIndexBuffer());
    barrelMesh->SetName(prefix + "Barrel");
    barrelMesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrel));
    barrelMesh->LocalTransform.SetTranslate(
        APoint(0.0f, 2.512749f, -2.999999f));
    barrelParent->AttachChild(barrelMesh);
}
//----------------------------------------------------------------------------
APoint Castle::msBarrelTrn[MAX_BARRELS] =
{
    APoint::ORIGIN,
    APoint::ORIGIN,

    // Two-row rack of barrels, five per row.
    APoint(1894.745972f, -18.803799f, 37.956505f),  // 2
    APoint(1894.745972f, -22.794411f, 37.956505f),  // 3
    APoint(1894.745972f, -26.816143f, 37.956505f),  // 4
    APoint(1894.745972f, -30.652424f, 37.956505f),  // 5
    APoint(1894.745972f, -34.528419f, 37.956505f),  // 6
    APoint(1894.745972f, -18.803799f, 41.727104f),  // 7
    APoint(1894.745972f, -22.794411f, 41.727104f),  // 8
    APoint(1894.745972f, -26.816143f, 41.727104f),  // 9
    APoint(1894.745972f, -30.652424f, 41.727104f),  // 10
    APoint(1894.745972f, -34.528419f, 41.727104f),  // 11

    // Two-row rack of barrels, five per row.
    APoint(1870.532104f, -18.725361f, 41.727104f),  // 12
    APoint(1870.532104f, -22.794411f, 41.727104f),  // 13
    APoint(1870.532104f, -26.816143f, 41.727104f),  // 14
    APoint(1870.532104f, -30.652424f, 41.727104f),  // 15
    APoint(1870.532104f, -34.528419f, 41.727104f),  // 16
    APoint(1870.532104f, -18.725361f, 37.956505f),  // 17
    APoint(1870.532104f, -22.794411f, 37.956505f),  // 18
    APoint(1870.532104f, -26.816143f, 37.956505f),  // 19
    APoint(1870.532104f, -30.652424f, 37.956505f),  // 20
    APoint(1870.532104f, -34.528419f, 37.956505f),  // 21

    // Two-row rack of barrels, four per row.
    APoint(1880.682861f, -34.528419f, 41.727104f),  // 22
    APoint(1880.682861f, -22.794411f, 41.727104f),  // 23
    APoint(1880.682861f, -26.816143f, 41.727104f),  // 24
    APoint(1880.682861f, -30.652424f, 41.727104f),  // 25
    APoint(1880.682861f, -22.794411f, 37.956505f),  // 26
    APoint(1880.682861f, -26.816143f, 37.956505f),  // 27
    APoint(1880.682861f, -30.652424f, 37.956505f),  // 28
    APoint(1880.682861f, -34.528419f, 37.956505f),  // 29

    // Two-row rack of barrels, four per row.
    APoint(1885.211670f, -34.528419f, 41.727104f),  // 30
    APoint(1885.211670f, -22.794411f, 41.727104f),  // 31
    APoint(1885.211670f, -26.816143f, 41.727104f),  // 32
    APoint(1885.211670f, -30.652424f, 41.727104f),  // 33
    APoint(1885.211670f, -22.794411f, 37.956505f),  // 34
    APoint(1885.211670f, -26.816143f, 37.956505f),  // 35
    APoint(1885.211670f, -30.652424f, 37.956505f),  // 36
    APoint(1885.211670f, -34.528419f, 37.956505f)   // 37
};
//----------------------------------------------------------------------------
float Castle::msBarrelZAngle[MAX_BARRELS] =
{
    0.0f,
    0.0f,
    -90.000000f,  // 2
    -89.999991f,  // 3
    -90.000783f,  // 4
    -90.000000f,  // 5
    -90.000000f,  // 6
     90.000017f,  // 7
     89.999961f,  // 8
    -89.999995f,  // 9
     90.000000f,  // 10
    -89.999951f,  // 11
    -89.999994f,  // 12
    -90.000046f,  // 13
    -90.000000f,  // 14
     89.999951f,  // 15
    -90.000000f,  // 16
    -90.000000f,  // 17
    -90.000012f,  // 18
    -90.000000f,  // 19
    -90.000000f,  // 20
    -90.000000f,  // 21
    -89.999985f,  // 22
    -90.000000f,  // 23
    -90.000000f,  // 24
    -90.000000f,  // 25
    -90.000000f,  // 26
    -89.999980f,  // 27
    -89.999988f,  // 28
     89.999991f,  // 29
    -90.000000f,  // 30
     90.000000f,  // 31
    -90.000000f,  // 32
     89.999993f,  // 33
    -90.000009f,  // 34
    -89.999995f,  // 35
     90.000000f,  // 36
    -90.000000f   // 37
};
//----------------------------------------------------------------------------
float Castle::msBarrelYAngle[MAX_BARRELS] =
{
    0.0f,
    0.0f,
     22.499998f,  // 2
    -67.500002f,  // 3
     89.499997f,  // 4
     23.500002f,  // 5
    -30.500006f,  // 6
     65.999993f,  // 7
    -80.000003f,  // 8
     50.000002f,  // 9
     56.500002f,  // 10
    -86.000001f,  // 11
     53.500000f,  // 12
     81.500004f,  // 13
    -35.500005f,  // 14
     82.000003f,  // 15
    -68.500000f,  // 16
    -53.500000f,  // 17
    -54.000003f,  // 18
     79.000002f,  // 19
    -11.999999f,  // 20
     42.000004f,  // 21
     76.499999f,  // 22
     44.499999f,  // 23
     11.499998f,  // 24
    -33.000000f,  // 25
    -17.500002f,  // 26
    -79.999997f,  // 27
     54.999998f,  // 28
     67.500002f,  // 29
    -52.500002f,  // 30
     79.500006f,  // 31
    -29.000001f,  // 32
    -62.000000f,  // 33
    -43.499995f,  // 34
     43.999996f,  // 35
     89.500002f,  // 36
     32.000001f   // 37
};
//----------------------------------------------------------------------------
float Castle::msBarrelXAngle[MAX_BARRELS] =
{
    0.0f,
    0.0f,
     89.999985f,  // 2
     89.999982f,  // 3
     89.999609f,  // 4
     89.999985f,  // 5
     89.999988f,  // 6
    -90.000017f,  // 7
    -89.999980f,  // 8
     89.999984f,  // 9
    -90.000012f,  // 10
     89.999951f,  // 11
     89.999994f,  // 12
     89.999954f,  // 13
     89.999992f,  // 14
    -90.000000f,  // 15
     89.999981f,  // 16
     89.999994f,  // 17
     89.999994f,  // 18
     90.000000f,  // 19
     89.999986f,  // 20
     89.999986f,  // 21
     90.000029f,  // 22
     89.999990f,  // 23
     89.999986f,  // 24
     89.999992f,  // 25
     89.999986f,  // 26
     89.999980f,  // 27
     89.999988f,  // 28
    -90.000018f,  // 29
     89.999989f,  // 30
    -90.000000f,  // 31
     89.999992f,  // 32
    -90.000000f,  // 33
     89.999991f,  // 34
     89.999986f,  // 35
    -90.000000f,  // 36
     89.999988f   // 37
};
//----------------------------------------------------------------------------
void Castle::CreateBarrel (int i)
{
    // Node<barrelNode>
    //     TriMesh<barrel>

    Node* barrelNode = new0 Node();
    barrelNode->LocalTransform.SetTranslate(msBarrelTrn[i]);
    barrelNode->LocalTransform.SetScale(
        APoint(0.677369f, 0.677369f, 0.637836f));
    barrelNode->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msBarrelZAngle[i]*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_Y, msBarrelYAngle[i]*Mathf::DEG_TO_RAD)*
        HMatrix(AVector::UNIT_X, msBarrelZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(barrelNode);

    TriMesh* barMesh = new0 TriMesh(
        mBarrelMesh->GetVertexFormat(),
        mBarrelMesh->GetVertexBuffer(),
        mBarrelMesh->GetIndexBuffer());
    char number[3];
    sprintf(number, "%d", i);
    std::string name = "Barrel[" + std::string(number) + "]";
    barMesh->SetName(name);
    barMesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrel));
    barMesh->LocalTransform.SetTranslate(
        APoint(0.0f, -0.000016f, -3.0f));
    barrelNode->AttachChild(barMesh);
}
//----------------------------------------------------------------------------
APoint Castle::msDoorFrameTrn[MAX_DOORFRAMES] =
{
    APoint::ORIGIN,
    APoint(1875.994629f,  -15.620457f, 43.833332f),  // 1
    APoint(1860.006470f,  -15.620457f, 43.833332f),  // 2
    APoint(1843.993286f,  -15.620457f, 43.833332f),  // 3
    APoint(1843.998291f,    2.367251f, 43.833332f),  // 4
    APoint(1859.997925f,    2.366236f, 43.833332f),  // 5
    APoint(1834.919922f,   -6.650425f, 43.833332f),  // 6
    APoint(1891.994751f,    2.366236f, 43.833332f),  // 7
    APoint(1885.067139f,   26.251705f, 43.833332f),  // 8
    APoint(1866.962280f,   25.348019f, 43.833332f),  // 9
    APoint(1866.956909f,   41.351143f, 43.833332f),  // 10
    APoint(1885.052734f,   41.362923f, 43.833332f),  // 11
    APoint(1857.802368f,   65.249870f, 43.833332f),  // 12
    APoint(1857.851440f,   83.318832f, 43.833332f),  // 13
    APoint(1837.520264f,   83.322334f, 43.833332f),  // 14
    APoint(1837.499146f,   65.213364f, 43.833332f),  // 15
    APoint(1817.171021f,   65.260635f, 43.833332f),  // 16
    APoint(1817.194946f,   83.264915f, 43.833332f),  // 17
    APoint(1796.858276f,   83.356354f, 43.833332f),  // 18
    APoint(1796.869995f,   65.285172f, 43.833332f),  // 19
    APoint(1785.659058f,   74.296715f, 43.833332f),  // 20
    APoint(1807.036255f,   42.269661f, 43.833332f),  // 21
    APoint(1803.873291f,   20.145685f, 43.833332f),  // 22
    APoint(1794.720459f,   33.275734f, 43.833332f),  // 23
    APoint(1803.873291f,   -4.244927f, 43.833332f),  // 24
    APoint(1803.872803f,  -28.564281f, 43.833332f),  // 25
    APoint(1803.836670f,  -52.930870f, 43.833332f),  // 26
    APoint(1794.682007f,  -66.144150f, 43.833332f),  // 27
    APoint(1794.686646f,  -98.096169f, 43.833332f),  // 28
    APoint(1810.582031f, -116.162849f, 43.833332f),  // 29
    APoint(1836.642822f, -116.110542f, 43.833332f),  // 30
    APoint(1862.639771f, -116.178024f, 43.833332f),  // 31
    APoint(1896.613403f, -116.158752f, 43.833332f),  // 32
    APoint(1887.609253f, -107.134781f, 43.833332f),  // 33
    APoint(1912.624878f, -116.148674f, 43.833332f),  // 34
    APoint(1912.622925f,  -98.068062f, 43.833332f),  // 35
    APoint(1928.599731f, -116.068962f, 43.833332f),  // 36
    APoint(1937.614136f, -107.134499f, 43.833332f),  // 37
    APoint(1937.646729f,  -91.116020f, 43.833332f),  // 38
    APoint(1937.602539f,  -75.138031f, 43.833332f),  // 39
    APoint(1937.632935f,  -59.119759f, 43.833332f),  // 40
    APoint(1773.638062f,   82.818306f, 43.833332f),  // 41
    APoint(1753.695557f,   82.851143f, 43.833332f),  // 42
    APoint(1733.712280f,   82.850105f, 43.833332f),  // 43
    APoint(1698.071777f,   69.665985f, 43.833332f),  // 44
    APoint(1678.246094f,   49.864525f, 43.833332f),  // 45
    APoint(1665.976196f,   62.140488f, 43.833332f),  // 46
    APoint(1649.730835f,   45.891235f, 43.833332f),  // 47
    APoint(1654.211548f,   10.310408f, 43.833332f),  // 48
    APoint(1648.164795f,   16.323067f, 17.666666f),  // 49
    APoint(1669.797607f,   25.824842f, 17.666666f),  // 50
    APoint(1649.764038f,   45.933460f, 17.666666f),  // 51
    APoint(1678.656372f,   65.007118f, 17.666666f),  // 52
    APoint(1699.207520f,   79.152817f, 43.833332f),  // 53
    APoint(1693.644653f,   84.713318f, 37.433445f),  // 54
    APoint(1688.521484f,   84.718063f, 34.976566f),  // 55
    APoint(1682.986206f,   79.180832f, 28.130363f),  // 56
    APoint(1640.421143f,   -4.980236f, 25.544624f),  // 57
    APoint(1645.975220f,  -10.534098f, 18.986813f),  // 58
    APoint(1645.979858f,  -15.655856f, 17.147026f),  // 59
    APoint(1640.430908f,  -21.155571f, 10.646077f),  // 60
    APoint(1630.016235f,  -39.722786f,  7.833416f),  // 61
    APoint(1626.005127f,    0.981415f, 21.000000f),  // 62
    APoint(1626.152710f,    0.745347f, 45.500084f),  // 63
    APoint(1698.071777f, -187.846176f, 43.833332f),  // 64 // neg scale
    APoint(1678.246094f, -168.044708f, 43.833332f),  // 65 // neg scale
    APoint(1665.976196f, -180.320679f, 43.833332f),  // 66 // neg scale
    APoint(1649.730835f, -164.071426f, 43.833332f),  // 67 // neg scale
    APoint(1654.211548f, -128.490601f, 43.833332f),  // 68 // neg scale
    APoint(1648.164795f, -134.503265f, 17.666666f),  // 69 // neg scale
    APoint(1669.797607f, -144.005035f, 17.666666f),  // 70 // neg scale
    APoint(1649.764038f, -164.113647f, 17.666666f),  // 71 // neg scale
    APoint(1678.656372f, -183.187317f, 17.666666f),  // 72 // neg scale
    APoint(1699.207520f, -197.333008f, 43.833332f),  // 73 // neg scale
    APoint(1682.986206f, -197.361023f, 28.130363f),  // 74 // neg scale
    APoint(1640.421143f, -113.199959f, 25.544624f),  // 75 // neg scale
    APoint(1645.975220f, -107.646111f, 18.986813f),  // 76 // neg scale
    APoint(1645.979858f, -102.524345f, 17.147026f),  // 77 // neg scale
    APoint(1640.430908f,  -97.024635f, 10.646077f),  // 78 // neg scale
    APoint(1630.016235f,  -78.457405f,  7.833416f),  // 79 // neg scale
    APoint(1626.005127f, -119.161613f, 21.000000f),  // 80 // neg scale
    APoint(1626.152710f, -118.925545f, 45.500084f),  // 81 // neg scale
    APoint(1688.521484f, -202.898254f, 34.976566f),  // 82 // neg scale
    APoint(1693.644653f, -202.893509f, 37.433445f),  // 83 // neg scale
};
//----------------------------------------------------------------------------
float Castle::msDoorFrameZAngle[MAX_DOORFRAMES] =
{
    0.0f,
      0.0f,  // 1
      0.0f,  // 2
      0.0f,  // 3
      0.0f,  // 4
      0.0f,  // 5
     90.0f,  // 6
      0.0f,  // 7
    -90.0f,  // 8
    -90.0f,  // 9
    -90.0f,  // 10
    -90.0f,  // 11
    180.0f,  // 12
    180.0f,  // 13
    180.0f,  // 14
    180.0f,  // 15
    180.0f,  // 16
    180.0f,  // 17
    180.0f,  // 18
    180.0f,  // 19
    -90.0f,  // 20
    -90.0f,  // 21
    -90.0f,  // 22
    180.0f,  // 23
    -90.0f,  // 24
    -90.0f,  // 25
    -90.0f,  // 26
      0.0f,  // 27
      0.0f,  // 28
      0.0f,  // 29
      0.0f,  // 30
      0.0f,  // 31
      0.0f,  // 32
    -90.0f,  // 33
      0.0f,  // 34
      0.0f,  // 35
      0.0f,  // 36
    -90.0f,  // 37
    -90.0f,  // 38
    -90.0f,  // 39
    -90.0f,  // 40
    180.0f,  // 41
    180.0f,  // 42
    180.0f,  // 43
    135.0f,  // 44
    135.0f,  // 45
    135.0f,  // 46
    135.0f,  // 47
    135.0f,  // 48
    135.0f,  // 49
    135.0f,  // 50
    135.0f,  // 51
     45.0f,  // 52
     45.0f,  // 53
     45.0f,  // 54
    135.0f,  // 55
    135.0f,  // 56
   -135.0f,  // 57
   -135.0f,  // 58
    135.0f,  // 59
    135.0f,  // 60
   -180.0f,  // 61
     98.0f,  // 62
     92.0f,  // 63
     45.0f,  // 64 // neg scale
     45.0f,  // 65 // neg scale
     45.0f,  // 66 // neg scale
     45.0f,  // 67 // neg scale
     45.0f,  // 68 // neg scale
     45.0f,  // 69 // neg scale
     45.0f,  // 70 // neg scale
     45.0f,  // 71 // neg scale
    135.0f,  // 72 // neg scale
    135.0f,  // 73 // neg scale
     45.0f,  // 74 // neg scale
    -45.0f,  // 75 // neg scale
    -45.0f,  // 76 // neg scale
     45.0f,  // 77 // neg scale
     45.0f,  // 78 // neg scale
     0.0f,   // 79 // neg scale
     82.0f,  // 80 // neg scale
     88.0f,  // 81 // neg scale
     45.0f,  // 82 // neg scale
    135.0f,  // 83 // neg scale
};
//----------------------------------------------------------------------------
APoint Castle::msDoorFramePivotTrn[35] =
{
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 49
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 50
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 51
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 52
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 53
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 54
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 55
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 56
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 57
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 58
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 59
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 60
    APoint::ORIGIN,  // 61 (unused)
    APoint::ORIGIN,  // 62 (unused)
    APoint::ORIGIN,  // 63 (unused)
    APoint::ORIGIN,  // 64 (unused)
    APoint::ORIGIN,  // 65 (unused)
    APoint::ORIGIN,  // 66 (unused)
    APoint::ORIGIN,  // 67 (unused)
    APoint::ORIGIN,  // 68 (unused)
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 69 // neg scale
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 70 // neg scale
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 71 // neg scale
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 72 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 73 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 74 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 75 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 76 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 77 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 78 // neg scale
    APoint::ORIGIN,  // 79 // neg scale (unused)
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 80 // neg scale
    APoint( 0.000000f, 0.000000f, 7.833418f),  // 81 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 82 // neg scale
    APoint(-0.504759f, 0.000022f, 0.000000f),  // 83 // neg scale
};
//----------------------------------------------------------------------------
std::string Castle::GetDoorFrameFilename (int i)
{
    if ((1 <= i && i <= 52) || (64 <= i && i <= 72))
    {
        return "DoorFrame01.txt";
    }

    if ((53 <= i && i <= 60) || (73 <= i && i <= 78) || i == 82 || i == 83)
    {
        return "DoorFrame53.txt";
    }

    if (i == 61 || i == 79)
    {
        return "DoorFrame61.txt";
    }

    // i == 62, 63, 80, 81
    return "DoorFrame62.txt";
}
//----------------------------------------------------------------------------
void Castle::CreateDoorFrame (int i)
{
    TriMesh* mesh = LoadMeshPNT1(GetDoorFrameFilename(i));
    char number[3];
    sprintf(number, "%d", i);
    std::string name = "DoorFrame[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mDoorFrame));
    mesh->LocalTransform.SetTranslate(msDoorFrameTrn[i]);
    mesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msDoorFrameZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateDoorFramePivotTrn (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msDoorFrameTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msDoorFrameZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1(GetDoorFrameFilename(i));
    char number[3];
    sprintf(number, "%d", i);
    std::string name = "DoorFrame[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mDoorFrame));
    mesh->LocalTransform.SetTranslate(msDoorFramePivotTrn[i-49]);
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Castle::CreateDoorFrameScalePivotTrn (int i)
{
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "DoorFrame[" + std::string(number) + "]";

    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msDoorFrameTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msDoorFrameZAngle[i]*Mathf::DEG_TO_RAD));
    node->LocalTransform.SetScale(APoint(1.985981f, 0.838555f, 1.091798f));
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1(GetDoorFrameFilename(i));
    mesh->SetName(name);
    mesh->SetEffectInstance(mDLitMatTexEffect->CreateInstance(mDLight,
        mDrawBridgeMaterial, mTilePlanks));
    mesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 7.833418f));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msBunkTrn[MAX_BUNKS] =
{
    APoint::ORIGIN,  // 0
    APoint(1814.838745f, 100.605499f, 36.000000f), // 1
    APoint::ORIGIN,  // 2
    APoint::ORIGIN,  // 3
    APoint(1824.212158f,  100.573982f, 36.000000f), // 4
    APoint(1685.923340f,   66.158577f, 17.666666f), // 5
    APoint(1692.451660f,   60.012146f, 17.666666f), // 6
    APoint(1698.752808f,   53.888996f, 17.666666f), // 7
    APoint(1705.666016f,   46.950336f, 17.666666f), // 8
    APoint(1689.008179f,   30.694120f, 17.666666f), // 9
    APoint(1682.251099f,   37.463722f, 17.666666f), // 10
    APoint(1675.794434f,   44.081757f, 17.666666f), // 11
    APoint(1669.482666f,   50.373619f, 17.666666f), // 12
    APoint(1698.752808f, -172.069183f, 17.666666f), // 13
    APoint(1689.008179f, -148.874313f, 17.666666f), // 14
    APoint(1685.923340f, -184.338760f, 17.666666f), // 15
    APoint(1692.451660f, -178.192337f, 17.666666f), // 16
    APoint(1682.251099f, -155.643921f, 17.666666f), // 17
    APoint(1675.794434f, -162.261948f, 17.666666f), // 18
    APoint(1669.482666f, -168.553802f, 17.666666f), // 19
    APoint(1705.666016f, -165.130524f, 17.666666f)  // 20
};
//----------------------------------------------------------------------------
float Castle::msBunkZAngle[MAX_BUNKS] =
{
    0.0f,     // 0
    180.0f,   // 1
    0.0f,     // 2
    0.0f,     // 3
    0.0f,     // 4
    -45.0f,   // 5
    -45.0f,   // 6
    -45.0f,   // 7
    -45.0f,   // 8
    -45.0f,   // 9
    -45.0f,   // 10
    -45.0f,   // 11
    -45.0f,   // 12
    -135.0f,  // 13
    -135.0f,  // 14
    -135.0f,  // 15
    -135.0f,  // 16
    -135.0f,  // 17
    -135.0f,  // 18
    -135.0f,  // 19
    -135.0f   // 20
};
//----------------------------------------------------------------------------
void Castle::CreateBunk (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msBunkTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msBunkZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    Transform local;
    local.SetTranslate(APoint(0.0f, 0.0f, 1.0f));
    Texture2D* textures[2] = { mBunkwood, mBlanket };

    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("Bunk01.txt");

    for (int j = 0; j < 2; ++j)
    {
        meshes[j]->SetEffectInstance(mTexEffect->CreateInstance(textures[j]));
        meshes[j]->LocalTransform = local;
        node->AttachChild(meshes[j]);
    }

    char number[4];
    sprintf(number, "%d", i);
    std::string prefix = "Bunk[" + std::string(number) + "].";
    meshes[0]->SetName(prefix + "Bunkwood");
    meshes[1]->SetName(prefix + "Blanket");
}
//----------------------------------------------------------------------------
APoint Castle::msBenchTrn[MAX_BENCHES] =
{
    APoint::ORIGIN,  // 0
    APoint(1681.148315f,   79.009117f, 36.000000f),  // 1
    APoint(1674.953857f,   72.955605f, 36.000000f),  // 2
    APoint(1681.201050f,   66.904503f, 36.000000f),  // 3
    APoint(1687.431396f,   72.512283f, 36.000000f),  // 4
    APoint(1712.913086f,   49.999722f, 36.000000f),  // 5
    APoint(1706.630005f,   56.496555f, 36.000000f),  // 6
    APoint(1707.230347f,   43.421413f, 36.000000f),  // 7
    APoint(1700.947266f,   49.918247f, 36.000000f),  // 8
    APoint(1704.216431f,   41.177849f, 36.000000f),  // 9
    APoint(1697.933350f,   47.674683f, 36.000000f),  // 10
    APoint(1698.358276f,   35.063877f, 36.000000f),  // 11
    APoint(1692.075195f,   41.560711f, 36.000000f),  // 12
    APoint(1904.796875f,  -15.621780f, 36.000000f),  // 13
    APoint(1913.833740f,  -15.772926f, 36.000000f),  // 14
    APoint(1904.977783f,  -24.087351f, 36.000000f),  // 15
    APoint(1914.014526f,  -24.238497f, 36.000000f),  // 16
    APoint(1914.039063f,  -36.060966f, 36.000000f),  // 17
    APoint(1905.002197f,  -35.909821f, 36.000000f),  // 18
    APoint(1913.858154f,  -27.595396f, 36.000000f),  // 19
    APoint(1904.821411f,  -27.444250f, 36.000000f),  // 20
    APoint(1905.238281f,  -40.409866f, 36.000000f),  // 21
    APoint(1914.271240f,  -40.718704f, 36.000000f),  // 22
    APoint(1905.271484f,  -48.877304f, 36.000000f),  // 23
    APoint(1914.304199f,  -49.186138f, 36.000000f),  // 24
    APoint(1681.148315f, -197.189316f, 36.000000f),  // 25  // neg scale
    APoint(1674.953857f, -191.135803f, 36.000000f),  // 26  // neg scale
    APoint(1681.201050f, -185.084686f, 36.000000f),  // 27  // neg scale
    APoint(1687.431396f, -190.692474f, 36.000000f),  // 28  // neg scale
    APoint(1700.947266f, -168.098434f, 36.000000f),  // 29  // neg scale
    APoint(1704.216431f, -159.358032f, 36.000000f),  // 30  // neg scale
    APoint(1697.933350f, -165.854874f, 36.000000f),  // 31  // neg scale
    APoint(1698.358276f, -153.244064f, 36.000000f),  // 32  // neg scale
    APoint(1692.075195f, -159.740906f, 36.000000f),  // 33  // neg scale
    APoint(1707.230347f, -161.601608f, 36.000000f),  // 34  // neg scale
    APoint(1706.630005f, -174.676743f, 36.000000f),  // 35  // neg scale
    APoint(1712.913086f, -168.179916f, 36.000000f)   // 36  // neg scale
};
//----------------------------------------------------------------------------
float Castle::msBenchZAngle[MAX_BENCHES] =
{
       0.0f,  // 0
     -45.5f,  // 1
     -43.5f,  // 2
     -45.5f,  // 3
     -44.0f,  // 4
     -44.0f,  // 5
     -45.5f,  // 6
     -44.0f,  // 7
     -45.5f,  // 8
     -44.0f,  // 9
     -45.5f,  // 10
     -44.0f,  // 11
     -45.5f,  // 12
      -0.5f,  // 13
       1.0f,  // 14
      -0.5f,  // 15
       1.0f,  // 16
     179.5f,  // 17
    -179.0f,  // 18
     179.5f,  // 19
    -179.0f,  // 20
      -1.5f,  // 21
       0.0f,  // 22
      -1.5f,  // 23
       0.0f,  // 24
    -134.5f,  // 25
    -136.5f,  // 26
    -134.5f,  // 27
    -136.0f,  // 28
    -134.5f,  // 29
    -136.0f,  // 30
    -134.5f,  // 31
    -136.0f,  // 32
    -134.5f,  // 33
    -136.0f,  // 34
    -134.5f,  // 35
    -136.0f   // 36
};
//----------------------------------------------------------------------------
void Castle::CreateBench (int i)
{
    TriMesh* mesh = LoadMeshPNT1("Bench01.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "Bench[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mBench));
    mesh->LocalTransform.SetTranslate(msBenchTrn[i]);
    mesh->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z,
        msBenchZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msTableTrn[MAX_TABLES] =
{
    APoint::ORIGIN,  // 0
    APoint(1680.973999f,   72.984604f, 36.000000f),  // 1
    APoint(1706.665161f,   49.880695f, 36.000000f),  // 2
    APoint(1698.174683f,   41.944313f, 36.000000f),  // 3
    APoint(1909.019531f,  -19.503115f, 36.000000f),  // 4
    APoint(1909.816406f,  -32.179634f, 36.000000f),  // 5
    APoint(1909.392578f,  -44.364304f, 36.000000f),  // 6
    APoint(1680.973999f, -191.164795f, 36.000000f),  // 7 // neg scale
    APoint(1698.174683f, -160.124512f, 36.000000f),  // 8 // neg scale
    APoint(1706.665161f, -168.060883f, 36.000000f)   // 9 // neg scale
};
//----------------------------------------------------------------------------
float Castle::msTableZAngle[MAX_TABLES] =
{
    0.0f,    // 0
    45.0f,   // 1
    45.0f,   // 2
    45.0f,   // 3
    90.0f,   // 4
    -90.0f,  // 5
    90.0f,   // 6
    135.0f,  // 7
    135.0f,  // 8
    135.0f   // 9
};
//----------------------------------------------------------------------------
void Castle::CreateTable (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msTableTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msTableZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("Table01.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "Table[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 2.5f));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mBunkwood));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msBarrelRackTrn[MAX_BARREL_RACKS] =
{
    APoint::ORIGIN,  // 0
    APoint(1893.500000f, -26.615065f, 36.000000f),  // 1
    APoint(1869.371826f, -26.615065f, 36.000000f),  // 2
    APoint(1879.500732f, -26.615065f, 36.000000f),  // 3
    APoint(1884.024170f, -26.615065f, 36.000000f)   // 4
};
//----------------------------------------------------------------------------
void Castle::CreateBarrelRack (int i)
{
    TriMesh* mesh = 0;
    if (i == 1 || i == 2)
    {
        // 5 compartments
        mesh = LoadMeshPNT1("BarrelRack01.txt");
    }
    else if (i == 3 || i == 4)
    {
        // 4 compartments
        mesh = LoadMeshPNT1("BarrelRack03.txt");
    }
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "BarrelRack[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(msBarrelRackTrn[i]);
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mBarrelRack));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msChestTrn[MAX_CHESTS] =
{
    APoint::ORIGIN,  // 0
    APoint(1824.815430f,   90.043228f, 36.995735f), // 1
    APoint(1819.420288f,  102.660530f, 36.995735f), // 2
    APoint(1810.163208f,  102.764008f, 36.995735f), // 3
    APoint(1824.859863f,   86.495621f, 36.995735f), // 4
    APoint(1675.362671f,   56.633419f, 18.663862f), // 5
    APoint(1677.929565f,   54.083878f, 18.663862f), // 6
    APoint(1680.731201f,   51.380299f, 18.663862f), // 7
    APoint(1683.290894f,   48.761147f, 18.663862f), // 8
    APoint(1690.690308f,   64.725014f, 18.663862f), // 9
    APoint(1697.206543f,   58.508293f, 18.663862f), // 10
    APoint(1703.475952f,   51.985577f, 18.663862f), // 11
    APoint(1710.292847f,   45.316219f, 18.663862f), // 12
    APoint(1687.231689f,   35.807011f, 18.663862f), // 13
    APoint(1680.562134f,   42.305454f, 18.663862f), // 14
    APoint(1673.981812f,   48.790470f, 18.663862f), // 15
    APoint(1694.127563f,   29.056454f, 18.663862f), // 16
    APoint(1659.228516f,   40.477985f, 18.663862f), // 17
    APoint(1661.794800f,   37.926598f, 18.663862f), // 18
    APoint(1664.437134f,   35.221172f, 18.663862f), // 19
    APoint(1666.996582f,   32.600182f, 18.663862f), // 20
    APoint(1677.929565f, -172.264069f, 18.663862f), // 21  // had -1 scale
    APoint(1690.690308f, -182.905212f, 18.663862f), // 22  // had -1 scale
    APoint(1703.475952f, -170.165771f, 18.663862f), // 23  // had -1 scale
    APoint(1687.231689f, -153.987198f, 18.663862f), // 24  // had -1 scale
    APoint(1675.362671f, -174.813614f, 18.663862f), // 25  // had -1 scale
    APoint(1680.731201f, -169.560486f, 18.663862f), // 26  // had -1 scale
    APoint(1683.290894f, -166.941345f, 18.663862f), // 27  // had -1 scale
    APoint(1697.206543f, -176.688477f, 18.663862f), // 28  // had -1 scale
    APoint(1659.228516f, -158.658173f, 18.663862f), // 29  // had -1 scale
    APoint(1680.562134f, -160.485641f, 18.663862f), // 30  // had -1 scale
    APoint(1673.981812f, -166.970657f, 18.663862f), // 31  // had -1 scale
    APoint(1661.794800f, -156.106781f, 18.663862f), // 32  // had -1 scale
    APoint(1694.127563f, -147.236649f, 18.663862f), // 33  // had -1 scale
    APoint(1664.437134f, -153.401367f, 18.663862f), // 34  // had -1 scale
    APoint(1666.996582f, -150.780365f, 18.663862f), // 35  // had -1 scale
    APoint(1710.292847f, -163.496414f, 18.663862f)  // 36  // had -1 scale
};
//----------------------------------------------------------------------------
float Castle::msChestZAngle[MAX_CHESTS] =
{
    0.0f,     // 0
    0.0f,     // 1
    90.0f,    // 2
    90.0f,    // 3
    0.0f,     // 4
    -135.0f,  // 5
    -135.0f,  // 6
    -135.0f,  // 7
    -135.0f,  // 8
    45.0f,    // 9
    45.0f,    // 10
    45.0f,    // 11
    45.0f,    // 12
    45.0f,    // 13
    45.0f,    // 14
    45.0f,    // 15
    45.0f,    // 16
    -135.0f,  // 17
    -135.0f,  // 18
    -135.0f,  // 19
    -135.0f,  // 20
    135.0f,   // 21
    -45.0f,   // 22
    -45.0f,   // 23
    -45.0f,   // 24
    135.0f,   // 25
    135.0f,   // 26
    135.0f,   // 27
    -455.0f,   // 28
    135.0f,   // 29
    -45.0f,   // 30
    -45.0f,   // 31
    135.0f,   // 32
    -45.0f,   // 33
    135.0f,   // 34
    135.0f,   // 35
    -45.0f    // 36
};
//----------------------------------------------------------------------------
void Castle::CreateChest (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msChestTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msChestZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    char number[4];
    sprintf(number, "%d", i);
    std::string prefix = "Chest[" + std::string(number) + "].";

    Node* topnode = new0 Node();
    topnode->LocalTransform.SetTranslate(APoint(1.0f, 0.0f, 0.583333f));
    topnode->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Y, -90.0f*Mathf::DEG_TO_RAD));
    node->AttachChild(topnode);

    TriMesh* topmesh = LoadMeshPNT1("ChestTop01.txt");
    topmesh->SetName(prefix + "top");
    topmesh->LocalTransform.SetTranslate(APoint(-1.583333f, 0.0f, 1.0f));
    topmesh->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Y, 90.0f*Mathf::DEG_TO_RAD));
    topmesh->SetEffectInstance(mTexEffect->CreateInstance(mChest));
    topnode->AttachChild(topmesh);

    TriMesh* bottommesh = LoadMeshPNT1("ChestBottom01.txt");
    bottommesh->SetName(prefix + "bottom");
    bottommesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, -1.0f));
    bottommesh->SetEffectInstance(mTexEffect->CreateInstance(mChest));
    node->AttachChild(bottommesh);

    std::vector<TriMesh*> meshes(2);
    meshes[0] = topmesh;
    meshes[1] = bottommesh;
}
//----------------------------------------------------------------------------
APoint Castle::msCeilingLightTrn[MAX_CEILING_LIGHTS] =
{
    APoint::ORIGIN,  // 0
    APoint(1916.086304f, -25.430758f, 50.007027f),  // 1
    APoint(1790.871094f,  48.637581f, 43.996727f),  // 2
    APoint(1794.712280f, -82.175278f, 50.007000f)   // 3
};
//----------------------------------------------------------------------------
void Castle::CreateCeilingLight (int i)
{
    std::vector<TriMesh*> meshes = LoadMeshPNT1Multi("CeilingLight01.txt");

    char number[4];
    sprintf(number, "%d", i);
    std::string prefix = "CeilingLight[" + std::string(number) + "].";

    meshes[0]->SetName(prefix + "lightwood");
    meshes[0]->SetEffectInstance(mTexEffect->CreateInstance(mLightwood));
    meshes[0]->LocalTransform.SetTranslate(msCeilingLightTrn[i]);
    mTrnNode->AttachChild(meshes[0]);

    meshes[1]->SetName(prefix + "material26");
    meshes[1]->SetEffectInstance(mMatEffect->CreateInstance(mMaterial26));
    meshes[1]->LocalTransform.SetTranslate(msCeilingLightTrn[i]);
    mTrnNode->AttachChild(meshes[1]);

    meshes[2]->SetName(prefix + "rope");
    meshes[2]->SetEffectInstance(mTexEffect->CreateInstance(mRope));
    meshes[2]->LocalTransform.SetTranslate(msCeilingLightTrn[i]);
    mTrnNode->AttachChild(meshes[2]);
}
//----------------------------------------------------------------------------
APoint Castle::msSquareTableTrn[MAX_SQUARE_TABLES] =
{
    APoint::ORIGIN,  // 0
    APoint(1700.408325f,   37.138123f, 17.666666f),  // 1
    APoint(1683.537598f,   21.349033f, 17.666666f),  // 2
    APoint(1810.589844f,   91.107056f, 36.000000f),  // 3
    APoint(1783.386108f,   56.324100f, 36.000000f),  // 4
    APoint(1783.374390f,   45.321632f, 36.000000f),  // 5
    APoint(1700.408325f, -155.318314f, 17.666666f),  // 6  // neg scale
    APoint(1683.537598f, -139.529221f, 17.666666f)   // 7  // neg scale
};
//----------------------------------------------------------------------------
float Castle::msSquareTableZAngle[MAX_SQUARE_TABLES] =
{
    0.0f,    // 0
    0.0f,    // 1
    45.0f,   // 2
    0.0f,    // 3
    15.0f,   // 4
    -10.0f,  // 5
    180.0f,  // 6
    135.0f   // 7
};
//----------------------------------------------------------------------------
void Castle::CreateSquareTable (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msSquareTableTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msSquareTableZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("SquareTable01.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "SquareTable[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 2.5f));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mSquareTable));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msSimpleChairTrn[MAX_SIMPLE_CHAIRS] =
{
    APoint::ORIGIN,  // 0
    APoint(1682.132446f,   22.915470f, 17.661121f),  // 1
    APoint(1681.833008f,   19.096756f, 17.661121f),  // 2
    APoint(1685.899902f,   18.896498f, 17.661121f),  // 3
    APoint(1684.926880f,   22.492020f, 17.661121f),  // 4
    APoint(1700.391846f,   35.084896f, 17.661121f),  // 5
    APoint(1697.788818f,   37.294594f, 17.661121f),  // 6
    APoint(1702.497437f,   37.179901f, 17.661121f),  // 7
    APoint(1700.334229f,   39.292641f, 17.661121f),  // 8
    APoint(1810.595825f,   92.995415f, 35.995808f),  // 9
    APoint(1810.515503f,   89.083763f, 35.995808f),  // 10
    APoint(1812.566650f,   91.234467f, 35.995808f),  // 11
    APoint(1783.974243f,   53.823063f, 35.995808f),  // 12
    APoint(1785.593994f,   57.149139f, 35.995808f),  // 13
    APoint(1782.712158f,   58.469181f, 35.995808f),  // 14
    APoint(1782.949951f,   43.302998f, 35.995808f),  // 15
    APoint(1785.642944f,   44.907169f, 35.995808f),  // 16
    APoint(1783.708252f,   47.141273f, 35.995808f),  // 17
    APoint(1780.964966f,   45.834797f, 35.995808f),  // 18
    APoint(1780.694214f,   55.530125f, 35.995808f),  // 19
    APoint(1700.334229f, -157.472839f, 17.661121f),  // 20 // neg scale
    APoint(1702.497437f, -155.360092f, 17.661121f),  // 21 // neg scale
    APoint(1700.391846f, -153.265091f, 17.661121f),  // 22 // neg scale
    APoint(1684.926880f, -140.672211f, 17.661121f),  // 23 // neg scale
    APoint(1697.788818f, -155.474792f, 17.661121f),  // 24 // neg scale
    APoint(1681.833008f, -137.276947f, 17.661121f),  // 25 // neg scale
    APoint(1682.132446f, -141.095673f, 17.661121f),  // 26 // neg scale
    APoint(1685.899902f, -137.076691f, 17.661121f)   // 27 // neg scale
};
//----------------------------------------------------------------------------
float Castle::msSimpleChairZAngle[MAX_SIMPLE_CHAIRS] =
{
    0.0f,     // 0
    -135.0f,  // 1
    -26.0f,   // 2
    53.0f,    // 3
    124.5f,   // 4
    -26.0f,   // 5
    -93.5f,   // 6
    70.0f,    // 7
    -179.0f,  // 8
    -179.0f,  // 9
    -10.5f,   // 10
    118.0f,   // 11
    8.5f,     // 12
    118.0f,   // 13
    -153.5f,  // 14
    -7.5f,    // 15
    83.5f,    // 16
    158.0f,   // 17
    -111.0f,  // 18
    -81.0f,   // 19
    -1.0f,    // 20
    110.0f,   // 21
    -154.0f,  // 22
    55.5f,    // 23
    -86.5f,   // 24
    -154.0f,  // 25
    -45.0f,   // 26
    127.0f    // 27
};
//----------------------------------------------------------------------------
void Castle::CreateSimpleChair (int i)
{
    Node* node = new0 Node();
    node->LocalTransform.SetTranslate(msSimpleChairTrn[i]);
    node->LocalTransform.SetRotate(
        HMatrix(AVector::UNIT_Z, msSimpleChairZAngle[i]*Mathf::DEG_TO_RAD));
    mTrnNode->AttachChild(node);

    TriMesh* mesh = LoadMeshPNT1("SimpleChair01.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "SimpleChair[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(APoint(0.000027f, 0.000027f, 1.5f));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mSimpleChair));
    node->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msMugTrn[MAX_MUGS] =
{
    APoint::ORIGIN,  // 0
    APoint(1684.091431f,   67.877785f, 38.835186f),  // 1
    APoint(1685.019653f,   70.792397f, 38.835186f),  // 2
    APoint(1680.548218f,   71.016060f, 38.835186f),  // 3
    APoint(1679.973999f,   76.336327f, 38.835186f),  // 4
    APoint(1676.413574f,   75.179497f, 38.835186f),  // 5
    APoint(1693.378784f,   44.750465f, 38.835186f),  // 6
    APoint(1696.719604f,   45.740982f, 38.835186f),  // 7
    APoint(1702.048340f,   40.404408f, 38.835186f),  // 8
    APoint(1709.212280f,   45.257370f, 38.835186f),  // 9
    APoint(1704.098633f,   50.036484f, 38.835186f),  // 10
    APoint(1702.049438f,   52.967052f, 38.835186f),  // 11
    APoint(1709.660400f,   49.131836f, 38.835186f),  // 12
    APoint(1705.214478f,   53.631329f, 38.835186f),  // 13
    APoint(1914.311646f,  -21.135786f, 38.835186f),  // 14
    APoint(1914.912598f,  -17.833952f, 38.835186f),  // 15
    APoint(1909.045532f,  -17.915998f, 38.835186f),  // 16
    APoint(1904.326782f,  -18.284742f, 38.835186f),  // 17
    APoint(1904.579102f,  -21.235966f, 38.835186f),  // 18
    APoint(1903.304810f,  -30.535830f, 38.835186f),  // 19
    APoint(1905.643677f,  -34.151360f, 38.835186f),  // 20
    APoint(1911.353882f,  -34.064449f, 38.835186f),  // 21
    APoint(1915.690674f,  -33.817017f, 38.835186f),  // 22
    APoint(1913.835327f,  -30.677719f, 38.835186f),  // 23
    APoint(1907.563599f,  -30.436558f, 38.835186f),  // 24
    APoint(1911.102417f,  -21.376659f, 38.835186f),  // 25
    APoint(1903.540161f,  -45.818390f, 38.835186f),  // 26
    APoint(1906.682129f,  -42.255157f, 38.835186f),  // 27
    APoint(1911.426758f,  -45.929325f, 38.835186f),  // 28
    APoint(1915.602783f,  -45.820602f, 38.835186f),  // 29
    APoint(1684.091431f, -186.057983f, 38.835186f),  // 30  // neg scale
    APoint(1685.019653f, -188.972595f, 38.835186f),  // 31  // neg scale
    APoint(1680.548218f, -189.196259f, 38.835186f),  // 32  // neg scale
    APoint(1679.973999f, -194.516525f, 38.835186f),  // 33  // neg scale
    APoint(1676.413574f, -193.359680f, 38.835186f),  // 34  // neg scale
    APoint(1693.378784f, -162.930664f, 38.835186f),  // 35  // neg scale
    APoint(1696.719604f, -163.921173f, 38.835186f),  // 36  // neg scale
    APoint(1702.048340f, -158.584595f, 38.835186f),  // 37  // neg scale
    APoint(1704.098633f, -168.216675f, 38.835186f),  // 38  // neg scale
    APoint(1702.049438f, -171.147247f, 38.835186f),  // 39  // neg scale
    APoint(1705.214478f, -171.811523f, 38.835186f),  // 40  // neg scale
    APoint(1709.212280f, -163.437561f, 38.835186f),  // 41  // neg scale
    APoint(1709.660400f, -167.312027f, 38.835186f)   // 42  // neg scale
};
//----------------------------------------------------------------------------
float Castle::msMugZAngle[MAX_MUGS] =
{
    0.0f,     // 0
    -75.5f,   // 1
    123.0f,   // 2
    76.0f,    // 3
    -46.5f,   // 4
    -179.5f,  // 5
    -75.5f,   // 6
    39.0f,    // 7
    -19.0f,   // 8
    19.0f,    // 9
    -102.5f,  // 10
    -147.5f,  // 11
    115.0f,   // 12
    8.0f,     // 13
    179.5f,   // 14
    46.5f,    // 15
    -58.0f,   // 16
    150.5f,   // 17
    -11.0f,   // 18
    179.5f,   // 19
    21.0f,    // 20
    -32.5f,   // 21
    -85.0f,   // 22
    20.5f,    // 23
    8.5f,     // 24
    8.5f,     // 25
    179.5f,   // 26
    -30.0f,   // 27
    -9.0f,    // 28
    -118.0f,  // 29
    -104.5f,  // 30  // neg scale
    57.0f,    // 31  // neg scale
    104.0f,   // 32  // neg scale
    -133.5f,  // 33  // neg scale
    -0.5f,    // 34  // neg scale
    -104.5f,  // 35  // neg scale
    141.0f,   // 36  // neg scale
    -161.0f,  // 37  // neg scale
    -77.5f,   // 38  // neg scale
    -32.5f,   // 39  // neg scale
    172.0f,   // 40  // neg scale
    161.0f,   // 41  // neg scale
    65.0f     // 42  // neg scale
};
//----------------------------------------------------------------------------
void Castle::CreateMug (int i)
{
    TriMesh* mesh = LoadMeshPNT1("Mug.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "Mug[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(msMugTrn[i]);
    mesh->LocalTransform.SetMatrix(
        HMatrix(AVector::UNIT_Z, msMugZAngle[i]*Mathf::DEG_TO_RAD));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mMug));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
APoint Castle::msDoorTrn[MAX_DOORS] =
{
    APoint::ORIGIN,  // 0
    APoint(1695.567627f,   69.417938f, 36.000000f),  // 1
    APoint(1675.808838f,   49.528076f, 36.000000f),  // 2
    APoint(1663.579590f,   61.938667f, 36.000000f),  // 3
    APoint(1647.188599f,   45.527767f, 36.000000f),  // 4
    APoint(1656.808838f,   10.662731f, 36.000000f),  // 5
    APoint(1630.031616f,  -78.957748f,  0.000000f),  // 6
    APoint(1628.378052f,  -41.600922f,  0.000000f),  // 7
    APoint(1793.068481f,  -64.016670f, 36.000000f),  // 8
    APoint(1793.044678f, -100.102715f, 36.000000f)   // 9
};
//----------------------------------------------------------------------------
float Castle::msDoorZAngle[MAX_DOORS] =
{
    0.0f,     // 0
    -135.0f,  // 1
    -135.0f,  // 2
    -135.0f,  // 3
    -135.0f,  // 4
    45.0f,    // 5
    180.0f,   // 6
    -90.0f,   // 7
    90.0f,    // 8
    -90.0f    // 9
};
//----------------------------------------------------------------------------
void Castle::CreateDoor (int i)
{
    TriMesh* mesh = LoadMeshPNT1("Door.txt");
    char number[4];
    sprintf(number, "%d", i);
    std::string name = "Door[" + std::string(number) + "]";
    mesh->SetName(name);
    mesh->LocalTransform.SetTranslate(msDoorTrn[i]);
    mesh->LocalTransform.SetMatrix(
        HMatrix(AVector::UNIT_Z, msDoorZAngle[i]*Mathf::DEG_TO_RAD));
    mesh->SetEffectInstance(mTexEffect->CreateInstance(mDoor));
    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
