#ifndef __UDP_H__
#define __UDP_H__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

class udp
{
private:
    int serverSocket;//for  server mode
    sockaddr_in cad;//temporary client info
    int clen;//temporary client info

    int clientSocket;//for client mode
    sockaddr_in sad;//for both server and client mode

private:
    int mode;//server or client mode
    static int serverM;
    static int clientM;

    int inited;
public:
    udp(int aMode)//-1: client, 1:server
    {
        mode = aMode;
        inited = 0;
    };
    ~udp()
    {
        if (inited == 1)
        {
            if (mode == udp::serverM)
                ::close(serverSocket);
            if (mode == udp::clientM)
                ::close(clientSocket);
        }
    };
public:
    int init(int port);//server mode
    int init(const char *host, int port);//client 
    int send(const char *msg, int len);//for client
    int recv(char *msg, int len);//for server
    int senderInfo(char *host, int &port);//should be invoked immediately
                                                //after recv()
    int close();//close the socket

};



#endif
