#ifndef VECT_H
#define VECT_H


#include <stdlib.h>
class vect
{
public:
    vect();
    vect(int size);
    

    // constant member functions
    int size() const;
    double value(int loc) const;
    void print();
    double mag();  // euclidean length

    // modification member functions
    void assign(double val,int loc);
    vect& operator =(const vect& source);
    
private:
    double *x;
    int dim;
};

// helper functions
vect operator +(const vect &a, const vect &b);
vect operator -(const vect &a, const vect &b);
vect operator *(const double &c, const vect &c);


#endif

