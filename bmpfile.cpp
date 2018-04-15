#include "bmpfile.h"
#include <QDebug>
#include <math.h>

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
    delete[] binaryImage;
}

void BmpFile::ReadImage(string path)
{
    padding = 0;
    ifstream file(path, ios::binary); // Open bmp file
    char fileTemp[14]; // temp variable of file header
    file.read(fileTemp, sizeof(fileTemp)); //read file header from image
    fileHeader->setFileHeader(fileTemp); // set file header

    char imageTemp[40]; // temp variable of image header
    file.read(imageTemp, sizeof(imageTemp));
    imageHeader->setImageHeader(imageTemp);

    if(!fileHeader->isBmp())
    {
        qDebug() << "Dosya Bmp Degil";
        return;
    }

    file.seekg(fileHeader->getOffBits()); // seek cursor bayt of start data
    while((imageHeader->getWidth()*3+padding)%4 != 0)   padding++;
    data = new BYTE[imageHeader->getBiSizeImage()];
    BYTE *pointerOfData = data;
    BYTE buffer[imageHeader->getWidth()*3]; // create buffer which take 1 row of image
    for(int i=0;i<(int)(imageHeader->getBiSizeImage()/sizeof(buffer));i++)
    {
        file.read((char*)buffer,sizeof(buffer));
        memcpy(pointerOfData,buffer,sizeof(buffer));
        pointerOfData += sizeof(buffer);
        file.read((char*)buffer,padding); // move cursor for padding
    }
    file.close();
}

void BmpFile::ExportImage(string fileName)
{
    /* combine file header, image header and data in one bmp file */
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fileHeader->getAllHeader(),14);
    out.write((char*)imageHeader->getAllHeader(),40);
    BYTE* pointerOfData = dataOfManipulated;
    unsigned int pixelNumber = 0; // for padding
    for(int i=0;i<(int)imageHeader->getBiSizeImage()/3;i++)
    {
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData++,1);
        pixelNumber++;
        if(pixelNumber==imageHeader->getWidth())
        {
            BYTE pad = 0;
            for(int i=0;i<padding;i++) out.write((char*)&pad,1);
            pixelNumber = 0;
        }
    }
    out.close();
}

void BmpFile::ExportImage(string fileName, BYTE *data)
{
    /* combine file header, image header and data in one bmp file */
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fileHeader->getAllHeader(),14);
    out.write((char*)imageHeader->getAllHeader(),40);
    BYTE* pointerOfData = data;
    unsigned int pixelNumber = 0; // for padding
    for(int i=0;i<(int)imageHeader->getBiSizeImage()/3;i++)
    {
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData,1);
        out.write((char*)pointerOfData++,1);
        pixelNumber++;
        if(pixelNumber==imageHeader->getWidth())
        {
            BYTE pad = 0;
            for(int i=0;i<padding;i++) out.write((char*)&pad,1);
            pixelNumber = 0;
        }
    }
    out.close();
}

void BmpFile::ExportColoredImage(string fileName, BYTE *data)
{
    /* combine file header, image header and data in one bmp file */
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fileHeader->getAllHeader(),14);
    out.write((char*)imageHeader->getAllHeader(),40);
    BYTE* pointerOfData = data;
    unsigned int pixelNumber = 0; // for padding
    for(int i=0;i<(int)imageHeader->getBiSizeImage();i++)
    {
        out.write((char*)pointerOfData++,1);
        out.write((char*)pointerOfData++,1);
        out.write((char*)pointerOfData++,1);
        pixelNumber++;
        if(pixelNumber==imageHeader->getWidth())
        {
            BYTE pad = 0;
            for(int i=0;i<padding;i++) out.write((char*)&pad,1);
            pixelNumber = 0;
        }
    }
    out.close();
}

void BmpFile::grayScale(string outName)
{
    /* create grayscale image and export it */
    dataOfManipulated = new BYTE[imageHeader->getBiSizeImage()/3];
    BYTE *iterator = dataOfManipulated;
    for(int i=0;i<(int)imageHeader->getBiSizeImage();i+=3)
    {
        *iterator = BYTE((data[i]*0.21+data[i+1]*0.72+data[i+2]*0.07));
        iterator++;
    }

    ExportImage(outName);
}

