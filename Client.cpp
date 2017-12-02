#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

int main(void)
{
	std::string myStr;
	std::cout << "I am a client" << std::endl;	
	while(true)
	{
		//Ask for the user to input a string to server
		std::cout << "What text would you like to send to the server" << std::endl;
		std::getline(std::cin, myStr);
		if(myStr == "done")
		{
			//if the user enters "done" close the client
			std::cout << "Cleaning up...." << std::endl;
			break;
		}
		try{ 
			//create a new socket and open it
			Socket sock("127.0.0.1", 2000);
			int i = sock.Open();
			ByteArray ba(myStr);
			//send a ByteArray with the user's string to the server
			int j = sock.Write(ba);
			
			//command to close the server
			if(myStr == "closeserver")
			{
				//sleep for half a second to the server can receive and process the request
				usleep(500000);
				//Open one more socket, this socket's purpose is so it will free the server from the blocking .Accpet() call and
				//it will cause the server to exit the loop it's in
				Socket goodbyeSocket("127.0.0.1", 2000);
				goodbyeSocket.Open();
				//Send a goodbye message, close the socket and break from the while loop
				ByteArray goodbyeba("goodbye");
				goodbyeSocket.Write(goodbyeba);
				goodbyeSocket.Close();
				break;
			}
			//a byte array to receive the data
			ByteArray anotha("Text should be cleared");
			int k = sock.Read(anotha);
			//get the returned string and print it
			std::string returnstr = anotha.ToString();
			std::cout<< returnstr << std::endl;
			std::cout << std::endl;
			sock.Close();
		}
		catch(std::string& s)
		{
			//if the server is offline, it will enter this catch statement and exit the Client
			std::cout << "Server is offline, shutting down client now..." << std::endl;
			break;
		}
	}
	std::cout << "The client says goodbye..." << std::endl;
	
}
