/*
 * Author: Zi Yan
 * Date: 03/20/2011
 *
 */
#include "udp.h"
#include <cstring>
#include <iostream>
#include <errno.h>

//indicators of socket mode: server, client
int udp::serverM = 1;
int udp::clientM = -1;


/*
 * initialize socket for server mode
 *
 * @param port server port number
 *
 * @ret status
 */
int udp::init(int port)
{
    if (mode != udp::serverM)
        return -1;
    //start
    serverSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        std::cerr<<"socket creation failed\n";
        ::exit(1);
    }

    memset((char *)&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = INADDR_ANY;
    sad.sin_port = htons((u_short)port);

    if (bind(serverSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        std::cerr<<"bind failed: "<<errno<<"\n";
        ::exit(1);
    }

    inited = 1;
    return 0;
}

/*
 * initialize for client
 *
 * @param host host name to be connnected
 * @param port remote host port number
 *
 * @ret status
 *
 */
int udp::init(const char *host, int port)
{
    struct hostent *ptrh;

    if (mode != udp::clientM)
        return -1;

    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        std::cerr<<"Socket creation failed\n";
        ::exit(1);
    }

    memset((char *)&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons((u_short)port);
    ptrh = gethostbyname(host);

    if (((char *)ptrh) == NULL)
    {
        std::cerr<<"invalid host: "<<host<<std::endl;
        ::exit(1);
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

    inited = 1;
    return 0;
}

/*
 * send message to remote peer
 *
 * @param msg the content of a message
 * @param len message length
 *
 * @ret length of send message
 * 
 */
int udp::send(const char *msg, int len)
{
    if (inited == 0)
        return -1;
    
    return sendto(clientSocket, msg, len, 0,
                  (struct sockaddr *)&sad, sizeof(struct sockaddr));
}

/*
 * receive message from remote peer
 *
 * @param msg received message
 * @param len buffer len
 *
 * @ret length of received message
 *
 */
int udp::recv(char *msg, int len)
{
    //struct sockaddr_in cad;
    //int clen;

    if (inited == 0)
        return -1;

    return recvfrom(serverSocket, msg, len, 0,
                   (struct sockaddr *)&cad, (socklen_t *)&clen);
}

/*
 * give out the info of the sender of the received message
 *
 * @param host host name, dummy parameter
 * @param port port number
 *
 */
int udp::senderInfo(char *host, int &port)
{
    //inet_ntop(cad.sin_family, &cad.sin_addr, host, INET_ADDRSTRLEN);

    port = ntohs(cad.sin_port);

    return 0;
}

/*
 * close the connection
 *
 */
int udp::close()
{
    if (inited == 1)
        {
            if (mode == udp::serverM)
            {
                inited = 0;
                return ::close(serverSocket);
            }
            if (mode == udp::clientM)
            {
                inited = 0;
                return ::close(clientSocket);
            }
        }
}