void BmpFile::drawRect(int x1, int y1, int x2, int y2)
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

void BmpFile::drawCircle(float x, float y, float r)
{
    // draw circle point of center is (x,y) and radius is r
    for(float i=0;i<=2*M_PI;i+=1/r)
    {
        dataOfManipulated[(int)(y+(r*sin(i)))*imageHeader->getWidth()+(int)(x+(r*cos(i)))] = 255;
    }
}

void BmpFile::drawEllipse(float x, float y, float i, float j)
{
    // draw ellipse of center (x,y) and i is
    float divider = i;  // divider for step size
    if (i<j) divider=j; // greater radius must be divider
    for(float k=0;k<=2*M_PI;k+=1/divider)
    {
        dataOfManipulated[(int)(y+(j*sin(k)))*imageHeader->getWidth()+(int)(x+(i*cos(k)))] = 255;
    }
}

void BmpFile::maskApply(int maskRow, int maskColumn, float *mask)
{
    // move mask on the image
    grayScale("outputs/output");
    int size = (imageHeader->getHeight()-2)*(imageHeader->getWidth()-2);
    BYTE *maskedData = new BYTE[size];
    BYTE sum=0;

    for(unsigned int h=0;h<imageHeader->getHeight()-2;h++)
    {
        for(unsigned int w=0;w<imageHeader->getWidth()-2;w++)
        {
            for(int i=0;i<maskColumn;i++)
            {
                for(int j=0;j<maskRow;j++)
                {
                    sum += mask[i*maskRow+j] * dataOfManipulated[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth()-2);
            maskedData[adres] = sum;
            sum = 0;
        }
    }
    padding = 0;
    while(((imageHeader->getWidth()-2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()-2));
    imageHeader->setWidth((imageHeader->getWidth()-2));
    ExportImage("outputs/maske", maskedData);
    delete[] maskedData;
}

void BmpFile::zoom(int x1, int y1, int x2, int y2)
{
    // write zero between of pixels
    int width = (x2-x1)*2+1;
    int height = (y2-y1)*2+1;
    BYTE *buffer = new BYTE[width*height]();
    BmpFile *newBmp = new BmpFile;
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width+1;j++)
        {
            if((i%2==0) || (j%2==0))
            {
                buffer[i*width+j] = 0;
            }
            else
            {
                buffer[i*width+j] = dataOfManipulated[(y1+(i/2-1))*imageHeader->getWidth()+(x1+(j/2-1))];
            }
        }
    }
    // some header process ex: size, width, height etc
    DWORD newImageSize = width*height*3+54;
    int newPadding = 0;
    while((width*3+newPadding)%4 != 0)   newPadding++;
    newBmp->fileHeader->setFileHeader((char*)fileHeader->getAllHeader());
    newBmp->fileHeader->setSize(newImageSize);
    newBmp->setPadding(newPadding);
    newBmp->imageHeader->setImageHeader((char*)imageHeader->getAllHeader());
    newBmp->imageHeader->setHeight(height);
    newBmp->imageHeader->setWidth(width);
    newBmp->imageHeader->setSize(newImageSize-54);
    newBmp->ExportImage("outputs/zoom", buffer);
    delete[] buffer;
}

void BmpFile::setPadding(int value)
{
    padding = value;
}

BYTE *BmpFile::getData() const
{
    return data;
}

BYTE *BmpFile::getDataOfManipulated() const
{
    return dataOfManipulated;
}

void BmpFile::setData(BYTE *value)
{
    data = new BYTE[sizeof(value)];
    std::memcpy(data,value,sizeof(*value));
}

int *BmpFile::histogramData()
{
    int *histogram = new int[256]();
    for(unsigned long i=0;i<(imageHeader->getHeight()*imageHeader->getWidth());i++)
    {
        histogram[(unsigned long)dataOfManipulated[i]]++;
    }
    return histogram;
}

void BmpFile::histogramEqualization()
{
    int *histogram = histogramData();  // get histogram

    unsigned int normalized[256];
    normalized[0] = histogram[0];       // set first element
    // cumulative histogram data
    int temp = histogram[0];
    for(int i=1;i<256;i++)
    {
        temp += histogram[i];
        normalized[i] = (histogram[i] + normalized[i-1]);
    }

    float numberOfPixels = imageHeader->getHeight()*imageHeader->getWidth();

    // find max gray level value
    int maxGray(0);
    for (int i=255;i>0;i--)
    {
        if(histogram[i])
        {
            maxGray = i;
            break;
        }
    }

    // normalize histogram data
    for (int i=0;i<256;i++)
        normalized[i] = normalized[i]/(float)numberOfPixels*maxGray;

    // mapping
    for(int i=0; i<numberOfPixels;i++)
    {
        dataOfManipulated[i] = normalized[dataOfManipulated[i]];
    }
}

void BmpFile::kmeans()
{
    constexpr int set = 2;
    kMeans<int,set> means;
    int numberOfPixels = imageHeader->getHeight() * imageHeader->getWidth();
    means.setOrnekSayisi(numberOfPixels);
    for(int i=0; i<numberOfPixels; i++)
    {
        means.setDataList((int)dataOfManipulated[i]);
    }

    means.initMeans();
    while(!means.kumele())
        means.initMeans();

    int colours[set];
    int pad = 255/(set-1);
    int color=0;
    for (int i=0;i<set;i++)
    {
        colours[i] = color;
        color+=pad;
    }

    //int *colours = means.getMerkez();
    int *labeledData = means.getetiket();
    BYTE *buffer = new BYTE[numberOfPixels];
    for(int i=0;i<numberOfPixels;i++)
        buffer[i] = colours[labeledData[i]];
    ExportImage("outputs/kmeans", (BYTE*)buffer);
    delete[] buffer;
}

void BmpFile::coloredKmeans()
{
    constexpr int set = 2;
    kMeans<MultiDimensionalArray,set> means;
    int numberOfPixels = imageHeader->getHeight() * imageHeader->getWidth();
    means.setOrnekSayisi(numberOfPixels);
    for(int i=0; i<numberOfPixels*3; i+=3)
    {
        MultiDimensionalArray temp(3);
        int point[3] = {data[i],data[i+1],data[i+2]};
        temp.append(point);
        means.setDataList(temp);
    }

    means.initMeans();
    while(!means.kumele())
        means.initMeans();

    int colours[3] = {0,255};
    int *labeledData = means.getetiket();
    BYTE *buffer = new BYTE[numberOfPixels];
    for(int i=0;i<numberOfPixels;i++)
    {
        buffer[i] = colours[labeledData[i]];
    }
    ExportImage("outputs/kmeans2", (BYTE*)buffer);
    delete[] buffer;
}

void BmpFile::erosion(int maskRow, int maskColumn, float *mask)
{
    int size = (imageHeader->getHeight())*(imageHeader->getWidth());
    BYTE* buffer = new BYTE[size]();
    int sum=1;

    for(unsigned int h=1;h<imageHeader->getHeight()-1;h++)
    {
        for(unsigned int w=1;w<imageHeader->getWidth()-1;w++)
        {
            for(int i=0;i<maskColumn;i++)
            {
                for(int j=0;j<maskRow;j++)
                {
                    sum += mask[i*maskRow+j] && dataOfManipulated[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth());
            if(sum==5)
                buffer[adres] = 255;
            sum = 0;
        }
    }
    ExportImage("outputs/kmeans", buffer);
}

void BmpFile::dilation(int maskRow, int maskColumn, float *mask)
{
    int size = (imageHeader->getHeight())*(imageHeader->getWidth());
    BYTE* buffer = new BYTE[size]();
    int sum=1;

    for(unsigned int h=1;h<imageHeader->getHeight()-1;h++)
    {
        for(unsigned int w=1;w<imageHeader->getWidth()-1;w++)
        {
            for(int i=0;i<maskColumn;i++)
            {
                for(int j=0;j<maskRow;j++)
                {
                    sum += mask[i*maskRow+j] && dataOfManipulated[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth());
            if(sum>1)
                buffer[adres] = 255;
            sum = 0;
        }
    }
    ExportImage("outputs/kmeans", buffer);
}

