#ifndef __UDP_H__
#define __UDP_H__

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>

class udp
{
private:
    int serverSocket;//for  server mode

    int clientSocket;//for client mode
    sockaddr_in sad;//for both server and client mode

private:
    int mode;//server or client mode
    static int serverM;
    static int clientM;

    int inited;
public:
    udp(int aMode)
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
    int close();

};



#endif
