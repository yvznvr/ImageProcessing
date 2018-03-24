#ifndef MULTIDIMENSIONALARRAY_H
#define MULTIDIMENSIONALARRAY_H

#include <math.h>

class MultiDimensionalArray
{
public:
    MultiDimensionalArray();
    MultiDimensionalArray(MultiDimensionalArray *);
    MultiDimensionalArray(int dimension);
    float operator-(const MultiDimensionalArray);
    MultiDimensionalArray operator +(const MultiDimensionalArray);
    MultiDimensionalArray operator /(const int);
    MultiDimensionalArray operator =(const MultiDimensionalArray);
    bool operator !=(const MultiDimensionalArray);
    void append(int *);
    void convertInt(int*);
    float *array;
    int getDimension() const;
    void setDimension(int value);

private:
    int dimension;

};

#endif // MULTIDIMENSIONALARRAY_H
