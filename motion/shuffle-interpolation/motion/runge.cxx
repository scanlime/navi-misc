#include <stdlib.h>
#include <iostream>
#include "vect.h"
#include "runge.h"
#include "function.h"

double t;
vect k1(const vect &x, double h)
{
    return f(x,t);
}
    
vect k2(const vect &x, double h, const vect &k1_)
{
    return f(x + (h/2)*k1_,t+(h/2));
}

vect k3(const vect &x, double h, const vect &k2_)
{
    return f(x + (h/2)*k2_,t+(h/2));
}

vect k4(const vect &x, double h, const vect &k3_)
{
    return f(x + h*k3_,t+h);
}

vect runge(const vect &x, double h,double time)
{
    t = time;
    
    vect k1_(x.size()),k2_(x.size()),k3_(x.size()),k4_(x.size());
 
    k1_=k1(x,h);
    k2_=k2(x,h,k1_);
    k3_=k3(x,h,k2_);
    k4_=k4(x,h,k3_);
    vect dx(x.size());
    dx=((x + (h/6)*(k1_ + (2.0*k2_) + (2.0*k3_) + k4_)));
    return dx;
}
