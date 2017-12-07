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


class SocketThread : public Thread
{
private:
    // Reference to our connected socket
    Socket& socket;

    // A byte array for the data we are receiving and sending
    ByteArray data;

    //boolean to store termination
    bool& terminate;

    //store port number
    int port;

    //reference to vector of sockets
    std::vector<Socket> clientSockets;
public:
    SocketThread(Socket& socket, bool& terminate, int port, std::vector<Socket> &clientSockets)
    : socket(socket), terminate(terminate), port(port), clientSockets(clientSockets)
    {}

    ~SocketThread()
    {}

    Socket& GetSocket()
    {
        return socket;
    }

    virtual long ThreadMain()
    {
    //first get a reference to the semaphore
    //need the string of the port number
    std::string portstr = std::to_string(port);
    Semaphore protect(portstr);
    //passed to this function is a socket and an array of sockets by reference
    try{
        std::cout << "A thread was connected on port "<< port << std::endl;
        socket.Read(data);
        std::string myname = data.ToString();
        while(!terminate) {
            int j = socket.Read(data);
            if(j == 0){
                break;
            }
            std::cout << "J: " << j << std::endl;
            std::string recStr = data.ToString();
            std::cout << "Messaged received: " << recStr << "on port "<< port << std::endl;
            if(recStr == "shutdown\n") {
                //get access to semaphore
                protect.Wait();
                //remove this socket from the client sockets vector
                clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), socket), clientSockets.end());
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
};

// This thread handles the server operations
class ServerThread : public Thread
{
private:
    SocketServer& server;
    std::vector<Socket> clientSockets;
    std::vector<SocketThread*> socketThreads;
    bool terminate = false;
    int port;
public:
    ServerThread(SocketServer& server, int port)
    : server(server), port(port)
    {}

    ~ServerThread()
    {
        // Close the client sockets
        for (auto thread : socketThreads)
        {
            try
            {
                // Close the socket
                Socket& toClose = thread->GetSocket();
                toClose.Close();
            }
            catch (...)
            {
                //This will catch all exceptions
            }
        }

        // Setting this will allow the thread loops to terminate
        terminate = true;
    }

    virtual long ThreadMain()
    {
        while(true)
        {
            try
            {
                std::string portstr = std::to_string(port);
                Semaphore protect(portstr, 1, true);
                // Wait for a client socket connection
                std::cout<<"I get here in server" << std::endl;
                Socket sock = server.Accept();
                Socket* newConnection = new Socket(sock);

                // Pass a reference to this pointer into a new socket thread
                Socket& socketReference = *newConnection;
                clientSockets.push_back(sock);
                socketThreads.push_back(new SocketThread(socketReference, terminate, port, std::ref(clientSockets)));
            }
            catch (TerminationException terminationException)
            {
                std::cout << "Shutting down now" << std::endl;
                return terminationException;
            }
            catch (std::string error)
            {
                std::cout << std::endl << "[Error] " << error << std::endl;
                return 1;
            }
        }
    }
};

int main(void) {
    int port = 2020;
    int numberOfRooms = 7;
    std::cout << "I am the Server" << std::endl;
    std::cout << "Please type done to end the Server" << std::endl;
    //std::cout.flush();
    std::vector<SocketServer> serverSockets;
    // for(int i = 0; i < numberOfRooms; i++){
    //     SocketServer server(port);
    //     serverSockets.push_back(server);
    //     ServerThread st(server, port);
    //     port++;
    // }
    //cinWaiter.Wait();
    //std::cin.get();
    //server.Shutdown();
    SocketServer server(port);
    ServerThread st(server, port);
    while(true){
        std::string stopCommand;
        std::cout << "to stop all the server's enter the command: stop" << std::endl;
        
        getline(std::cin, stopCommand);
        std::cout << "command: " << stopCommand << std::endl;
        if(stopCommand == "done"){
            std::cout<< "I was told to stop" << std::endl;
            // for(int j = 0; j < numberOfRooms; j++){
            //     serverSockets[j].Shutdown();
            // }
            server.Shutdown();
            
            break;
        }
    
    } 

    std::cout << "GoodBye!" << std::endl;

    
}