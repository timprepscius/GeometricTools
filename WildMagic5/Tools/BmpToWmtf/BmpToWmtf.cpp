// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.1 (2012/07/04)

#include <windows.h>
#include "BmpToWmtf.h"
using namespace Wm5;

WM5_CONSOLE_APPLICATION(BmpToWmtf);

//----------------------------------------------------------------------------
BmpToWmtf::BmpToWmtf ()
    :
    ConsoleApplication("ToolsInternal/BmpToWmtf")
{
}
//----------------------------------------------------------------------------
BmpToWmtf::~BmpToWmtf ()
{
}
//----------------------------------------------------------------------------
bool LoadBMP (const char* name, unsigned char alpha, int& width, int& height,
    unsigned char*& texels)
{
    HBITMAP hImage = (HBITMAP) LoadImage(NULL, name, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!hImage)
    {
        assert(hImage);
        width = 0;
        height = 0;
        texels = 0;
        return false;
    }

    DIBSECTION dibSection;
    GetObject(hImage, sizeof(DIBSECTION), &dibSection);

    width = dibSection.dsBm.bmWidth;
    height = dibSection.dsBm.bmHeight;

    if (dibSection.dsBm.bmBitsPixel == 24)
    {
        // Pad with an alpha channel.
        int numBytes = 4*width*height;
        texels = new1<unsigned char>(numBytes);
        unsigned char* src = (unsigned char*)dibSection.dsBm.bmBits;
        unsigned char* trg = texels;
        for (int i = 0; i < numBytes; ++i)
        {
            if ((i % 4) == 3)
            {
                *trg++ = alpha;
            }
            else
            {
                *trg++ = *src++;
            }
        }
    }
    else if (dibSection.dsBm.bmBitsPixel == 32)
    {
        texels = new1<unsigned char>(4*width*height);
        memcpy(texels, dibSection.dsBm.bmBits, 4*width*height);
    }
    else
    {
        assert(false);
        texels = 0;
        DeleteObject(hImage);
        return false;
    }

    DeleteObject(hImage);
    return true;
}
//----------------------------------------------------------------------------
int BmpToWmtf::Main (int, char**)
{
    // Allow for a constant alpha channel.
    int alphaValue = 255;
    if (TheCommand->GetInteger("a", alphaValue))
    {
        if (alphaValue < 0)
        {
            alphaValue = 0;
        }
        else if (alphaValue > 255)
        {
            alphaValue = 255;
        }
    }
    unsigned char alpha = (unsigned char)alphaValue;

    // Get the BMP name.
    char* bmpname = 0;
    if (!TheCommand->GetFilename(bmpname))
    {
        return -1;
    }

    char name[512];
    sprintf(name, "%s.bmp", bmpname);

    // Load the BMP.
    int width, height;
    unsigned char* texels;
    if (!LoadBMP(name, alpha, width, height, texels))
    {
        delete1(bmpname);
        return -2;
    }

    Texture2D* texture = new0 Texture2D(Texture::TF_A8R8G8B8, width,
        height, 1);

    int format = (int)texture->GetFormat();
    int type = (int)texture->GetTextureType();
    int usage = (int)texture->GetUsage();
    int numLevels = texture->GetNumLevels();
    int numDimensions = texture->GetNumDimensions();
    int numTotalBytes = texture->GetNumTotalBytes();

    int dimension[3][Texture::MM_MAX_MIPMAP_LEVELS];
    int numLevelBytes[Texture::MM_MAX_MIPMAP_LEVELS];
    int levelOffsets[Texture::MM_MAX_MIPMAP_LEVELS];
    for (int level = 0; level < Texture::MM_MAX_MIPMAP_LEVELS; ++level)
    {
        dimension[0][level] = texture->GetDimension(0, level);
        dimension[1][level] = texture->GetDimension(1, level);
        dimension[2][level] = texture->GetDimension(2, level);
        numLevelBytes[level] = texture->GetNumLevelBytes(level);
        levelOffsets[level] = texture->GetLevelOffset(level);
    }

    int userField[Texture::MAX_USER_FIELDS];
    for (int i = 0; i < Texture::MAX_USER_FIELDS; ++i)
    {
        userField[i] = 0;
    }

    sprintf(name, "%s.wmtf", bmpname);
    delete1(bmpname);
    FILE* outFile = fopen(name, "wb");
    if (!outFile)
    {
        assert(outFile);
        delete1(texels);
        delete0(texture);
        return -3;
    }
    fwrite(&format, sizeof(int), 1, outFile);
    fwrite(&type, sizeof(int), 1, outFile);
    fwrite(&usage, sizeof(int), 1, outFile);
    fwrite(&numLevels, sizeof(int), 1, outFile);
    fwrite(&numDimensions, sizeof(int), 1, outFile);
    fwrite(dimension[0], sizeof(int), Texture::MM_MAX_MIPMAP_LEVELS,
        outFile);
    fwrite(dimension[1], sizeof(int), Texture::MM_MAX_MIPMAP_LEVELS,
        outFile);
    fwrite(dimension[2], sizeof(int), Texture::MM_MAX_MIPMAP_LEVELS,
        outFile);
    fwrite(numLevelBytes, sizeof(int), Texture::MM_MAX_MIPMAP_LEVELS,
        outFile);
    fwrite(&numTotalBytes, sizeof(int), 1, outFile);
    fwrite(levelOffsets, sizeof(int), Texture::MM_MAX_MIPMAP_LEVELS,
        outFile);
    fwrite(userField, sizeof(int), Texture::MAX_USER_FIELDS, outFile);
    fwrite(texels, sizeof(unsigned char), numLevelBytes[0], outFile);
    fclose(outFile);

    delete1(texels);
    delete0(texture);
    return 0;
}
//----------------------------------------------------------------------------
