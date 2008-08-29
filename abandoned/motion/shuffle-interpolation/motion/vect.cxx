#include <iostream>
#include <math.h>
#include <assert.h>
#include <climits>
#include "vect.h"

vect::vect()
{
}

vect::vect(int size)
{
    x = new double [size];
    dim = size;
    for(int i = 0; i < size; i++)
        x[i] = -1;
}


    // constant member functions
int vect::size() const
{
    return dim;
}

double vect::value(int loc) const
{
    return x[loc];
}

void vect::print()
{
	std::cout << x[0] << "\t";
	std::cout << x[1] << "\t";
	std::cout << x[2] << std::endl;
}

double vect::mag() // euclidean length
{
    double sqsum = 0;
    for(int i = 0; i < dim; i++)
        sqsum += x[i]*x[i];
    return sqrt(sqsum);
}

// modification member functions
void vect::assign(double val,int loc)
{
    x[loc] = val;
}

vect& vect::operator =(const vect& source)
{
    assert(dim == source.dim);
    if(this == &source) return *this;

    for(unsigned int i=0; i<source.dim;i++)
    {
        x[i] = source.x[i];
    }
    return *this;
}

// helper functions
vect operator +(const vect &a, const vect &b)
{
    assert(a.size()==b.size());
    vect sum(a.size());
    for(int i = 0; i < a.size(); i++)
        sum.assign(a.value(i)+b.value(i),i);
    return sum;
}

vect operator -(const vect &a, const vect &b)
{
    assert(a.size()==b.size());
    vect diff(a.size());
    for(int i = 0; i< a.size(); i++)
        diff.assign(a.value(i)-b.value(i),i);
    return diff;
}

vect operator *(const double &c, const vect &b)
{
    assert(b.size()>0);
    vect prod(b.size());
    for(int i = 0; i < b.size(); i++)
        prod.assign(c*b.value(i),i);
    return prod;
}



