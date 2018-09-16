#include <iostream>
#include <thread>
#include "tcplistener.h"

using namespace std;

int main()
{
    TCPListener tst;
//    thread t1(&TCPListener::acceptData, &tst);
    tst.acceptData();
//    t1.join();
    return 0;
}
