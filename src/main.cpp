#include <iostream>
#include "ServerSide/WhatsappServer.h"

const int PORT_NUM = 8875;

int main() {
    std::cout << "Hello, World!" << std::endl;
    WhatsappServer * server = new WhatsappServer();
    server->activateServer(PORT_NUM);
    return 0;
}