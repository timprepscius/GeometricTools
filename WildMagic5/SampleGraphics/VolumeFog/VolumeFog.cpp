// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "VolumeFog.h"
#include "VolumeFogEffect.h"

WM5_WINDOW_APPLICATION(VolumeFog);

//----------------------------------------------------------------------------
VolumeFog::VolumeFog ()
    :
    WindowApplication3("SampleGraphics/VolumeFog", 0, 0, 640, 480,
        Float4(0.0f, 0.5f, 0.75f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool VolumeFog::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    APoint camPosition(0.0f, -9.0f, 1.5f);
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

    InitializeCameraMotion(0.005f, 0.002f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void VolumeFog::OnTerminate ()
{
    mScene = 0;
    mMesh = 0;
    mScreenCamera = 0;
    mScreenPolygon = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeFog::OnIdle ()
{
    MeasureTime();

    bool fogNeedsUpdate = false;
    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
        fogNeedsUpdate = true;
    }

    if (MoveObject())
    {
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
        fogNeedsUpdate = true;
    }

    if (fogNeedsUpdate)
    {
        UpdateFog();
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearDepthBuffer();

        // Draw fogged terrain.
        mRenderer->SetCamera(mCamera);
        mRenderer->Draw(mCuller.GetVisibleSet());

        // Draw background image.
        mRenderer->SetCamera(mScreenCamera);
        mRenderer->Draw(mScreenPolygon);

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void VolumeFog::CreateScene ()
{
    // Create a screen-space camera for the background image.
    mScreenCamera = ScreenTarget::CreateCamera();

    // Create a screen polygon for the background image.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mScreenPolygon = ScreenTarget::CreateRectangle(vformat, GetWidth(),
        GetHeight(), 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

    std::string skyName = Environment::GetPathR("BlueSky.wmtf");
    Texture2D* skyTexture = Texture2D::LoadWMTF(skyName);
    Texture2DEffect* skyEffect = new0 Texture2DEffect();
    mScreenPolygon->SetEffectInstance(skyEffect->CreateInstance(skyTexture));

    // Create the scene graph for the terrain.
    mScene = new0 Node();

    // Begin with a flat height field.
    vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mMesh = StandardMesh(vformat).Rectangle(64, 64, 8.0f, 8.0f);
    mScene->AttachChild(mMesh);

    // Set the heights based on a precomputed height field.  Also create a
    // texture image to go with the height field.
    std::string heightFieldName = Environment::GetPathR("HeightField.wmtf");
    Texture2D* heightTexture = Texture2D::LoadWMTF(heightFieldName);
    unsigned char* data = (unsigned char*)heightTexture->GetData(0);
    Float4 white(1.0f, 1.0f, 1.0f, 0.0f);
    VertexBufferAccessor vba(mMesh);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        unsigned char value = *data;
        float height = 3.0f*value/255.0f + 0.05f*Mathf::SymmetricRandom();

        *data++ = (unsigned char)Mathf::IntervalRandom(32.0f, 64.0f);
        *data++ = 3*(128 - value/2)/4;
        *data++ = 0;
        *data++ = 255;

        vba.Position<Float3>(i)[2] = height;

        // The fog color is white.  The alpha channel is filled in by the
        // function UpdateFog().
        vba.Color<Float4>(0, i) = white;
    }

    UpdateFog();

    std::string effectFile = Environment::GetPathR("VolumeFog.wmfx");
    VolumeFogEffect* effect = new0 VolumeFogEffect(effectFile);
    mMesh->SetEffectInstance(effect->CreateInstance(heightTexture));
}
//----------------------------------------------------------------------------
void VolumeFog::UpdateFog ()
{
    // Compute the fog factors based on the intersection of a slab bounded
    // by z = zbot and z = ztop with rays emanating from the camera location
    // to each vertex in the height field.
    const float zBot = 0.0f;  // slab bottom
    const float zTop = 0.5f;  // slab top
    const float fogConstant = 8.0f;  // fog = length/(length + fogconstant)
    float tBot, tTop, invDirZ, length;

    // Having the camera below the height field is not natural, so we just
    // leave the fog values the way they are.
    APoint camPosition = mCamera->GetPosition();
    if (camPosition[2] <= zBot)
    {
        return;
    }

    VertexBufferAccessor vba(mMesh);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        // The ray is E+t*D, where D = V-E with E the eye point and V the
        // vertex.  The ray reaches the vertex at t = 1.
        APoint heightPosition = vba.Position<Float3>(i);

        AVector direction = heightPosition - camPosition;
        float tVmE = direction.Normalize();
        float alpha;

        if (camPosition[2] >= zTop)
        {
            if (direction[2] >= -Mathf::ZERO_TOLERANCE)
            {
                // The ray never intersects the slab, so no fog at vertex.
                alpha = 0.0f;
            }
            else
            {
                // The ray intersects the slab.  Compute the length of the
                // intersection and map it to a fog factor.
                invDirZ = 1.0f/direction[2];
                tTop = (zTop - camPosition[2])*invDirZ;
                if (tTop < tVmE)
                {
                    // The eye is above the slab; the vertex is in or below.
                    tBot = (zBot - camPosition[2])*invDirZ;
                    if (tBot < tVmE)
                    {
                        // The vertex is below the slab.  Clamp to bottom.
                        length = tBot - tTop;
                        alpha = length/(length + fogConstant);
                    }
                    else
                    {
                        // The vertex is inside the slab.
                        length = tVmE - tTop;
                        alpha = length/(length + fogConstant);
                    }
                }
                else
                {
                    // The eye and vertex are above the slab, so no fog.
                    alpha = 0.0f;
                }
            }
        }
        else  // position[2] in (z0,z1)
        {
            if (direction[2] >= Mathf::ZERO_TOLERANCE)
            {
                // The ray intersects the top of the slab.
                invDirZ = 1.0f/direction[2];
                tTop = (zTop - camPosition[2])*invDirZ;
                if (tTop < tVmE)
                {
                    // The vertex is above the slab.
                    alpha = tTop/(tTop + fogConstant);
                }
                else
                {
                    // The vertex is on or inside the slab.
                    alpha = tVmE/(tVmE + fogConstant);
                }
            }
            else if (direction[2] <= -Mathf::ZERO_TOLERANCE)
            {
                // The ray intersects the bottom of the slab.
                invDirZ = 1.0f/direction[2];
                tBot = (zBot - camPosition[2])*invDirZ;
                if (tBot < tVmE)
                {
                    // The vertex is below the slab.
                    alpha = tBot/(tBot + fogConstant);
                }
                else
                {
                    // The vertex is on or inside the slab.
                    alpha = tVmE/(tVmE + fogConstant);
                }
            }
            else
            {
                // The ray is parallel to the slab, so both eye and vertex
                // are inside the slab.
                alpha = tVmE/(tVmE + fogConstant);
            }
        }

        vba.Color<Float4>(0, i)[3] = alpha;
    }

    mRenderer->Update(mMesh->GetVertexBuffer());
}
//----------------------------------------------------------------------------
