#include "bmpimageheader.h"

BmpImageHeader::BmpImageHeader()
{
}

BmpImageHeader::~BmpImageHeader()
{
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

DWORD BmpImageHeader::getBiSizeImage()
{
    DWORD biSizeImage;
    BYTE *temp = allData + 20;
    std::memcpy(&biSizeImage, temp, sizeof(biSizeImage));
    return biSizeImage;
}

void BmpImageHeader::setSize(DWORD size)
{
    BYTE *temp = allData + 20;
    std::memcpy(temp,&size,4);
}

BYTE* BmpImageHeader::getAllHeader()
{
    // return pointer of header
    return allData;
}

void BmpImageHeader::setWidth(int width)
{
    BYTE *pointerOfData = allData + 4;
    std::memcpy(pointerOfData, &width, 4);
}

void BmpImageHeader::setHeight(int height)
{
    BYTE *pointerOfData = allData + 8;
    std::memcpy(pointerOfData, &height, 4);
}

/*
void BmpImageHeader::setBiBitCount(const WORD &value)
{
    biBitCount = value;
}
*/
