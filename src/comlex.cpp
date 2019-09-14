#include "complex.h"

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
    os<<"complex: "<<'('<<x.real()<<','<<x.imag()<<')';
    return os;
}
complex operator*(const complex & A, const complex & B)
{
    complex c;
    c.re = A.re * B.re;
    c.im = A.im * B.im;
    return c;
}


void testComplex()
{
    complex c1(9, 8);
    complex c2(10, 11);
    c2+=c1;
    cout<<c2<<endl;
    complex c3 = c2*c1;
    cout<<c3<<endl;
    //c1 << cout;
    //cout << c1 <<endl;
    //c2.show();
    //complex c3=c1+c2;
   // c3.show();


}