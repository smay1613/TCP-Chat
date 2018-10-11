#include <iostream>
#include <thread>
#include "tcplistener.h"

using namespace std;

int main()
{
    TCPListener mainListener;
    mainListener.acceptData();

    return 0;
}
