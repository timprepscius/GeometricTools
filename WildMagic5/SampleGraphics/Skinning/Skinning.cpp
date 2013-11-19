// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Skinning.h"

WM5_WINDOW_APPLICATION(Skinning);

//----------------------------------------------------------------------------
Skinning::Skinning ()
    :
    WindowApplication3("SampleGraphics/Skinning", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool Skinning::OnInitialize ()
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

    InitializeCameraMotion(0.1f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void Skinning::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Skinning::OnIdle ()
{
    MeasureTime();

    UpdateConstants((float)GetTimeInSeconds());

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
void Skinning::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);

    // The skinned object is a cylinder.
    const int radialSamples = 10;
    const int axisSamples = 7;
    const float radius = 10.0f;
    const float height = 80.0f;
    const float invRS = 1.0f/(float)radialSamples;
    const float invASm1 = 1.0f/(float)(axisSamples - 1);
    const float halfHeight = 0.5f*height;
    const APoint center(0.0f, 0.0f, 100.0f);
    const AVector u(0.0f,0.0f,-1.0f);
    const AVector v(0.0f,1.0f,0.0f);
    const AVector axis(1.0f,0.0f,0.0f);

    // Generate geometry.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT4, 0);
    int vstride = vformat->GetStride();
    const int numVertices = axisSamples*(radialSamples + 1);
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    int r, a, aStart, i;
    float* sn = new1<float>(radialSamples + 1);
    float* cs = new1<float>(radialSamples + 1);
    for (r = 0; r < radialSamples; ++r)
    {
        float angle = Mathf::TWO_PI*invRS*r;
        cs[r] = Mathf::Cos(angle);
        sn[r] = Mathf::Sin(angle);
    }
    sn[radialSamples] = sn[0];
    cs[radialSamples] = cs[0];

    // Generate the cylinder itself.
    for (a = 0, i = 0; a < axisSamples; ++a, ++i)
    {
        float axisFraction = a*invASm1;  // in [0,1]
        float z = -halfHeight + height*axisFraction;

        // Compute center of slice.
        APoint sliceCenter = center + z*axis;

        // Compute slice vertices with duplication at end point.
        Float3 color(axisFraction, 1.0f - axisFraction, 0.3f);
        Float4 tcoord;
        int save = i;
        for (r = 0; r < radialSamples; ++r, ++i)
        {
            AVector normal = cs[r]*u + sn[r]*v;
            vba.Position<Float3>(i) = sliceCenter + radius*normal;
            vba.Color<Float3>(0,i) = color;
            vba.TCoord<Float4>(0, i) = ComputeWeights(a);
        }

        vba.Position<Float3>(i) = vba.Position<Float3>(save);
        vba.Color<Float3>(0, i) = color;
        vba.TCoord<Float4>(0, i) = ComputeWeights(a);
    }

    // Generate connectivity.
    int numTriangles = 2*(axisSamples - 1)*radialSamples;
    int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (a = 0, aStart = 0; a < axisSamples - 1; ++a)
    {
        int i0 = aStart;
        int i1 = i0 + 1;
        aStart += radialSamples + 1;
        int i2 = aStart;
        int i3 = i2 + 1;
        for (i = 0; i < radialSamples; ++i, indices += 6)
        {
            indices[0] = i0++;
            indices[1] = i1;
            indices[2] = i2;
            indices[3] = i1++;
            indices[4] = i3++;
            indices[5] = i2++;
        }
    }

    delete1(cs);
    delete1(sn);

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mTrnNode->AttachChild(mesh);

    std::string effectFile = Environment::GetPathR("Skinning.wmfx");
    SkinningEffect* effect = new0 SkinningEffect(effectFile);

    ShaderFloat* skinningMatrix[4] =
    {
        new0 ShaderFloat(4),
        new0 ShaderFloat(4),
        new0 ShaderFloat(4),
        new0 ShaderFloat(4)
    };

    for (i = 0; i < 4; ++i)
    {
        mSkinningMatrix[i] = skinningMatrix[i]->GetData();
    }

    mesh->SetEffectInstance(effect->CreateInstance(skinningMatrix));
}
//----------------------------------------------------------------------------
Float4 Skinning::ComputeWeights (int a)
{
    Float4 tcoord(0.0f, 0.0f, 0.0f, 0.0f);

    if (a == 0)
    {
        tcoord[0] = 1.0f;
        tcoord[1] = 0.0f;
        tcoord[2] = 0.0f;
        tcoord[3] = 0.0f;
    }
    else if (a == 1)
    {
        tcoord[0] = 0.5f;
        tcoord[1] = 0.5f;
        tcoord[2] = 0.0f;
        tcoord[3] = 0.0f;
    }
    else if (a == 2)
    {
        tcoord[0] = 0.0f;
        tcoord[1] = 1.0f;
        tcoord[2] = 0.0f;
        tcoord[3] = 0.0f;
    }
    else if (a == 3)
    {
        tcoord[0] = 0.0f;
        tcoord[1] = 0.5f;
        tcoord[2] = 0.5f;
        tcoord[3] = 0.0f;
    }
    else if (a == 4)
    {
        tcoord[0] = 0.0f;
        tcoord[1] = 0.0f;
        tcoord[2] = 1.0f;
        tcoord[3] = 0.0f;
    }
    else if (a == 5)
    {
        tcoord[0] = 0.0f;
        tcoord[1] = 0.0f;
        tcoord[2] = 0.5f;
        tcoord[3] = 0.5f;
    }
    else
    {
        tcoord[0] = 0.0f;
        tcoord[1] = 0.0f;
        tcoord[2] = 0.0f;
        tcoord[3] = 1.0f;
    }

    return tcoord;
}
//----------------------------------------------------------------------------
void Skinning::UpdateConstants (float time)
{
    // Create some arbitrary skinning transformations.
    const float factor = Mathf::PI/1.25f;
    int div = (int)(time/factor);

    // The angle in in [-factor/4, +factor/4].
    float maxAngle = Mathf::FAbs(time - (div + 0.5f)*factor) - 0.25f*factor;

    for (int i = 0; i < 4; ++i)
    {
        float angle;
        if ((int)(time/factor + 0.25f) % 2)
        {
            angle = Mathf::FAbs(i - 1.5f)*maxAngle;
        }
        else
        {
            angle = (i - 1.5f)*maxAngle;
        }

        float cs = Mathf::Cos(angle);
        float sn = Mathf::Sin(angle);
        float yTrn = 10.0f*Mathf::Sin(time + 0.5f*i);

        float* skinningMatrix = mSkinningMatrix[i];
        skinningMatrix[ 0] = cs;
        skinningMatrix[ 1] = -sn;
        skinningMatrix[ 2] = 0.0f;
        skinningMatrix[ 3] = 0.0f;
        skinningMatrix[ 4] = sn;
        skinningMatrix[ 5] = cs;
        skinningMatrix[ 6] = 0.0f;
        skinningMatrix[ 7] = yTrn;
        skinningMatrix[ 8] = 0.0f;
        skinningMatrix[ 9] = 0.0f;
        skinningMatrix[10] = 1.0f;
        skinningMatrix[11] = 0.0f;
        skinningMatrix[12] = 0.0f;
        skinningMatrix[13] = 0.0f;
        skinningMatrix[14] = 0.0f;
        skinningMatrix[15] = 1.0f;
    }
}
//----------------------------------------------------------------------------
