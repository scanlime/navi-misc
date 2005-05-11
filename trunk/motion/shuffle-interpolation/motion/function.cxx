#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "vect.h"

vect f(const vect &x,double t)
{

    vect dx(x.size());
    // 0 - x
    // 1 - y
    // 2 - z
    double a,r,b;
    a = 16.0;
    r = 45.0;
    b = 4.0;

    dx.assign(a*(x.value(1)-x.value(0)),0);
    dx.assign(r*x.value(0) - x.value(1) - x.value(0)*x.value(2),1);
    dx.assign(x.value(0)*x.value(1) - b*x.value(2),2);
    
    return dx;
}
