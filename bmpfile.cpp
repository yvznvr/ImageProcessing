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
        if(pixelNumber>=imageHeader->getWidth()*3)
        {
            BYTE pad = 0;
            for(int i=0;i<padding;i++) out.write((char*)&pad,1);
            pixelNumber = 0; i+=padding;
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
    float derivateOfY[9] = {1,0,-1,2,0,-2,1,0,-1};
    float derivateOfX[9] = {1,2,1,0,0,0,-1,-2,-1};
    float imageX[size], imageY[size];
    int angle[size];
    float realAngle[size], thisAngle;
    grayScale("outputs/output");
    maskApply(grayImage, imageY, 3, 3, derivateOfY);
    maskApply(grayImage, imageX, 3, 3, derivateOfX);
    int max=0, min=1020;

    padding = 0;
    while(((imageHeader->getWidth()-2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()-2));
    imageHeader->setWidth((imageHeader->getWidth()-2));

    // gradiant angle and magnitude
    for(int i=0; i<size; i++)
    {
        image[i] = sqrt(pow(imageX[i], 2) + pow(imageY[i], 2));
        thisAngle = (atan2(imageY[i], imageX[i])/M_PI) * 180.0;
        realAngle[i] = thisAngle;

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

    ExportImage("outputs/x", normalize(imageX, size));
    ExportImage("outputs/y", normalize(imageY, size));
    ExportImage("outputs/magnitude", normalize(image, size));

    // non-max suppression
    int directions[8] = {1,0,1,1,0,1,-1,1};
    for(int h=1;h<height-1;h++)
    {
        for(int r=1;r<width-1;r++)
        {
            int ang = angle[h*width+r];
            if(image[h*width+r] == 0) continue;
            else if((image[h*width+r]>image[(h+directions[2*ang+1])*width+r+directions[2*ang]]) &&
                    (image[h*width+r]>image[(h-directions[2*ang+1])*width+r-directions[2*ang]]))
                continue;
            else image[h*width+r] = 0;
        }
    }

    ExportImage("outputs/nonmax", normalize(image, size));

    // Hysteresis Thresholding

    int hysteresis = max*0.65;

    for (int i=0;i<size;i++)
    {
        if(image[i]>max-hysteresis) image[i] = max;
        else if(image[i]<hysteresis) image[i] = 0;
    }

    ExportImage("outputs/hysteresis", normalize(image, size));

    for(int h=0;h<height-1;h++)
    {
        for(int r=0;r<width-1;r++)
        {
            int ang = (2 + angle[h*width+r])% 4;

            if(image[h*width+r]==max || image[h*width+r]==0) continue;
            else if(image[(h+directions[2*ang+1])*width+r+directions[2*ang]]==max)
               image[h*width+r] = max;
            else image[h*width+r] = 0;
        }
    }

    // normalize
    BYTE im[size];
    for (int i=0; i<size; i++)
        im[i] = image[i]*255/max;

    ExportImage("outputs/canny", im);
    //houghTransform(angle, im);
    houghCircle(realAngle, im);
}
void BmpFile::houghCircle(float *ang, BYTE *image)
{
    const int height = imageHeader->getHeight();
    const int width = imageHeader->getWidth();
    const int maxLength = 300;
    //const int maxLength = sqrt(pow(height,2)+pow(width,2));

    int ***houghArray = new int**[width];
    for(int i=0;i<width;++i)
    {
        houghArray[i]=new int*[height];

        for(int k=0;k<height;++k)
            houghArray[i][k]=new int[maxLength];
    }

    for (int r=0; r<maxLength; r++)
    {
        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                houghArray[x][y][r] = 0;
            }
        }
    }


    for (int r=0; r<maxLength; r++)
    {
        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                if (image[y*width+x] == 255)
                {
                    int x0 = x - r * cos(ang[y*width+x]*M_PI/180);
                    int y0 = y - r * sin(ang[y*width+x]*M_PI/180);
                    if(x0<1 || y0<1) continue;
                    houghArray[x0][y0][r] = houghArray[x0][y0][r] + 1;
                }
            }
        }
    }

    padding = 0;
    while(((imageHeader->getWidth()+2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()+2));
    imageHeader->setWidth((imageHeader->getWidth()+2));

    vector<int*> circles;
    for (int r=0; r<maxLength; r++)
    {
        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                int dist = houghArray[x][y][r];
                if(dist>10)
                {                    
//                    drawCircle(x,y,r);
                    int *circle = new int[4];
                    circle[0] = x; circle[1] = y; circle[2] = r; circle[3] = dist;
                    if(circles.size()==0) circles.push_back(circle);
                    else
                    {
                        unsigned int counter = 0;
                        for(unsigned int i=0;i<circles.size();i++)
                        {
                            if(sqrt(pow(circles.at(i)[0]-x, 2) + pow(circles.at(i)[1]-y, 2)) > r+circles.at(i)[2])
                            {
                                // objects does not collide
                                counter++;
                                continue;
                            }
                            if(circles.at(i)[3]>dist) break;
                            else
                            {
                                circles.erase(circles.begin()+i);
                                circles.push_back(circle);
                                break;
                            }
                        }
                        if(counter==circles.size()) circles.push_back(circle);
                    }
                }
            }
        }
    }
    delete[] houghArray;


    #include <fstream>
