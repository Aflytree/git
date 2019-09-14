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
    friend complex operator*(const complex & A, const complex & B);

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

void testComplex();
#endif