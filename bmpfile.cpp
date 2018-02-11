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
    BYTE* pointerOfData = data;
    for(int i=0;i<(int)imageHeader->getBiSizeImage();i++)
    {
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
    for(int i=0;i<(int)imageHeader->getBiSizeImage();i++)
    {
        out.write((char*)pointerOfData++,1);
    }
    out.close();
}

void BmpFile::grayScale(string outName)
{
    /* create grayscale image and export it */
    dataOfManipulated = new BYTE[(fileHeader->getSize()-fileHeader->getOffBits())];
    BYTE mean;
    BYTE *iterator = dataOfManipulated;
    for(int i=0;i<(int)(imageHeader->getBiSizeImage());i+=3)
    {
        //mean = (data->at(i)*0.299+data->at(i+1)*0.587+data->at(i+2)*0.114);
        //mean = (data->at(i)+data->at(i+1)+data->at(i+2))/3;
        mean = (data[i]*0.21+data[i+1]*0.72+data[i+2]*0.07);
        iterator[i] = mean;
        iterator[i+1] = mean;
        iterator[i+2] = mean;
    }

    ExportImage(outName, dataOfManipulated);
    delete[] dataOfManipulated;
}

