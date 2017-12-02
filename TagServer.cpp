#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <thread>
#include "socket.h"

using namespace Sync;

int main(void) {
    std::cout << "I am running and am the server" << std::endl;
    SocketServer sockser(2000);
    Socket sock = sockser.Accept();
    ByteArray ba("Will be removed");
    while(running) {
        try {
            

            sock.Read(ba);
            std::string str = ba.ToString();
            std::cout << "This is what I got " << str << std::endl;
            std::string returnstr= "X" + str;
            ByteArray returnba(returnstr);
            int k = sock.Write(returnba);
        }
        catch(std:: string &s){
            std::cout << s << std::endl;
        }
    }
}