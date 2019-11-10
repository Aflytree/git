#ifndef __MY_STRING__
#define __MY_STRING__

#include <iostream>
using namespace std;

class myString
{
public:
    myString(const char * cstr = 0);
    myString(const myString& str);
    myString& operator=(const myString& str);
    ~myString();
    char* ge_c_str() const {return m_data;}
private:
    char* m_data;
};

#endif