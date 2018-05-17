#include "bmpfile.h"
#include <QDebug>
#include <math.h>
#include <humoments.h>

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
    delete[] grayImage;
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

void BmpFile::ExportColoredImage(string fileName)
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

float BmpFile::euclideanDistance(float *arr1, float *arr2, int size)
{
    float temp=0;
    for(int i=0;i<size;i++)
    {
        temp += pow((arr1[i] - arr2[i]),2);
    }
    return sqrt(temp);
}

void BmpFile::cannyEdgeDetection()
{
    // canny edge detection algorithm
    int height = imageHeader->getHeight() - 2;
    int width = imageHeader->getWidth() - 2;
    int size = height*width;
    float image[size];
    float derivateOfX[9] = {1,0,-1,2,0,-2,1,0,-1};
    float derivateOfY[9] = {1,2,1,0,0,0,-1,-2,-1};
    float imageX[size], imageY[size];
    int angle[size], thisAngle;
    grayScale("outputs/output");
    maskApply(grayImage, imageY, 3, 3, derivateOfY);
    maskApply(grayImage, imageX, 3, 3, derivateOfX);
    int max=0, min=255;
    int* angleForHough = new int[size];

    // gradiant angle and magnitude
    for(int i=0; i<size; i++)
    {
        image[i] = sqrt(pow(imageX[i], 2) + pow(imageY[i], 2));
//        image[i] = abs(imageX[i]) + abs(imageY[i]);
        thisAngle = (atan2(imageY[i], imageX[i])/M_PI) * 180.0;
        angleForHough[i] = thisAngle;

        if ( ( (thisAngle < 22.5) && (thisAngle > -22.5) ) || (thisAngle > 157.5) || (thisAngle < -157.5) )
            angle[i] = 0;
        else if ( ( (thisAngle > 22.5) && (thisAngle < 67.5) ) || ( (thisAngle < -112.5) && (thisAngle > -157.5) ) )
            angle[i] = 1;
        else if ( ( (thisAngle > 67.5) && (thisAngle < 112.5) ) || ( (thisAngle < -67.5) && (thisAngle > -112.5) ) )
            angle[i] = 2;
        else if ( ( (thisAngle > 112.5) && (thisAngle < 157.5) ) || ( (thisAngle < -22.5) && (thisAngle > -67.5) ) )
            angle[i] = 3;
        if(image[i]<min) min = image[i];
        if(image[i]>max) max = image[i];
    }

    // non-max suppression
//    int directions[8] = {1,0,1,1,0,1,-1,1};
    int directions[8] = {0,1,1,1,1,0,1,-1};
    for(int h=0;h<height-1;h++)
    {
        for(int r=0;r<width-1;r++)
        {
            int ang = angle[h*width+r];
            if(image[h*width+r]<=image[(h+directions[2*ang+1])*width+r+directions[2*ang]])
               image[h*width+r] = 0;
        }
    }

    // Hysteresis Thresholding
    int hysteresis = (max-min)*0.15;
    for (int i=0;i<size;i++)
    {
        if(image[i]>max-hysteresis) image[i] = max;
        else if(image[i]<hysteresis) image[i] = 0;
    }


    for(int h=0;h<height-1;h++)
    {
        for(int r=0;r<width-1;r++)
        {
            int ang = 3 - angle[h*width+r];
            if(image[h*width+r]<=image[(h+directions[2*ang+1])*width+r+directions[2*ang]])
               image[h*width+r] = 0;
            else image[h*width+r] = max;
        }
    }

    // normalize
    BYTE im[size];
    for (int i=0; i<size; i++)
        im[i] = image[i]*255/max;

    padding = 0;
    while(((imageHeader->getWidth()-2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()-2));
    imageHeader->setWidth((imageHeader->getWidth()-2));
    ExportImage("outputs/canny", im);
    houghTransform(angle, im);
    delete[] angleForHough;
}

void BmpFile::houghTransform(int *ang, BYTE *image)
{
    int height = imageHeader->getHeight();
    int width = imageHeader->getWidth();
    int maxLength;
    if(height>width) maxLength=height*1.42; // 1.42 = 2^1/2
    else maxLength=width*1.42; // 1.42 = 2^1/2

    int *houghArray = new int[360*maxLength];
    for (int i=0;i<360*maxLength;i++) houghArray[i]=0;

    int realAngle[4] = {0,45,90,135};

    for (int r=0; r<height; r++)
    {
        for (int c=0; c<width; c++)
        {
            if (image[r*width+c] == 255)
            {
                int tempAngle = realAngle[ang[r*width+c]];
                int d = abs(c*sin(tempAngle*M_PI/180) + r*cos(tempAngle*M_PI/180));
                houghArray[tempAngle+360*d] = houghArray[tempAngle+360*d] + 1;
            }
        }
    }


    for (int r=0; r<maxLength; r++)
    {
        for (int c=0; c<360; c++)
        {
            int dist = houghArray[r*360+c];
            if((dist>500) && c==0) drawLine(0, r+1, 255,0,0);
            else if((dist>500) && c==90) drawLine(90, r+1, 0,0,255);
        }
    }

    padding = 0;
    while(((imageHeader->getWidth()-2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()+2));
    imageHeader->setWidth((imageHeader->getWidth()+2));
    ExportColoredImage("outputs/hough");
    delete[] houghArray;
}

void BmpFile::drawLine(int angle, int distance, int red, int green, int blue)
{
    int height = imageHeader->getHeight()+2;
    int width = imageHeader->getWidth()+2;

    for(int i=0;i<height;i++)
    {
        for (int j=0; j<width;j++)
        {
            int d = abs(j*sin(angle*M_PI/180) + i*cos(angle*M_PI/180));
            if(d==distance)
            {
                data[3*(i*width+j)] = blue;
                data[3*(i*width+j)+1] = green;
                data[3*(i*width+j)+2] = red;
            }
        }
    }
}

void BmpFile::ExportImage(string fileName)
{
    /* combine file header, image header and data in one bmp file */
    ofstream out(fileName+".bmp", ios::binary);
    out.write((char*)fileHeader->getAllHeader(),14);
    out.write((char*)imageHeader->getAllHeader(),40);
    BYTE* pointerOfData = grayImage;
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
    grayImage = new BYTE[imageHeader->getBiSizeImage()/3];
    BYTE *iterator = grayImage;
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
        grayImage[i*imageHeader->getWidth()+x1] = 255;
        grayImage[i*imageHeader->getWidth()+x2] = 255;
    }
    for(int i=x1; i<x2; i++)
    {
        grayImage[y1*imageHeader->getWidth()+i] = 255;
        grayImage[y2*imageHeader->getWidth()+i] = 255;
    }
}

void BmpFile::drawRectColored(int x1, int y1, int x2, int y2, int red , int green, int blue)
{
    // draw rectangle point of (x1,y1) to (x2,y2)
    // coordinates start bottom of the picture
    x1 = 3*x1; y1 = 3*y1;
    x2 = 3*x2; y2 = 3*y2;
    for(int i=y1; i<y2; i+=3)
    {
        data[i*imageHeader->getWidth()+x1] = blue;  // blue first because of little endian
        data[i*imageHeader->getWidth()+x1+1] = green;
        data[i*imageHeader->getWidth()+x1+2] = red;
        data[i*imageHeader->getWidth()+x2] = blue;
        data[i*imageHeader->getWidth()+x2+1] = green;
        data[i*imageHeader->getWidth()+x2+2] = red;
    }
    for(int i=x1; i<x2; i+=3)
    {
        data[y1*imageHeader->getWidth()+i] = blue;
        data[y1*imageHeader->getWidth()+i+1] = green;
        data[y1*imageHeader->getWidth()+i+2] = red;
        data[y2*imageHeader->getWidth()+i] = blue;
        data[y2*imageHeader->getWidth()+i+1] = green;
        data[y2*imageHeader->getWidth()+i+2] = red;
    }
}

void BmpFile::drawCircle(float x, float y, float r)
{
    // draw circle point of center is (x,y) and radius is r
    for(float i=0;i<=2*M_PI;i+=1/r)
    {
        grayImage[(int)(y+(r*sin(i)))*imageHeader->getWidth()+(int)(x+(r*cos(i)))] = 255;
    }
}

void BmpFile::drawEllipse(float x, float y, float i, float j)
{
    // draw ellipse of center (x,y) and i is
    float divider = i;  // divider for step size
    if (i<j) divider=j; // greater radius must be divider
    for(float k=0;k<=2*M_PI;k+=1/divider)
    {
        grayImage[(int)(y+(j*sin(k)))*imageHeader->getWidth()+(int)(x+(i*cos(k)))] = 255;
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
                    sum += mask[i*maskRow+j] * grayImage[(w+i)+(h+j)*imageHeader->getWidth()];
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

void BmpFile::maskApply(BYTE* image, float* buffer, int maskRow, int maskColumn, float *mask)
{
    // move mask on the image and write buffer
    float sum=0;

    for(unsigned int h=0;h<imageHeader->getHeight()-2;h++)
    {
        for(unsigned int w=0;w<imageHeader->getWidth()-2;w++)
        {
            for(int i=0;i<maskColumn;i++)
            {
                for(int j=0;j<maskRow;j++)
                {
                    sum += mask[i*maskRow+j] * image[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth()-2);
            buffer[adres] = sum;
            sum = 0;
        }
    }
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
                buffer[i*width+j] = grayImage[(y1+(i/2-1))*imageHeader->getWidth()+(x1+(j/2-1))];
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

BYTE *BmpFile::getGrayImage() const
{
    return grayImage;
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
        histogram[(unsigned long)grayImage[i]]++;
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
        grayImage[i] = normalized[grayImage[i]];
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
        means.setDataList((int)grayImage[i]);
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
    binaryImage = new BYTE[numberOfPixels];
    for(int i=0;i<numberOfPixels;i++)
        binaryImage[i] = colours[labeledData[i]];
    ExportImage("outputs/kmeans", (BYTE*)binaryImage);
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
    binaryImage = new BYTE[numberOfPixels];
    for(int i=0;i<numberOfPixels;i++)
    {
        binaryImage[i] = colours[labeledData[i]];
    }
    ExportImage("outputs/kmeans2", (BYTE*)binaryImage);
}

void BmpFile::erosion(int maskRow, int maskColumn, float *mask)
{
    int size = (imageHeader->getHeight())*(imageHeader->getWidth());
    BYTE* buffer = new BYTE[size]();
    int sum=1;
    int numberOfOne=0;
    for(int i=0;i<maskRow*maskColumn;i++) {if(mask[i]==1) numberOfOne++;} // count number of 1 in mask
    for(unsigned int h=1;h<imageHeader->getHeight()-1;h++)
    {
        for(unsigned int w=1;w<imageHeader->getWidth()-1;w++)
        {
            for(int i=0;i<maskColumn;i++)
            {
                for(int j=0;j<maskRow;j++)
                {
                    sum += mask[i*maskRow+j] && binaryImage[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth());
            if(sum==numberOfOne)
                buffer[adres] = 255;
            sum = 0;
        }
    }
    ExportImage("outputs/erosion", buffer);
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
                    sum += mask[i*maskRow+j] && binaryImage[(w+i)+(h+j)*imageHeader->getWidth()];
                }
            }
            int adres = (w)+(h)*(imageHeader->getWidth());
            if(sum>1)
                buffer[adres] = 255;
            sum = 0;
        }
    }
    ExportImage("outputs/dilation", buffer);
}

int *BmpFile::labeledObjects()
{
    // return start and end point of all object in array
    // exp: [ob1.start.x, ob1.start.y, ob1.end.x, ob1.end.y, ...]
    // (0,0) point is left-bottom of image
    int imageWidth = imageHeader->getWidth();
    int imageHeight = imageHeader->getHeight();
    BYTE buffer[imageHeight*imageWidth];
    std::memcpy(buffer,binaryImage,sizeof(buffer));
    int nextLabel = 1; // if necessary what label we writing in matrix
    for (int i=0;i<imageHeight*imageWidth;i++)
    {
        if(buffer[i]<250) continue; // if pixel is background pass it
        // if left and bottom pixel was not labeled give a new label it
        else if(i%imageWidth!=0 && buffer[i-1]==0 && buffer[i-imageWidth]==0) buffer[i]=nextLabel++;
        else
        {
            // if left or bottom pixel was labeled give old label it
            if(buffer[i-imageWidth]==0) buffer[i]=buffer[i-1];
            else if(buffer[i-1]==0) buffer[i]=buffer[i-imageWidth];
            else
            {
                buffer[i] = buffer[i-imageWidth];
                continue;
            }
        }
    }


    for (int i=imageHeight*imageWidth;i>-1;i--)
    {
        // check label and fix collision labels
        // horizontally
        if(buffer[i-1]<1 || buffer[i]<1) continue;
        else buffer[i-1] = buffer[i];
    }

    for(int r=1;r<imageWidth;r++)
    {
        for(int h=1;h<imageHeight;h++)
        {
            // check label and fix collision labels
            // vertically
            if(buffer[h*imageWidth+r]==0 || buffer[(h+1)*imageWidth+r]==0) continue;
            buffer[(h+1)*imageWidth+r]=buffer[h*imageWidth+r];
        }
    }

    findCoor(buffer);
    ExportImage("outputs/connected", buffer);
}


void BmpFile::copyDataToBinary()
{
    // copy data to binary and grayscale
    grayScale("outputs/gray");
    int size = imageHeader->getHeight()*imageHeader->getWidth();
    binaryImage = new BYTE[size];
    std::memcpy(binaryImage, grayImage, size);
}

void BmpFile::findCoor(BYTE *buffer)
{
    vector<int> labelName;
    vector<int*> coor;
    int imageSize = imageHeader->getHeight()*imageHeader->getWidth();
    for (int i=0;i<imageSize;i++)
    {
        // get all label name
        if(buffer[i]>0)
        {
            if(labelName.size()==0) labelName.push_back(buffer[i]);
            for(int j=0; j<labelName.size();j++)
            {
                if(labelName.at(j)==buffer[i]) break;
                else if(j==labelName.size()-1) labelName.push_back(buffer[i]);
            }
        }

    }
    std::cout << labelName[0] << endl << labelName[1] << endl << labelName[2] << endl;
    int imageWidth = imageHeader->getWidth();
    int imageHeight = imageHeader->getHeight();
    for(int i=0;i<labelName.size();i++)
    {
        int *temp = new int[4];
        temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;
        for(int h=0;h<imageHeight;h++)
        {
            for(int w=0;w<imageWidth;w++)
            {
                if(buffer[h*imageWidth+w]==labelName.at(i))
                {
                    if(temp[0]==0 || temp[1]==0) {temp[0]=w; temp[1]=h;}
                    if(w<temp[0]) temp[0]=w;
                    if(h<temp[1]) temp[1]=h;
                    if(w>temp[2]) temp[2]=w;
                    if(h>temp[3]) temp[3]=h;
                }
            }
        }
        coor.push_back(temp);
    }

    // delete some noisy point from coor vector
    for(int i=0;i<coor.size();i++)
    {
        int *temp = coor.at(i);
        if((temp[2]-temp[0])*(temp[3]-temp[1])<100)
        {
            coor.erase(coor.begin()+i);
            std::cout << temp[0] << ", " <<temp[1] << " - " << temp[2] << ", " <<temp[3] << endl;
            delete[] temp;
            i=0;
        }
    }

    //calc invariant moments
    float trainingMoments[7] = {0,0,0,0,0,0,0};
//    float circleMomentsAvarage[7] = {0.159318,3.40529e-05,0.00342119,3.44978e-06,-1.08233e-10,-0.00941491,-2.28339e-10};
//    float rectMomentsAvarage[7] = {0.167432,0.000272216,1.77375e-05,1.83106e-06,2.58899e-11,-3.33241e-05,-1.9265e-12};
//    float mercimek[7] = {0.165087,0.00170739,0.00708642,0.000142945,2.59475e-07,-0.00802825,1.58015e-07};
//    float cekirdek[7] = {0.234149,0.0277601,1.0764,0.237477,0.105781,-0.196802,-0.0101849};
//    float eriste[7] = {1.00171,0.997429,22.9338,19.5061,761.183,19.5376,-11.6029};
//    float fasulye[7] = {0.190439,0.0107762,0.0305301,0.00227828,3.78958e-05,0.00298993,2.54525e-05};
//    float misir[7] = {0.164747,0.000834593,0.0931484,0.00139435,2.86284e-06,-0.0279052,5.64801e-06};
//    float nohut[7] = {0.166182,0.00138244,0.156274,0.0031489,0.000338772,-0.0143759,-0.000127367};
//    float para[7] = {0.16244,0.00104876,0.0017776,1.98324e-05,-3.45616e-09,-0.00703107,9.6867e-10};
    float lharf[7] = {0.286828,0.0340665,20.8127,3.40889,7.689,1.25756,-22.2991};
    float daire[7] = {0.159175,7.55916e-08,0.000310305,2.08339e-09,1.28275e-15,-0.00416719,9.78727e-16};
//    float kare[7] = {0.166594,0,8.87177e-15,0,0,0,0};
    float kare[7] = {0.166456,6.36627e-08,0.000167543,1.4734e-05,-3.77652e-09,-0.00299195,7.45479e-10};
//    float ucgen[7] = {0.283339,0.0474304,14.3388,5.63822,71.4114,2.92195,-5.98171};
    float ucgen[7] = {0.288553,0.0498737,16.0789,6.38615,95.8317,2.91965,-7.09319};
    vector<float*> averageObjectMoments;
//    averageObjectMoments.push_back(mercimek);averageObjectMoments.push_back(cekirdek);
//    averageObjectMoments.push_back(eriste);averageObjectMoments.push_back(fasulye);
//    averageObjectMoments.push_back(misir);averageObjectMoments.push_back(nohut);
//    averageObjectMoments.push_back(para);
    averageObjectMoments.push_back(lharf);averageObjectMoments.push_back(daire);
    averageObjectMoments.push_back(kare);averageObjectMoments.push_back(ucgen);
    vector<int*> color;
    int red[3]={0,0,255}; int green[3]={0,255,0}; int blue[3]={255,0,0}; int colorW[3]={255,0,255};
    color.push_back(red);color.push_back(green);color.push_back(blue);color.push_back(colorW);
    for(int object=0;object<coor.size();object++)
    {
        int width = coor.at(object)[2] - coor.at(object)[0];
        int height = coor.at(object)[3] - coor.at(object)[1];
        int arr[width*height];
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                arr[j+i*width] = grayImage[(coor[object][0]+j) + (i+coor[object][1])*imageWidth];
            }
        }
        HuMoments obj1(arr,width,height);
        obj1.calcOrgins();
        obj1.calcInvariantMoments();
        float *moments = obj1.getInvariantMoments();
        for(int e=0; e<7; e++)
        {
            trainingMoments[e] += moments[e];
        }
        // find minumum distance to find object
        float minDistance=0;
        int whichObject=0; // order of object
        for(int obj=0; obj<averageObjectMoments.size();obj++)
        {
            float temp = euclideanDistance(averageObjectMoments.at(obj), moments, 7);
            if(obj==0) {minDistance = temp; whichObject = obj;}
            else if(temp<minDistance) {minDistance = temp; whichObject = obj;}
        }
//        float circleDistance = euclideanDistance(circleMomentsAvarage,moments,7);
//        float rectDistance = euclideanDistance(rectMomentsAvarage,moments,7);
        drawRectColored(coor.at(object)[0],coor.at(object)[1],coor.at(object)[2],coor.at(object)[3],color.at(whichObject)[0],color.at(whichObject)[1],color.at(whichObject)[2]);
        std::cout << "Number Of Object is " << whichObject << endl;
        std::cout << coor[object][0] << " " << coor[object][1] << " " << coor[object][2] << " " << coor[object][3] << endl;
        std::cout << "Momentler " << endl << moments[0] << endl << moments[1] << endl << moments[2] << endl << moments[3]
                << endl << moments[4] << endl << moments[5] << endl << moments[6] << endl;
    }
    std::cout << "Avarage Moments Of All Objects" << endl;
    for(int e=0; e<7; e++)
    {
        trainingMoments[e] = trainingMoments[e]/coor.size();
        std::cout << trainingMoments[e] << endl;
    }

    // Draw Rectangle
    for(int i=0;i<coor.size();i++)
    {
        drawRect(coor.at(i)[0],coor.at(i)[1],coor.at(i)[2],coor.at(i)[3]);
//        std::cout << coor.at(i)[0] << " " << coor.at(i)[1]<< " " <<coor.at(i)[2]<< " " <<coor.at(i)[3]<<endl;
    }
    ExportColoredImage("outputs/rect");
    //ExportImage("outputs/rect");
    std::cout << "Toplam Nesne Sayısı: " << coor.size() << endl;
}

/*
void BmpFile::findCoor(BYTE *buffer)
{
    vector<int> labelName;
    vector<int*> coor;
    int imageSize = imageHeader->getHeight()*imageHeader->getWidth();
    for (int i=0;i<imageSize;i++)
    {
        // get all label name
        if(buffer[i]>0)
        {
            if(labelName.size()==0) labelName.push_back(buffer[i]);
            for(int j=0; j<labelName.size();j++)
            {
                if(labelName.at(j)==buffer[i]) break;
                else if(j==labelName.size()-1) labelName.push_back(buffer[i]);
            }
        }

    }
    std::cout << labelName[0] << endl << labelName[1] << endl << labelName[2] << endl;
    int imageWidth = imageHeader->getWidth();
    int imageHeight = imageHeader->getHeight();
    for(int i=0;i<labelName.size();i++)
    {
        int *temp = new int[4];
        temp[0]=0;temp[1]=0;temp[2]=0;temp[3]=0;
        for(int h=0;h<imageHeight;h++)
        {
            for(int w=0;w<imageWidth;w++)
            {
                if(buffer[h*imageWidth+w]==labelName.at(i))
                {
                    if(temp[0]==0 || temp[1]==0) {temp[0]=w; temp[1]=h;}
                    if(w<temp[0]) temp[0]=w;
                    if(h<temp[1]) temp[1]=h;
                    if(w>temp[2]) temp[2]=w;
                    if(h>temp[3]) temp[3]=h;
                }
            }
        }
        coor.push_back(temp);
    }

    // delete some noisy point from coor vector
    for(int i=0;i<coor.size();i++)
    {
        int *temp = coor.at(i);
        if((temp[2]-temp[0])*(temp[3]-temp[1])<50)
        {
            coor.erase(coor.begin()+i);
            std::cout << temp[0] << ", " <<temp[1] << " - " << temp[2] << ", " <<temp[3] << endl;
            delete[] temp;
            i=0;
        }
    }

    //calc invariant moments
    float trainingMoments[7] = {0,0,0,0,0,0,0};
    float circleMomentsAvarage[7] = {0.159318,3.40529e-05,0.00342119,3.44978e-06,-1.08233e-10,-0.00941491,-2.28339e-10};
    float rectMomentsAvarage[7] = {0.167432,0.000272216,1.77375e-05,1.83106e-06,2.58899e-11,-3.33241e-05,-1.9265e-12};
    float mercimek[7] = {0.165087,0.00170739,0.00708642,0.000142945,2.59475e-07,-0.00802825,1.58015e-07};
    float cekirdek[7] = {0.234149,0.0277601,1.0764,0.237477,0.105781,-0.196802,-0.0101849};
    float eriste[7] = {1.00171,0.997429,22.9338,19.5061,761.183,19.5376,-11.6029};
    float fasulye[7] = {0.190344,0.0107463,0.0290251,0.00217886,3.53235e-05,0.00293058,2.30653e-05};
    float misir[7] = {0.164518,0.000822099,0.0890708,0.00135955,8.1815e-06,-0.0243856,4.06153e-06};
    float nohut[7] = {0.166182,0.00138244,0.156274,0.0031489,0.000338772,-0.0143759,-0.000127367};
    float para[7] = {0.162445,0.00105162,0.0017707,1.98319e-05,-2.80333e-09,-0.00708324,1.32397e-09};
    float lharf[7] = {0.286828,0.0340665,20.8127,3.40889,7.689,1.25756,-22.2991};
    float daire[7] = {0.159175,7.55916e-08,0.000310305,2.08339e-09,1.28275e-15,-0.00416719,9.78727e-16};
//    float kare[7] = {0.166594,0,8.87177e-15,0,0,0,0};
    float kare[7] = {0.166456,6.36627e-08,0.000167543,1.4734e-05,-3.77652e-09,-0.00299195,7.45479e-10};
//    float ucgen[7] = {0.283339,0.0474304,14.3388,5.63822,71.4114,2.92195,-5.98171};
    float ucgen[7] = {0.288553,0.0498737,16.0789,6.38615,95.8317,2.91965,-7.09319};
    vector<float*> averageObjectMoments;
    averageObjectMoments.push_back(mercimek);averageObjectMoments.push_back(cekirdek);
    averageObjectMoments.push_back(eriste);averageObjectMoments.push_back(fasulye);
    averageObjectMoments.push_back(misir);averageObjectMoments.push_back(nohut);
    averageObjectMoments.push_back(para);
//    averageObjectMoments.push_back(lharf);averageObjectMoments.push_back(daire);
//    averageObjectMoments.push_back(kare);averageObjectMoments.push_back(ucgen);
    vector<int*> color;
    int red[3]={0,0,255}; int green[3]={0,255,0}; int blue[3]={255,0,0}; int colorW[3]={255,0,255};
    int sari[3]={255,255,255}; int turuncu[3]={0,123,255}; int mor[3]={177,0,255};
    color.push_back(red);color.push_back(green);color.push_back(blue);color.push_back(colorW);
    color.push_back(sari);color.push_back(turuncu);color.push_back(mor);
    for(int object=0;object<coor.size();object++)
    {
        int width = coor.at(object)[2] - coor.at(object)[0];
        int height = coor.at(object)[3] - coor.at(object)[1];
        int arr[width*height];
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
            {
                arr[j+i*width] = grayImage[(coor[object][0]+j) + (i+coor[object][1])*imageWidth];
            }
        }
        HuMoments obj1(arr,width,height);
        obj1.calcOrgins();
        obj1.calcInvariantMoments();
        float *moments = obj1.getInvariantMoments();
        for(int e=0; e<7; e++)
        {
            trainingMoments[e] += moments[e];
        }
        // find minumum distance to find object
        float minDistance=0;
        int whichObject=0; // order of object
        for(int obj=0; obj<averageObjectMoments.size();obj++)
        {
            float temp = euclideanDistance(averageObjectMoments.at(obj), moments, 7);
            if(obj==0) {minDistance = temp; whichObject = obj;}
            else if(temp<minDistance) {minDistance = temp; whichObject = obj;}
        }
//        float circleDistance = euclideanDistance(circleMomentsAvarage,moments,7);
//        float rectDistance = euclideanDistance(rectMomentsAvarage,moments,7);
        drawRectColored(coor.at(object)[0],coor.at(object)[1],coor.at(object)[2],coor.at(object)[3],color.at(whichObject)[0],color.at(whichObject)[1],color.at(whichObject)[2]);
        std::cout << "Number Of Object is " << whichObject << endl;
        std::cout << coor[object][0] << " " << coor[object][1] << " " << coor[object][2] << " " << coor[object][3] << endl;
        std::cout << "Momentler " << endl << moments[0] << endl << moments[1] << endl << moments[2] << endl << moments[3]
                << endl << moments[4] << endl << moments[5] << endl << moments[6] << endl;
    }
    std::cout << "Avarage Moments Of All Objects" << endl;
    for(int e=0; e<7; e++)
    {
        trainingMoments[e] = trainingMoments[e]/coor.size();
        std::cout << trainingMoments[e] << endl;
    }

    // Draw Rectangle
    for(int i=0;i<coor.size();i++)
    {
        drawRect(coor.at(i)[0],coor.at(i)[1],coor.at(i)[2],coor.at(i)[3]);
//        std::cout << coor.at(i)[0] << " " << coor.at(i)[1]<< " " <<coor.at(i)[2]<< " " <<coor.at(i)[3]<<endl;
    }
    ExportColoredImage("outputs/rect");
//    ExportImage("outputs/rect");
    std::cout << "Toplam Nesne Sayısı: " << coor.size() << endl;
}
*/