//    ofstream outfile;
//    outfile.open("data");
    ifstream inpfile;
    inpfile.open("normaldata");
    vector<int*> histograms;
    string s;
    getline(inpfile,s);
    int l = stoi(s.c_str());
    for (int i=0;i<l;i++)
    {
        int *arr = new int[59];
        for(int j=0;j<59;j++)
        {
            string value;
            getline(inpfile,value);
            arr[j] = stoi(value.c_str());
        }
        histograms.push_back(arr);
    }


    for(unsigned int i=0;i<circles.size();i++)
    {
        int *temp = circles.at(i);
        drawCircle(temp[0], temp[1], temp[2]);
        cout<< temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << endl;
        int histogram[59];
        rlbp(temp, histogram);
//        for(int j=0;j<59;j++)
//        {
//            // save values
//            outfile << histogram[j] << endl;
//        }

        for(int j=0;j<l;j++)
        {
            float diss = 99999999;
            for(int s=0;s<59;s++)
            {
                int temp = dissimilarity(histogram,histograms.at(j),59, 0);
                if (temp<diss) diss = temp;
            }
            cout << "Fark: " << diss << endl;
        }
        delete temp;
    }
//    outfile.close();

    ExportColoredImage("outputs/hough");
}

void BmpFile::houghTransform(int *ang, BYTE *image)
{
    int height = imageHeader->getHeight();
    int width = imageHeader->getWidth();
    int maxLength = sqrt(pow(height,2)+pow(width,2));

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
                int d = abs(r*sin(tempAngle*M_PI/180) + c*cos(tempAngle*M_PI/180));
                houghArray[tempAngle+360*d] = houghArray[tempAngle+360*d] + 1;
            }
        }
    }

    for (int r=0; r<maxLength; r++)
    {
        for (int c=0; c<360; c++)
        {
            int dist = houghArray[r*360+c];
            if((dist>130) && c==0) drawLine(90, r+1, 255,0,0);
            else if((dist>130) && c==90) drawLine(0, r+1, 0,0,255);
        }
    }

    padding = 0;
    while(((imageHeader->getWidth()+2)*3+padding)%4 != 0)   padding++;
    imageHeader->setHeight((imageHeader->getHeight()+2));
    imageHeader->setWidth((imageHeader->getWidth()+2));
    ExportColoredImage("outputs/hough");

    BYTE *hoArray = new BYTE[maxLength*360];
    for (int r=0; r<maxLength; r++)
    {
        for (int c=0; c<360; c++)
        {
            if (houghArray[r*360+c]>50) hoArray[r*360+c] = 255;
        }
    }

    imageHeader->setHeight(360);;
    imageHeader->setWidth(maxLength);
    padding = 0;
    while(((imageHeader->getWidth())*3+padding)%4 != 0)   padding++;
    ExportImage("outputs/houghspace",hoArray);

    delete[] houghArray;
    delete[] hoArray;
}

