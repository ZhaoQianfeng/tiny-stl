/* test zvector
*/
#include "zvector.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
using namespace zstd;

class Dog
{
private:
    int year;
    string name;
public:
    Dog(){ cout << "default ctor" << endl;}

    Dog(int year, string name):year(year),name(name)
    {
        cout << "constructor" << endl;
    }

    int get_year() const
    {
        return year;
    }

    string get_name() const
    {
        return name;
    }

    ~Dog()
    {
        cout << "deconstructor " << name << endl;
    }
};


int main()
{

    
    zvector<zvector<int>> a(10, zvector<int>(5,10086)) ;

    cout<< a[0][0] <<endl;
    a[0][0]=1;
    cout<< a[0][0] <<endl;
    cout<< a[1][0] <<endl;
    cout<< a[0][1] <<endl;
    cout<< a[1][1] <<endl;

    zvector<Dog> b (10, Dog());
    cout << b[1].get_name() << endl;
    
}