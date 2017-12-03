#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <thread>
#include "socket.h"
#include <vector>

using namespace Sync;


void TFunction(Socket sock, std::vector<Socket> &clientSockets) {
    //passed to this function is a socket and an array of sockets by reference
    try{
        ByteArray ba("Will be removed");
        int j = sock.Read(ba);
        std::string myname = ba.ToString();
        while(true) {
            ByteArray receivedBa("Will be removed");
            sock.Read(receivedBa);
            std::string recStr = receivedBa.ToString();
            std::cout << "Messaged received: " << recStr << std::endl;
            //Add semaphore wait here java 75
            for(int i = 0; i < clientSockets.size(); i++) {
                //loop through and send messages to all the connected sockets
                ByteArray sendBa(recStr);
                clientSockets[i].Write(sendBa);
            }
            //Release semaphore here
        }
    } 
    catch(std::string &s) {
        std::cout << s << std::endl;
    }

}

void SFunction(int port) {
    std::cout << "Now creating on port " << port << std::endl;
    int j = 0;
    std::cin >> j;
    std::vector<Socket> clientSockets;
    try {
        SocketServer sockser(port);
        while(true){
            Socket sock = sockser.Accept();
            clientSockets.push_back(sock);
            std::thread t(TFunction, sock, std::ref(clientSockets));
            t.detach();
        }
    } 
    catch(std::string &s) {
        std::cout << s << std::endl;
    }
}

int main(void) {
    int port = 2020;
    int c = 0;
    std::cout << "I consider myself to the be the driver of the program" << std::endl;
    std::cout << "The Original server will be created on port " << port << std::endl;
    std::cin >> c;
    while(true) {
        //std::thread t(SFunction, port);
        SocketServer ser(2020);
        Socket sock = ser.Accept();
        std:: cout << "I got here" << std::endl;
        //t.detach();
        std::cout << "Would you like to create another Server Thread? No will shut down everything(y/n)" << std::endl;
        char decision = 'j';
        std::cin >> decision;
        if(decision == 'n'){
            //this case is for the no command being chosen
            std::cout << "Shutting down now....";
            break;
        }
        else {
            port++;

        }
    }
    // SocketServer sockser(2000);
    // Socket sock = sockser.Accept();
    // std::cout << "I have a connection" << std::endl;
    // ByteArray ba("Will be removed");
    // while(true) {
    //     try {
            
    //         sock.Read(ba);
    //         std::string str = ba.ToString();
    //         std::cout << "This is what I got " << str << std::endl;
    //         std::string returnstr= "X" + str;
    //         ByteArray returnba(returnstr);
    //         std::string anotha = returnba.ToString();
    //         std::cout << anotha << std::endl;
    //         int k = sock.Write(returnba);
    //     }
    //     catch(std:: string &s){
    //         std::cout << s << std::endl;
    //     }
    // }
}