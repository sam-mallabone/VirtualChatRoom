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
    std::vector<SocketThread*> &clientSocketThreads;

    int chatRoom;
public:
    SocketThread(Socket& socket, bool& terminate, int port, std::vector<SocketThread*> &clientSocketThreads)
    : socket(socket), terminate(terminate), port(port), clientSocketThreads(clientSocketThreads)
    {}

    ~SocketThread()
    {}

    Socket& GetSocket()
    {
        return socket;
    }

    const int GetChatRoom()
    {
        return chatRoom;
    }

    virtual long ThreadMain()
    {
    //first get a reference to the semaphore
    //need the string of the port number
    std::string portstr = std::to_string(port);
    Semaphore protect(portstr);
    //passed to this function is a socket and an array of sockets by reference
    try{
        socket.Read(data);
        std::string chatRoomNum = data.ToString();
        std::cout << chatRoomNum << std::endl;
        chatRoom = std::stoi(chatRoomNum);
        std::cout << chatRoom << std::endl;
        while(!terminate) {
            int j = socket.Read(data);
            if(j == 0){
                break;
            }
            std::string recStr = data.ToString();
            if(recStr == "shutdown\n") {
                //get access to semaphore
                protect.Wait();
                //remove this socket from the client sockets vector
                clientSocketThreads.erase(std::remove(clientSocketThreads.begin(), clientSocketThreads.end(), this), clientSocketThreads.end());
                //release the semaphore
                protect.Signal();
                //break out of the loop
                std::cout<< "A client has sent notice to shutdown... removing the client" << std::endl;
                break;
            }
            if(recStr[0]=='/'){
                std::string chatStr = recStr.substr(1, recStr.size() - 1);
                chatRoom = std::stoi(chatStr);
                std::cout << chatRoom << std::endl;
                continue;
            }
            //get the semaphore so thread can go into critical area
            protect.Wait();
            for(int i = 0; i < clientSocketThreads.size(); i++) {
                SocketThread* clientSocketThread = clientSocketThreads[i];
                if (clientSocketThread->GetChatRoom() == chatRoom)
                {
                    Socket& clientSocket = clientSocketThread->GetSocket();
                    ByteArray sendBa(recStr);
                    clientSocket.Write(sendBa);
                }
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
    std::cout<<"exiting a thread on now" << std::endl;
    }
};

// This thread handles the server operations
class ServerThread : public Thread
{
private:
    SocketServer& server;
    std::vector<SocketThread*> socketThreads;
    bool terminate = false;
    int port;
    int chatNum;
public:
    ServerThread(SocketServer& server, int port, int chatNum)
    : server(server), port(port), chatNum(chatNum)
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
                std::cout << "Waiting for a client" <<std::endl;
                Semaphore protect(portstr, 1, true);
                std::string numberOfChats = std::to_string(chatNum) + '\n';
                ByteArray ba(numberOfChats); 
                // Wait for a client socket connection
                Socket sock = server.Accept();
                sock.Write(ba);
                Socket* newConnection = new Socket(sock);

                // Pass a reference to this pointer into a new socket thread
                Socket& socketReference = *newConnection;
                //clientSockets.push_back(sock);
                socketThreads.push_back(new SocketThread(socketReference, terminate, port, std::ref(socketThreads)));
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
    int numberOfRooms;
    //this is the main driver for the server, it controls the spawning of
    //a server thread and controls when the server will gracefully terminate
    std::cout << "I am the Server" << std::endl;
    std::cout << "How many chat rooms would you like? Please enter an int" << std::endl;
    std::cin >> numberOfRooms;
    std::cout << "Please type done to end the Server" << std::endl;
    SocketServer server(port);
    ServerThread st(server, port, numberOfRooms);
    while(true){
        std::string stopCommand;
        getline(std::cin, stopCommand);
        if(stopCommand == "done"){
            std::cout<< "I was told to stop" << std::endl;
            server.Shutdown();
            break;
        }
    
    } 

    std::cout << "GoodBye!" << std::endl;
}