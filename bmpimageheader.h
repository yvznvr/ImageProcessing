#ifndef BMPIMAGEHEADER_H
#define BMPIMAGEHEADER_H

#include <cstdint>
#include <cstring>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef uint32_t DWORD;


class BmpImageHeader
{
public:
    BmpImageHeader();
    ~BmpImageHeader();
    BmpImageHeader(char*);
    void setImageHeader(char *);
    DWORD getWidth();
    DWORD getHeight();
    WORD getBitCount();
    BYTE* getAllHeader();
    //void setBiBitCount(const WORD &value);
    void setWidth(int);
    void setHeight(int);
    DWORD getBiSizeImage();
    void setSize(DWORD);

private:
    BYTE allData[40];
};

#endif // BMPIMAGEHEADER_H
