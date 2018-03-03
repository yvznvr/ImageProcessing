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
    ~BmpFileHeader();
    BmpFileHeader(char *);
    void setFileHeader(char *);
    WORD getType();
    DWORD getSize();
    DWORD getOffBits();
    bool isBmp();
    BYTE* getAllHeader();
    void setSize(DWORD);

private:
    BYTE allData[14];
};

#endif // BMPFILEHEADER_H
