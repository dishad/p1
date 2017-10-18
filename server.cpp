// Server

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <signal.h>

using namespace std;

int main (int argc , char *argv[])
{
    int sockfd, clifd, read_size;
    socklen_t clilen;
    struct sockaddr_in serverAddr, clientAddrAddr;
    char buffer[256];
    string ip;
    int port;

	ifstream config;
	config.open("config.txt");
    while(!config.eof()) {
            config >> ip;
            config >> port;
    }

    config.close();


    //Create socket
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons( 8888 );

    //Bind
    if( bind(sockfd,(struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

     //Listen
    listen(sockfd , 5);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    clilen = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
<<<<<<< HEAD:server.cpp
    clifd = accept(sockfd, (struct sockaddr *)&clientAddr, (socklen_t*)&clilen);
=======
    clifd = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&clilen);
>>>>>>> 1d35b2188609212152ccd0c59c5ebb9fa4645748:lol.cpp
    if (clifd < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while( (read_size = recv(clifd , buffer , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(clifd , buffer , strlen(buffer));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

	system("pause");
    return 0;
}
