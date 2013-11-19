// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MeshEnvelope.h"
#include "CreateEnvelope.h"

WM5_WINDOW_APPLICATION(MeshEnvelope);

//----------------------------------------------------------------------------
MeshEnvelope::MeshEnvelope ()
    :
    WindowApplication2("SampleMathematics/MeshEnvelope", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mVertices = 0;
    mIndices = 0;
    mEnvelopeVertices = 0;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool MeshEnvelope::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Load the 3D face mesh.
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("FacePN.wmof");
#else
    std::string path = Environment::GetPathR("FacePN.be.wmof");
#endif
    InStream inStream;
    inStream.Load(path);
    TriMeshPtr mesh = StaticCast<TriMesh>(inStream.GetObjectAt(0));
    VertexBufferAccessor vba(mesh);
    mNumVertices = vba.GetNumVertices();

#if 1
    // Rotate the mesh just to see how the algorithm behaves.
    Matrix3f rotate(Vector3f::UNIT_X, Mathf::HALF_PI);
    for (int j = 0; j < mNumVertices; ++j)
    {
        vba.Position<Vector3f>(j) = rotate*vba.Position<Vector3f>(j);
    }
#endif

    // Project the mesh onto the xy-plane.
    mVertices = new1<Vector2f>(mNumVertices);
    float x = vba.Position<Vector3f>(0).X();
    float y = vba.Position<Vector3f>(0).Y();
    mVertices[0].X() = x;
    mVertices[0].Y() = y;
    float xmin = x, xmax = x;
    float ymin = y, ymax = y;
    int i;
    for (i = 1; i < mNumVertices; ++i)
    {
        x = vba.Position<Vector3f>(i).X();
        y = vba.Position<Vector3f>(i).Y();
        mVertices[i][0] = x;
        mVertices[i][1] = y;

        if (x < xmin)
        {
            xmin = x;
        }
        else if (x > xmax)
        {
            xmax = x;
        }

        if (y < ymin)
        {
            ymin = y;
        }
        else if (y > ymax)
        {
            ymax = y;
        }
    }

    // Transform the mesh into window coordinates.
    float xOffset = 0.125f*mSize;
    float yOffset = 0.125f*mSize;
    float xMult = (0.75f*mSize)/(xmax - xmin);
    float yMult = (0.75f*mSize)/(ymax - ymin);
    for (i = 0; i < mNumVertices; ++i)
    {
        mVertices[i].X() = xOffset + xMult*(mVertices[i].X() - xmin);
        mVertices[i].Y() = yOffset + yMult*(mVertices[i].Y() - ymin);
    }

    // Copy the mesh connectivity information.
    IndexBuffer* ibuffer = mesh->GetIndexBuffer();
    mNumIndices = ibuffer->GetNumElements();
    mIndices = new1<int>(mNumIndices);
    size_t numBytes = mNumIndices*sizeof(int);
    memcpy(mIndices, ibuffer->GetData(), numBytes);

    // Create the envelope of the mesh.
    CreateEnvelope(mNumVertices, mVertices, mNumIndices, mIndices,
        mNumEnvelopeVertices, mEnvelopeVertices);

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void MeshEnvelope::OnTerminate ()
{
    delete1(mVertices);
    delete1(mIndices);
    delete1(mEnvelopeVertices);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void MeshEnvelope::OnDisplay ()
{
    ClearScreen();

    // Draw the mesh triangles.
    ColorRGB black(0, 0, 0);
    int i, v0, v1, v2, x0, y0, x1, y1, x2, y2;
    for (i = 0; i < mNumIndices; /**/)
    {
        v0 = mIndices[i++];
        v1 = mIndices[i++];
        v2 = mIndices[i++];
        x0 = (int)mVertices[v0].X();
        y0 = (int)mVertices[v0].Y();
        x1 = (int)mVertices[v1].X();
        y1 = (int)mVertices[v1].Y();
        x2 = (int)mVertices[v2].X();
        y2 = (int)mVertices[v2].Y();
        DrawLine(x0, y0, x1, y1, black);
        DrawLine(x1, y1, x2, y2, black);
        DrawLine(x2, y2, x0, y0, black);
    }

    // Draw the mesh envelope.
    ColorRGB red(255, 0, 0);
    x0 = (int)mEnvelopeVertices[0].X();
    y0 = (int)mEnvelopeVertices[0].Y();
    x1 = x0;
    y1 = y0;
    for (i = 1; i < mNumEnvelopeVertices; ++i)
    {
        x2 = (int)mEnvelopeVertices[i].X();
        y2 = (int)mEnvelopeVertices[i].Y();
        DrawLine(x1, y1, x2, y2, red);
        x1 = x2;
        y1 = y2;
    }
    DrawLine(x1, y1, x0, y0, red);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
