#ifndef BMPFILEHEADER_H
#define BMPFILEHEADER_H

#include <cstring>
#include <cstdint>
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef uint32_t DWORD;


class BmpFileHeader
{
public:
    BmpFileHeader();
    BmpFileHeader(char *);
    void setFileHeader(char *);
    WORD getType();
    DWORD getSize();
    DWORD getOffBits();
    bool isBmp();
    void getAllHeader(BYTE *);

private:
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};

#endif // BMPFILEHEADER_H
