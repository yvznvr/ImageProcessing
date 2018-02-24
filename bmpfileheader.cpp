#include "bmpfileheader.h"


BmpFileHeader::BmpFileHeader()
{
    allData = new BYTE[14];
}

BmpFileHeader::~BmpFileHeader()
{
    delete[] allData;
}

BmpFileHeader::BmpFileHeader(char *header)
{
    setFileHeader(header);
}

void BmpFileHeader::setFileHeader(char* header)
{
    /* set header */
    std::memcpy(allData, header, 14);
}

WORD BmpFileHeader::getType()
{
    WORD bfType;
    std::memcpy(&bfType, allData, sizeof(bfType));
    return bfType;
}

DWORD BmpFileHeader::getSize()
{
    DWORD bfSize;
    BYTE *temp = allData + 2;
    std::memcpy(&bfSize, temp, sizeof(bfSize));
    return bfSize;
}

DWORD BmpFileHeader::getOffBits()
{
    DWORD bfOffBits;
    BYTE *temp = allData + 10;
    std::memcpy(&bfOffBits, temp, sizeof(bfOffBits));
    return bfOffBits;
}

bool BmpFileHeader::isBmp()
{
    // return true if file is bmp file
    if(getType()!=0x4D42)
        return false;
    return true;
}

BYTE* BmpFileHeader::getAllHeader()
{
    // return pointer of header
    return allData;
}

