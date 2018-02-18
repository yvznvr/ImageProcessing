#ifndef BMPFILE_H
#define BMPFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include "bmpfileheader.h"
#include "bmpimageheader.h"

typedef unsigned char BYTE;
using namespace std;
class BmpFile
{
public:
    BmpFile();
    ~BmpFile();
    BmpFile(string path);
    void ReadImage(string path);
    void ExportImage(string fileName);
    void ExportImage(string fileName, BYTE *data);
    void manipuleEt();
    void grayScale(string);
    void drawRect(int, int, int, int);
    void drawCircle(float, float, float);
    void drawEllipse(float,float,float,float);
    BmpFileHeader *fileHeader;
    BmpImageHeader *imageHeader;
private:
    BYTE *data;
    BYTE *dataOfManipulated;
};

#endif // BMPFILE_H
