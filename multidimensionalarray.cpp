#include "multidimensionalarray.h"

MultiDimensionalArray::MultiDimensionalArray()
{
    this->dimension = 3;
    array = new float[3];
    for(int i=0;i<3;i++) array[i] = 0;
}

MultiDimensionalArray::MultiDimensionalArray(MultiDimensionalArray * other)
{
    this->dimension = other->getDimension();
    for(int i=0;i<dimension;i++)
        array[i] = other->array[i];
}

MultiDimensionalArray::MultiDimensionalArray(int dimension)
{
    this->dimension = dimension;
    array = new float[dimension];
}

float MultiDimensionalArray::operator-(const MultiDimensionalArray other)
{
    float temp=0;
    for(int i=0;i<dimension;i++)
    {
        temp += pow((this->array[i] - other.array[i]),2);
    }
    return sqrt(temp);
}

MultiDimensionalArray MultiDimensionalArray::operator +(const MultiDimensionalArray other)
{
    MultiDimensionalArray temp(this->dimension);
    int data[this->dimension];
    for(int i=0;i<this->dimension;i++)
    {
            data[i] = this->array[i] + other.array[i];
    }
    temp.append(data);
    return temp;
}

MultiDimensionalArray MultiDimensionalArray::operator /(const int divisor)
{
    MultiDimensionalArray temp(this->dimension);
    int data[this->dimension];
    for(int i=0;i<this->dimension;i++)
    {
        data[i] = this->array[i] / divisor;
    }
    temp.append(data);
    return temp;
}

MultiDimensionalArray MultiDimensionalArray::operator =(const MultiDimensionalArray other)
{
    this->dimension = other.getDimension();
    for(int i=0;i<this->dimension;i++) this->array[i] = other.array[i];
}

bool MultiDimensionalArray::operator !=(const MultiDimensionalArray other)
{
    for (int i=0;i<this->dimension;i++)
    {
        if(this->array[i]!=other.array[i]) return true;
    }
    return false;
}

void MultiDimensionalArray::append(int *data)
{
    for(int i=0;i<dimension;i++) array[i] = data[i];
}

void MultiDimensionalArray::convertInt(int *arr)
{
    for(int i=0;i<this->dimension;i++)
        arr[i] = array[i];
}

int MultiDimensionalArray::getDimension() const
{
    return dimension;
}

void MultiDimensionalArray::setDimension(int value)
{
    dimension = value;
}
