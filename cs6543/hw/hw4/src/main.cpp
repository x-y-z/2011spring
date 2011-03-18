#include <iostream>
#include "udp.h"

int main()
{
    char buf[100];
    

//    udp aCon(1);

//    aCon.init(3333);

//    aCon.recv(buf, 100);

//    std::cout<<buf<<std::endl;

    udp aCon(-1);

    aCon.init("localhost", 3333);
    aCon.send("hello", 5);

    return 0;
}
