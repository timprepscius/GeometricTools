// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2013/07/14)

#include "MorphFaces.h"

WM5_WINDOW_APPLICATION(MorphFaces);

//----------------------------------------------------------------------------
MorphFaces::MorphFaces ()
    :
    WindowApplication3("SampleGraphics/MorphFaces", 0, 0, 512, 512,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    for (int i = 0; i < NUM_TARGETS; ++i)
    {
        mVertices[i] = 0;
        mWeightInterpolator[i] = 0;
    }
    mColorInterpolator = 0;
}
//----------------------------------------------------------------------------
bool MorphFaces::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mMorphResult->LocalTransform.SetTranslate(
        -mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        1.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    mScene->Update();

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    mAnimStartTime = (float)GetTimeInSeconds();
    return true;
}
//----------------------------------------------------------------------------
void MorphFaces::OnTerminate ()
{
    mScene = 0;
    mMorphResult = 0;
    mWireState = 0;
    mLight = 0;

    for (int i = 0; i < NUM_TARGETS; ++i)
    {
        delete1(mVertices[i]);
        delete0(mWeightInterpolator[i]);
    }
    delete0(mColorInterpolator);

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MorphFaces::OnIdle ()
{
    MeasureTime();

    if (MoveObject())
    {
        mScene->Update();
    }

    // The input time is relative to the starting time of the application.
    UpdateMorph((float)GetTimeInSeconds() - mAnimStartTime);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mVisibleSet);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MorphFaces::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    case '0':  // restart the animation
        mAnimStartTime = (float)GetTimeInSeconds();
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void MorphFaces::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateMorphResult();

    // Load the position/normal data for the morph targets.  Also, create the
    // weight interpolators.
    LoadTarget( 0, "M0Base");
    LoadTarget( 1, "M1Viseam01");
    LoadTarget( 2, "M2Viseam02");
    LoadTarget( 3, "M3Viseam03a");
    LoadTarget( 4, "M3Viseam03b");
    LoadTarget( 5, "M4TheNoLook");
    LoadTarget( 6, "M5Smile");
    LoadTarget( 7, "M10FullLeft");
    LoadTarget( 8, "M7FullRight");
    LoadTarget( 9, "M9HalfLeft");
    LoadTarget(10, "M8HalfRight");
    LoadTarget(11, "M6Anger");

    // The color interpolator is used to make the soldier's face red when
    // he is angry.
    std::string filename = Environment::GetPathR("LightColorSampler.txt");
    mColorInterpolator = new0 CubicInterpolator<3,float>(filename);

    // Initially populate the vertex buffer.
    UpdateMorph(0.0f);
}
//----------------------------------------------------------------------------
void MorphFaces::CreateMorphResult ()
{
    mMorphResult = new0 Node();
    mScene->AttachChild(mMorphResult);

    LightPntPerPixEffect* lmEffect = new0 LightPntPerPixEffect(); 

    mLight = new0 Light(Light::LT_POINT);
    mLight->Ambient = Float4(1.0f, 0.945098f, 0.792157f, 1.0f);
    mLight->Diffuse = Float4(1.0f, 0.945098f, 0.792157f, 1.0f);
    mLight->Specular = Float4(1.0f, 0.945098f, 0.792157f, 1.0f);
    mLight->Position = APoint(-1186.77f, -1843.32f, -50.7567f);
    mLight->Intensity = 0.5f;

    Material* materials[4] =
    {
        new0 Material(),
        0,
        new0 Material(),
        new0 Material()
    };
    materials[0]->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    materials[0]->Ambient = Float4(0.694118f, 0.607843f, 0.545098f, 1.0f);
    materials[0]->Diffuse = Float4(0.694118f, 0.607843f, 0.545098f, 1.0f);
    materials[0]->Specular = Float4(0.9f, 0.9f, 0.9f, 10.0f);

    materials[2]->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    materials[2]->Ambient = Float4(0.388235f, 0.282353f, 0.168627f, 1.0f);
    materials[2]->Diffuse = Float4(0.388235f, 0.282353f, 0.168627f, 1.0f);
    materials[2]->Specular = Float4(0.9f, 0.9f, 0.9f, 10.0f);

    materials[3]->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    materials[3]->Ambient = Float4(0.364706f, 0.0f, 0.0f, 1.0f);
    materials[3]->Diffuse = Float4(0.364706f, 0.0f, 0.0f, 1.0f);
    materials[3]->Specular = Float4(0.9f, 0.9f, 0.9f, 10.0f);

    Texture2DEffect* tEffect = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR,
        Shader::SC_REPEAT, Shader::SC_REPEAT);

    std::string textureName = Environment::GetPathR("Eye.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(textureName);
    texture->GenerateMipmaps();

    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 1,  // normals
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    std::string filename = Environment::GetPathR("SharedTexTri.txt");
    std::ifstream inFile(filename.c_str());
    inFile >> mNumVertices;
    VertexBuffer* vbuffer = new0 VertexBuffer(mNumVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    for (int i = 0; i < mNumVertices; ++i)
    {
        Float2 tcoord;
        inFile >> tcoord[0];
        inFile >> tcoord[1];
        vba.Position<Float3>(i) = Float3(0.0f, 0.0f, 0.0f);
        vba.TCoord<Float3>(1, i) = Float3(0.0f, 0.0f, 0.0f);
        vba.TCoord<Float2>(0, i) = tcoord;
    }
    for (int j = 0; j < 4; ++j)
    {
        int numSubTriangles;
        inFile >> numSubTriangles;
        int numSubIndices = 3*numSubTriangles;
        IndexBuffer* ibuffer = new0 IndexBuffer(numSubIndices, sizeof(int));
        int* subIndices = (int*)ibuffer->GetData();
        for (int i = 0; i < numSubIndices; ++i)
        {
            int index;
            inFile >> index;
            *subIndices++ = index;
        }

        TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
        mMorphResult->AttachChild(mesh);
        if (j != 1)
        {
            mesh->SetEffectInstance(lmEffect->CreateInstance(mLight,
                materials[j]));
        }
        else
        {
            mesh->SetEffectInstance(tEffect->CreateInstance(texture));
        }
        mVisibleSet.Insert(mesh);
    }
    inFile.close();
}
//----------------------------------------------------------------------------
void MorphFaces::LoadTarget (int i, const std::string& targetName)
{
    std::string filename = Environment::GetPathR(targetName + "PosNor.txt");
    std::ifstream inFile(filename.c_str());
    mVertices[i] = new1<InVertex>(mNumVertices);
    InVertex* vertex = mVertices[i];
    for (int j = 0; j < mNumVertices; ++j, ++vertex)
    {
        inFile >> vertex->Position[0];
        inFile >> vertex->Position[1];
        inFile >> vertex->Position[2];
        inFile >> vertex->Normal[0];
        inFile >> vertex->Normal[1];
        inFile >> vertex->Normal[2];
    }
    inFile.close();

    if (i > 0)
    {
        filename = Environment::GetPathR(targetName + "Weights.txt");
        mWeightInterpolator[i] = new0 CubicInterpolator<1,float>(filename);
    }
    else
    {
        // The base target's weights are 1 minus the sum of the weights of
        // the other targets.  There is no need for an interpolator.
        mWeightInterpolator[0] = 0;
    }
}
//----------------------------------------------------------------------------
void MorphFaces::UpdateMorph (float time)
{
    int i, j;

    // Get a pointer to the output vertex buffer.  This buffer is shared by
    // all children, so it suffices to get the pointer from child 0.
    Visual* visual = StaticCast<Visual>(mMorphResult->GetChild(0));
    OutVertex* output = (OutVertex*)visual->GetVertexBuffer()->GetData();

    // Sample the weights at the specified time.  Ensure that the sum of the
    // weights is 1.
    float weights[NUM_TARGETS];
    weights[0] = 1.0f;
    for (i = 1; i < NUM_TARGETS; ++i)
    {
        Tuple<1,float> interp = (*mWeightInterpolator[i])(time);
        weights[i] = interp[0];
        weights[0] -= interp[0];
    }

    // Compute the weighted sums.
    InVertex* inVertex = mVertices[0];
    OutVertex* outVertex = output;
    float weight = weights[0];
    for (j = 0; j < mNumVertices; ++j, ++inVertex, ++outVertex)
    {
        outVertex->Position = weight*inVertex->Position;
        outVertex->Normal = weight*inVertex->Normal;
    }
    for (i = 1; i < NUM_TARGETS; ++i)
    {
        inVertex = mVertices[i];
        outVertex = output;
        weight = weights[i];
        if (weight > 0.0f)
        {
            for (j = 0; j < mNumVertices; ++j, ++inVertex, ++outVertex)
            {
                outVertex->Position += weight*inVertex->Position;
                outVertex->Normal += weight*inVertex->Normal;
            }
        }
    }

    // Normalize the normals.
    outVertex = output;
    for (j = 0; j < mNumVertices; ++j, ++outVertex)
    {
        outVertex->Normal.Normalize();
    }

    // Update the VRAM copy.
    mRenderer->Update(visual->GetVertexBuffer());

    // Update the bounding spheres.
    for (j = 0; j < 4; ++j)
    {
        visual = StaticCast<Visual>(mMorphResult->GetChild(j));
        visual->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    }
    mScene->Update();

    // Update the light colors.
    Tuple<3,float> color = (*mColorInterpolator)(time);
    mLight->Ambient = Float4(color[0], color[1], color[2], 1.0f);
    mLight->Diffuse = mLight->Ambient;
    mLight->Specular = mLight->Ambient;
}
//----------------------------------------------------------------------------
