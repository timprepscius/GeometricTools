// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2012/07/04)

#include <windows.h>
#include <cassert>
#include <cstdio>
#pragma warning(disable:4996)

//----------------------------------------------------------------------------
void LoadBMP (const char* name, int& width, int& height,
    unsigned char*& data)
{
    HBITMAP hImage = (HBITMAP) LoadImage(NULL, name, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    assert(hImage);

    DIBSECTION dibSection;
    GetObject(hImage, sizeof(DIBSECTION), &dibSection);

    width = dibSection.dsBm.bmWidth;
    height = dibSection.dsBm.bmHeight;
    assert(dibSection.dsBm.bmBitsPixel == 24);
    data = new unsigned char[3*width*height];
    memcpy(data, dibSection.dsBm.bmBits, 3*width*height);

    DeleteObject(hImage);
}
//----------------------------------------------------------------------------
void SaveBMP (const char* name, int width, int height, unsigned char* data)
{
    assert((width % 4) == 0);
    int quantity = width*height;
    int numBytes = 3*quantity;

    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4d42;  // "BM"
    fileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        numBytes;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader;
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    FILE* outFile = fopen(name, "wb");
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, outFile);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, outFile);
    fwrite(data, sizeof(unsigned char), numBytes, outFile);
    fclose(outFile);
}
//----------------------------------------------------------------------------
int main (int, char** arguments)
{
    int width, height;
    unsigned char* data;
    LoadBMP(arguments[1], width, height, data);

    unsigned char* color = data;
    for (int i = 0; i < width*height; ++i, color += 3)
    {
        float b = (float)color[0];
        float g = (float)color[1];
        float r = (float)color[2];
        unsigned char gray = (unsigned char)(0.30f*r + 0.59f*g + 0.11f*b);
        color[0] = gray;
        color[1] = gray;
        color[2] = gray;
    }

    SaveBMP(arguments[2], width, height, data);
    delete[] data;
    return 0;
}
//----------------------------------------------------------------------------
