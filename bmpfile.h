#ifndef BMPFILE_H
#define BMPFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
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
    void maskApply(BYTE* image, float* buffer, int maskRow, int maskColumn, float *mask);
    void zoom(int x1, int y1, int x2, int y2);
    BmpFileHeader *fileHeader;
    BmpImageHeader *imageHeader;
    void setPadding(int value);
    BYTE *getData() const;
    BYTE *getGrayImage() const;
    void setData(BYTE *value);
    int* histogramData();
    void histogramEqualization();
    void kmeans();
    void coloredKmeans();
    void erosion(int maskRow, int maskColumn, float *mask);
    void dilation(int maskRow, int maskColumn, float *mask);
    int *labeledObjects();
    void copyDataToBinary();
    void findCoor(BYTE*);
    void drawRectColored(int x1, int y1, int x2, int y2, int red, int green, int blue);
    void ExportColoredImage(string fileName);
    float euclideanDistance(float *arr1, float *arr2, int size);
    void cannyEdgeDetection();
    void houghTransform(int *ang, BYTE* image);
    void drawLine(int angle, int distance, int red, int green, int blue);
private:
    BYTE *data;
    BYTE *grayImage;
    BYTE *binaryImage;
    int padding = 0;
};
#endif // BMPFILE_H
