#ifndef __COMPLEX__
#define __COMPLEX__

#include <iostream>
using namespace std;

template <class T> class complex;

template <class T> 
complex<T> operator*(const complex<T> & A, const complex<T> & B);
template <class T>
complex<T>& __doapl (complex<T>*,
                            const complex<T>&);

template <class T>
class complex
{
public:
    complex(T r = 0, T i =0)
    : re(r), im(i)
    {}
    complex& operator += (const complex&); //声明

    friend complex<T> operator*(const complex<T> & A, const complex<T> & B);

    T real() const {return re;/*return  this->re*/} 
    T imag() const {return im;}
    T show() const {
        //cout<<real()<<' '<<imag();
        //cout<<"hello"<<endl;
    }

private:
    T re, im;

    friend complex<T>& __doapl (complex<T>*,
                            const complex<T>&);

};

void testComplex();
#endif
