#include "bmpfile.h"
#include <QDebug>

BmpFile::BmpFile()
{
    fileHeader = new BmpFileHeader;
    imageHeader = new BmpImageHeader;
}

BmpFile::~BmpFile()
{
    delete[] fileHeader;
    delete[] imageHeader;
    delete[] data;
    delete[] dataOfManipulated;
}

void BmpFile::ReadImage(string path)
{
    ifstream file(path, ios::binary); // Open bmp file
    char fileTemp[14]; // temp variable of file header
    file.read(fileTemp, sizeof(fileTemp)); //read file header from image
    fileHeader->setFileHeader(fileTemp); // set file header

    char imageTemp[40]; // temp variable of image header
    file.read(imageTemp, sizeof(imageTemp));
    imageHeader->setImageHeader(imageTemp);


    if(!fileHeader->isBmp())
        qDebug() << "Dosya Bmp Degil";

    file.seekg(fileHeader->getOffBits()); // seek cursor bayt of start data

    data = new BYTE[imageHeader->getBiSizeImage()];
    BYTE *pointerOfData = data;
    BYTE buffer[imageHeader->getWidth()*3]; // create buffer which take 1 row of image
    for(int i=0;i<(int)(imageHeader->getBiSizeImage()/sizeof(buffer));i++)
    {
        file.read((char*)buffer,sizeof(buffer));
        memcpy(pointerOfData,buffer,sizeof(buffer));
        pointerOfData += sizeof(buffer);
    }
    file.close();
}

void BmpFile::ExportImage(string fileName)
{
    /* combine file header, image header and data in one bmp file */
    unsigned char fH[14];
    fileHeader->getAllHeader(fH);
    unsigned char iH[40];
    imageHeader->getAllHeader(iH);
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fH,sizeof(fH));
    out.write((char*)iH,sizeof(iH));
    BYTE* pointerOfData = dataOfManipulated;
    for(int i=0;i<(int)imageHeader->getBiSizeImage()/3;i++)
    {
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData++,1);
    }
    out.close();
}

void BmpFile::ExportImage(string fileName, BYTE *data)
{
    /* combine file header, image header and data in one bmp file */
    unsigned char fH[14];
    fileHeader->getAllHeader(fH);
    unsigned char iH[40];
    imageHeader->getAllHeader(iH);
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fH,sizeof(fH));
    out.write((char*)iH,sizeof(iH));
    BYTE* pointerOfData = data;
    for(int i=0;i<(int)imageHeader->getBiSizeImage()/3;i++)
    {
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData++,1);
    }
    out.close();
}

void BmpFile::grayScale(string outName)
{
    /* create grayscale image and export it */
    dataOfManipulated = new BYTE[imageHeader->getBiSizeImage()/3];
    BYTE mean;
    BYTE *iterator = dataOfManipulated;
    for(int i=0;i<(int)imageHeader->getBiSizeImage();i+=3)
    {
        mean = (data[i]*0.21+data[i+1]*0.72+data[i+2]*0.07);
        *iterator = mean;
        iterator++;
    }

    ExportImage(outName, dataOfManipulated);
}

bool BmpFile::drawRect(int x1, int y1, int x2, int y2)
{
    // draw rectangle point of (x1,y1) to (x2,y2)
    // coordinates start bottom of the picture
    for(int i=y1; i<y2; i++)
    {
        dataOfManipulated[i*imageHeader->getWidth()+x1] = 255;
        dataOfManipulated[i*imageHeader->getWidth()+x2] = 255;
    }
    for(int i=x1; i<x2; i++)
    {
        dataOfManipulated[y1*imageHeader->getWidth()+i] = 255;
        dataOfManipulated[y2*imageHeader->getWidth()+i] = 255;
    }
}

