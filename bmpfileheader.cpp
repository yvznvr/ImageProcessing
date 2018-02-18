#include "bmpfileheader.h"


BmpFileHeader::BmpFileHeader()
{

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
    /* seperate content of header */
    char* pointerOfHeader = header;
    std::memcpy(&bfType, header, sizeof(bfType));
    pointerOfHeader+=sizeof(bfType);
    std::memcpy(&bfSize, pointerOfHeader, sizeof(bfSize));
    pointerOfHeader+=sizeof(bfSize);
    std::memcpy(&bfReserved1, pointerOfHeader, sizeof(bfReserved1));
    pointerOfHeader+=sizeof(bfReserved1);
    std::memcpy(&bfReserved2, pointerOfHeader, sizeof(bfReserved2));
    pointerOfHeader+=sizeof(bfReserved2);
    std::memcpy(&bfOffBits, pointerOfHeader, sizeof(bfOffBits));
}

WORD BmpFileHeader::getType()
{
    return bfType;
}

DWORD BmpFileHeader::getSize()
{
    return bfSize;
}

DWORD BmpFileHeader::getOffBits()
{
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
    // write all header in data array
    allData = new BYTE[14];
    BYTE *temp = allData;
    std::memcpy(temp, &bfType, sizeof(WORD));
    std::memcpy(temp+=sizeof(bfType), &bfSize, sizeof(DWORD));
    std::memcpy(temp+=sizeof(bfSize), &bfReserved1, sizeof(WORD));
    std::memcpy(temp+=sizeof(bfReserved1), &bfReserved2, sizeof(WORD));
    std::memcpy(temp+=sizeof(bfReserved2), &bfOffBits, sizeof(DWORD));
    return allData;
}

