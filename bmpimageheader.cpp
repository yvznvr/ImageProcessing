#include "bmpimageheader.h"

BmpImageHeader::BmpImageHeader()
{
    allData = new BYTE[40];
}

BmpImageHeader::~BmpImageHeader()
{
    delete[] allData;
}

BmpImageHeader::BmpImageHeader(char *header)
{
    setImageHeader(header);
}

void BmpImageHeader::setImageHeader(char * header)
{
    // set header
    std::memcpy(allData, header, 40);
}

DWORD BmpImageHeader::getWidth()
{
    DWORD biWidth;
    BYTE *temp = allData + 4;
    std::memcpy(&biWidth, temp, sizeof(biWidth));
    return biWidth;
}

DWORD BmpImageHeader::getHeight()
{
    DWORD biHeight;
    BYTE *temp = allData + 8;
    std::memcpy(&biHeight, temp, sizeof(biHeight));
    return biHeight;
}

WORD BmpImageHeader::getBitCount()
{
    WORD biBitCount;
    BYTE *temp = allData + 14;
    std::memcpy(&biBitCount, temp, sizeof(biBitCount));
    return biBitCount;
}

DWORD BmpImageHeader::getBiSizeImage() const
{
    DWORD biSizeImage;
    BYTE *temp = allData + 20;
    std::memcpy(&biSizeImage, temp, sizeof(biSizeImage));
    return biSizeImage;
}

BYTE* BmpImageHeader::getAllHeader()
{
    // return pointer of header
    return allData;
}
/*
void BmpImageHeader::setBiBitCount(const WORD &value)
{
    biBitCount = value;
}
*/
