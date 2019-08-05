#ifndef __COMPLEX__
#define __COMPLEX__

#include <iostream>
using namespace std;

class complex
{
public:
    complex(double r = 0, double i =0)
    : re(r), im(i)
    {}
    complex& operator += (const complex&); //声明
    double real() const {return re;} 
    double imag() const {return im;}
    double show() const {
        //cout<<real()<<' '<<imag();
        //cout<<"hello"<<endl;
    }



private:
    double re, im;

    friend complex& __doap1 (complex*,
                            const complex&);

};

inline  complex&
__doap1(complex* ths, const complex& r)
{
    ths->re += r.re;
    ths->im += r.im;
    return *ths;
}

inline  complex&
complex::operator += (const complex& r)
{
    return __doap1(this, r);
}

inline complex
operator + (const complex& x, const complex & y)
{
   //   return complex( x.re + y.re,
  //                x.im + y.im);
    return complex(x.real() + y.real(),
                    x.imag() + y.imag());
}

inline complex
operator + (const complex& x, double y)
{
    return complex(x.real() + y, x.imag());
}

inline complex
operator + (double x, const complex& y)
{
    return complex(x + y.real(), y.imag());
}

inline ostream&
operator << (ostream& os, const complex& x)
{
  //  os<<'('<<x.real()<<','<<x.imag()<<')';

}

void testComplex();
#endif