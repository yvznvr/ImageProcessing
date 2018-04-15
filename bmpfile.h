#ifndef BMPFILE_H
#define BMPFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include "bmpfileheader.h"
#include "bmpimageheader.h"
#include "kmeans.h"
#include "multidimensionalarray.h"

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
    void ExportColoredImage(string fileName, BYTE *data);
    void manipuleEt();
    void grayScale(string);
    void drawRect(int, int, int, int);
    void drawCircle(float, float, float);
    void drawEllipse(float,float,float,float);
    void maskApply(int maskRow, int maskColumn, float *mask);
    void zoom(int x1, int y1, int x2, int y2);
    BmpFileHeader *fileHeader;
    BmpImageHeader *imageHeader;
    void setPadding(int value);
    BYTE *getData() const;
    BYTE *getDataOfManipulated() const;
    void setData(BYTE *value);
    int* histogramData();
    void histogramEqualization();
    void kmeans();
    void coloredKmeans();
    void erosion(int maskRow, int maskColumn, float *mask);
    void dilation(int maskRow, int maskColumn, float *mask);

private:
    BYTE *data;
    BYTE *dataOfManipulated;
    BYTE *binaryImage;
    int padding = 0;
};

#endif // BMPFILE_H