void BmpFile::drawLine(int angle, int distance, int red, int green, int blue)
{
    // Draw line axis of x and y
    int height = imageHeader->getHeight()+2;
    int width = imageHeader->getWidth()+2;

    if(angle==0)
    {
        for(int i=0;i<width;i++)
        {
            data[3*(distance*width+i)] = blue;
            data[3*(distance*width+i)+1] = green;
            data[3*(distance*width+i)+2] = red;
        }
    }
    else
    {
        for(int i=0;i<height;i++)
        {
            data[3*(i*width+distance)] = blue;
            data[3*(i*width+distance)+1] = green;
            data[3*(i*width+distance)+2] = red;
        }
    }
}

BYTE *BmpFile::normalize(float *array,int size)
{
    // normalize and convert float array to byte array
    int max=0;
    for (int i=0;i<size;i++)
        if(array[i]>max) max=array[i];

    BYTE *im = new BYTE[size];
    for (int i=0; i<size; i++)
        im[i] = array[i]*255/max;

    return im;

}

void BmpFile::rlbp(int * shape, int *histogram)
{
    // rotate inverse local binary pattern algorithm

    const uint8_t indexs[59] = {0, 1, 2, 3, 7, 8, 15, 16, 24, 31, 32,
                            48, 56, 63, 64, 65, 67, 96, 97, 99, 112,
                            113, 115, 120, 121, 123, 124, 126, 127,
                            128, 129, 130, 131, 135, 136, 143, 144,
                            152, 159, 160, 176, 184, 191, 192, 193,
                            195, 224, 225, 227, 240, 241, 243, 248,
                            249, 251, 252, 254, 255, 4}; //4 means 59th index


    auto index = [](auto *indexs, auto number){
        for(int i=0;i<59;i++)
        {
            if(indexs[i]==number) return i;
        }
        return 58;
    };

    for (int i=0;i<59;i++) histogram[i] = 0;

    int x = shape[0];
    int y = shape[1];
    float r = shape[2];

    // following values store points of rectangle that circle be in
    unsigned int x1 = x - r;
    unsigned int y1 = y - r;
    unsigned int x2 = x + r;
    unsigned int y2 = y + r;

    // directions of values
    // {x1,y1,x2,y2....x8,y8}
    int directions[16] = {-1,-1,-1,0,-1,1,0,1,1,1,1,0,1,-1,0,-1};
    for(unsigned int i=x1;i<x2;i++)
    {
        for(unsigned int j=y1;j<y2;j++)
        {
            if(sqrt(pow((i - x),2) + pow((j - y),2)) > r) continue;
            int sum = 0;
            for(int k=0;k<8;k++)
            {
                if(grayImage[i+directions[2*k]+(j+directions[2*k+1])*imageHeader->getWidth()] <
                        grayImage[i+j*imageHeader->getWidth()]) continue;
                sum += pow(2,k);
            }
            histogram[index(indexs, sum)]++;
        }
    }
}

float BmpFile::dissimilarity(int *h1, int *h2, int size, int shift)
{
    // dissimilarity metric with chi-square method
    float dissimilarity = 0;
    for(int i=0;i<size;i++)
    {
        float temp = pow((h1[i]-h2[(i+shift)%size]),2)/(h1[i]+h2[(i+shift)%size]);
        if(isnan(temp)) continue;
        dissimilarity += temp;
    }
    return dissimilarity;
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
    if(x-r<1 || y-r<1) return;
    for(float i=1/r;i<=2*M_PI;i+=1/r)
    {
        data[(int)(y+(r*sin(i)))*imageHeader->getWidth()*3+(int)(x+(r*cos(i)))*3] = 255;
        data[(int)(y+(r*sin(i)))*imageHeader->getWidth()*3+(int)(x+(r*cos(i)))*3+1] = 255;
        data[(int)(y+(r*sin(i)))*imageHeader->getWidth()*3+(int)(x+(r*cos(i)))*3+2] = 255;
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
            buffer[adres] = abs(sum);
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

    // normalize histogram data
    for (int i=0;i<256;i++)
        normalized[i] = normalized[i]/(float)numberOfPixels*255;

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
