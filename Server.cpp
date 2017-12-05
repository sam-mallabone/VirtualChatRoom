#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <thread>
#include "socket.h"
#include <vector>
#include "Semaphore.h"
#include <algorithm>

using namespace Sync;


void TFunction(Socket sock, int port, std::vector<Socket> &clientSockets) {
    //first get a reference to the semaphore
    //need the string of the port number
    std::string portstr = std::to_string(port);
    Semaphore protect(portstr);
    //passed to this function is a socket and an array of sockets by reference
    try{
        std::cout << "A thread was connected on port "<< port << std::endl;
        ByteArray ba("Will be removed");
        int j = sock.Read(ba);
        std::string myname = ba.ToString();
        while(true) {
            ByteArray receivedBa("Will be removed");
            sock.Read(receivedBa);
            std::string recStr = receivedBa.ToString();
            std::cout << "Messaged received: " << recStr << "on port "<< port << std::endl;
            if(recStr == "shutdown\n") {
                //get access to semaphore
                protect.Wait();
                //remove this socket from the client sockets vector
                clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), sock), clientSockets.end());
                //release the semaphore
                protect.Signal();
                //break out of the loop
                std::cout<< "notice to shutdown" << std::endl;
                break;
            }
            //get the semaphore so thread can go into critical area
            protect.Wait();
            for(int i = 0; i < clientSockets.size(); i++) {
                ByteArray sendBa(recStr);
                clientSockets[i].Write(sendBa);
            }
            protect.Signal();
            //Release semaphore here so other process can get it
        }
    } 
    catch(std::string &s) {
        std::cout << s << std::endl;
    }
    catch(std::exception &e){
        std:: cout<< e.what() << std::endl;
    }
    std::cout<<"exiting a thread on port " << port << std::endl;

}

void SFunction(int port) {
    std::cout << "Now creating on port " << port << std::endl;
    std::vector<Socket> clientSockets;
    //create a semaphore for this server
    //every server's semaphore is going to be named their port number since it guarunteed to be unique
    std::string portstr = std::to_string(port);
    Semaphore protect(portstr, 1, true);
    try {
        SocketServer sockser(port);
        while(true){
            Socket sock = sockser.Accept();
            //get access to the semaphore to enter the critical area
            protect.Wait();
            clientSockets.push_back(sock);
            protect.Signal();
            std::thread t(TFunction, sock, port, std::ref(clientSockets)); //should i protect this too?
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
    char decision = 'j';
    while(true) {
        std::thread t(SFunction, port);
        t.detach();
        usleep(500000);
        std::cout << "Would you like to create another Server Thread? No will shut down everything(y/n)" << std::endl;
        //char decision = 'j';
        std::cin >> decision;
        if(decision == 'n'){
            //this case is for the no command being chosen
            std::cout << "Shutting down now....";
            break;
            //I need to do alot of clean up right here!
        }
        else {
            port++;
        }
    }
}