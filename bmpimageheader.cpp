#include "bmpimageheader.h"

BmpImageHeader::BmpImageHeader()
{

}

BmpImageHeader::BmpImageHeader(char *header)
{
    setImageHeader(header);
}

void BmpImageHeader::setImageHeader(char * header)
{
    // seperate content of header
    char *pointerOfHeader = header;
    std::memcpy(&biSize, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biWidth, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biHeight, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biPlanes, pointerOfHeader, sizeof(WORD));
    pointerOfHeader+=sizeof(WORD);
    std::memcpy(&biBitCount, pointerOfHeader, sizeof(WORD));
    pointerOfHeader+=sizeof(WORD);
    std::memcpy(&biCompression, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biSizeImage, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biXPelsPerMeter, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biYPelsPerMeter, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biClrUsed, pointerOfHeader, sizeof(DWORD));
    pointerOfHeader+=sizeof(DWORD);
    std::memcpy(&biClrImportant, pointerOfHeader, sizeof(DWORD));
}

DWORD BmpImageHeader::getWidth()
{
    return biWidth;
}

DWORD BmpImageHeader::getHeight()
{
    return biHeight;
}

WORD BmpImageHeader::getBitCount()
{
    return biBitCount;
}

void BmpImageHeader::getAllHeader(unsigned char *data)
{
    // write all header in data array
    BYTE *temp = data;
    std::memcpy(temp, &biSize, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biSize), &biWidth, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biWidth), &biHeight, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biHeight), &biPlanes, sizeof(WORD));
    std::memcpy(temp+=sizeof(biPlanes), &biBitCount, sizeof(WORD));
    std::memcpy(temp+=sizeof(biBitCount), &biCompression, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biCompression), &biSizeImage, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biSizeImage), &biXPelsPerMeter, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biXPelsPerMeter), &biYPelsPerMeter, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biYPelsPerMeter), &biClrUsed, sizeof(DWORD));
    std::memcpy(temp+=sizeof(biClrUsed), &biClrImportant, sizeof(DWORD));
}

void BmpImageHeader::setBiBitCount(const WORD &value)
{
    biBitCount = value;
}

DWORD BmpImageHeader::getBiSizeImage() const
{
    return biSizeImage;
}
