// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "SaveBMP32.h"
#include "Wm5Memory.h"

// Define structs to avoid having to #include<windows.h>.  This allows the
// code to run on any little-endian machine.  Be aware that Microsoft has
// a hack for the definition of BITMAPFILEHEADER that forces it to be 2-byte
// packed; that is, the bfSize immediately follows bfType in memory without
// padding bfType by two more bytes (for 4-byte packing).  Big endian,
// such as PowerPC Macintosh needs byte swapping.
struct PrivateBitMapFileHeader
{
    unsigned short  bfType;
    unsigned long   bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned long   bfOffBits;
};
static const size_t sizeOfBitMapFileHeader = 14;  // the packed size

struct PrivateBitMapInfoHeader
{
    unsigned long   biSize;
    long            biWidth;
    long            biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned long   biCompression;
    unsigned long   biSizeImage;
    long            biXPelsPerMeter;
    long            biYPelsPerMeter;
    unsigned long   biClrUsed;
    unsigned long   biClrImportant;
};
const size_t sizeOfBitMapInfoHeader = sizeof(PrivateBitMapInfoHeader);

static const int BI_RGB = 0;

//----------------------------------------------------------------------------
bool SaveBMP32 (const std::string& name, const Image2<PixelBGRA8>& image)
{
    FILE* outFile = fopen(name.c_str(), "wb");
    if (!outFile)
    {
        assertion(false, "Cannot open file '%s' for write.\n", name.c_str());
        return false;
    }

    const int numPixels = image.GetNumPixels();
    const int numBytes = 4*image.GetNumPixels();
    const int size = sizeOfBitMapFileHeader + sizeOfBitMapInfoHeader;

    PrivateBitMapFileHeader fileHeader;
    fileHeader.bfType = 0x4d42;
    fileHeader.bfSize = size + numBytes;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = size;
    size_t numWritten = fwrite(&fileHeader.bfType, sizeof(unsigned short),
        1, outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (bfType).\n");
        fclose(outFile);
        return false;
    }

    numWritten = fwrite(&fileHeader.bfSize, sizeof(unsigned long), 1,
        outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (bfSize).\n");
        fclose(outFile);
        return false;
    }

    numWritten = fwrite(&fileHeader.bfReserved1, sizeof(unsigned short), 1,
        outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (bfReserved1).\n");
        fclose(outFile);
        return false;
    }

    numWritten = fwrite(&fileHeader.bfReserved2, sizeof(unsigned short), 1,
        outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (bfReserved2).\n");
        fclose(outFile);
        return false;
    }

    numWritten = fwrite(&fileHeader.bfOffBits, sizeof(unsigned long), 1,
        outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (bfOffBits).\n");
        fclose(outFile);
        return false;
    }

    PrivateBitMapInfoHeader infoHeader;
    infoHeader.biSize = sizeOfBitMapInfoHeader;
    infoHeader.biWidth = image.GetDimension(0);
    infoHeader.biHeight = image.GetDimension(1);
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;
    numWritten = fwrite(&infoHeader, sizeOfBitMapInfoHeader, 1, outFile);
    if (numWritten != 1)
    {
        assertion(false, "Failed to write (infoHeader).\n");
        fclose(outFile);
        return false;
    }

    PixelBGRA8* source = image.GetPixels1D();
    numWritten = fwrite(source, sizeof(PixelBGRA8), numPixels, outFile);
    if (numWritten != (size_t)numPixels)
    {
        assertion(false, "Failed to write (source).\n");
        fclose(outFile);
        return false;
    }

    fclose(outFile);
    return true;
}
//----------------------------------------------------------------------------
