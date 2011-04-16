/*
 * Author: Zi Yan
 *
 * Date: 2-16-2011
 *
 * Description: Bank program
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define ACC_NUM 10

//bank account data structure
typedef struct bank_acc
{
    char accNum[20];
    char zipcode[10];
    int amount;
}bank_acc;

//bank account info
bank_acc bAccount[ACC_NUM]=
{
    {"1111","22222",1000},
    {"2222","33333",50},
    {"3333","44444",25},
    {"4444","55555",78},
    {"5555","66666",465},
    {"6666","77777",424},
    {"7777","88888",123},
    {"8888","99999",457},
    {"9999","00000",789},
    {"0000","11111",741}
};

/*
 * error message output
 *
 * @para msg error message
 *
 */
int error(const char *msg)
{
    fprintf(stderr, "%s",msg);
    exit(1);
}


/*
 * the function for interacting with the browser
 *
 * @para data client socket
 *
 */ 
void *runBanker(void *msg)
{
    int clntSock;
    char buf[200];
    char reply[200];
    char accNum[20];
    char zipcode[10];
    char tmp[10];
    int amount;
    char *pElem, *tail;
    int i, opAcc = -1;

    memcpy(&clntSock, msg, sizeof(clntSock));

    int recvNum = recv(clntSock, buf, 200, 0);
    if (recvNum < 0)
        error("ERROR: receiving data error\n");
    
    fprintf(stderr, "LOG:got message: %s\n", buf);
    pElem = buf;

    //parse the msg
    memset(accNum, 0, 20);
    pElem = strchr(pElem, '$');
    pElem = pElem + 1;
    tail = strchr(pElem, '$');
    strncpy(accNum, pElem, tail - pElem);

    memset(zipcode, 0, 10);
    pElem = tail + 1;
    tail = strchr(pElem, '$');
    strncpy(zipcode, pElem, tail - pElem);

    pElem = tail + 1;
    tail = strchr(pElem, '$');
    strncpy(tmp, pElem, tail - pElem);
    amount = atoi(tmp);

    //fprintf(stderr, "\n%s:%s,%s,%d\n", buf, accNum, zipcode, amount);

    //find the account
    for (i = 0; i < ACC_NUM; i++)
    {
        if ((strcmp(bAccount[i].accNum, accNum) == 0)
            && (strcmp(bAccount[i].zipcode, zipcode) == 0))
        {
            opAcc = i;
            break;
        }

    }

    if (opAcc == -1)
    {
        sprintf(reply, "NO:No such a bank account.");
    }
    else
    {
        if (strncmp(buf, "ADD", 3) == 0)
        {
            sprintf(reply, "OK:Credit to your account.");
            bAccount[opAcc].amount += amount;
        }
        else if (strncmp(buf, "SUBSTRACT", 9) == 0)
        {
            if (bAccount[opAcc].amount < amount)
                sprintf(reply, "NO:Not enough balance.");
            else
            {
                sprintf(reply, "OK:Received your payment.");
                bAccount[opAcc].amount -= amount;
            }

        }
        else
        {
            sprintf(reply,"NO:Undefined operation.");
        }
    }

    int sendNum = send(clntSock, reply, sizeof(reply), 0);
    if (sendNum < 0)
        error("ERROR:Sending error\n");

    fprintf(stderr, "LOG:sending a message:%s\n", reply);

    close(clntSock);
}



int main(int argc, char** argv)
{
    int b_port;
    int listenSock, accSock;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    struct hostent *hostp;
    int clntlen;
    int i;

    //verify arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: bank <bank_port>!\n");
        exit(1);
    }


    //get bank port
    b_port = atoi(argv[1]);

    //make a socket
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock < 0)
        error("ERROR: cannot open socket\n");

    //set option
    int optval = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));

    bzero((char *)&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) b_port);

    //binding socket with port
    if (bind(listenSock, (struct sockaddr *)&serveraddr,
                sizeof(serveraddr)) < 0)
        error("ERROR: binding error\n");

    //listen to the port
    if (listen(listenSock, 5) < 0)
        error("ERROR: listen error\n");

    clntlen = sizeof(clientaddr);

    while(1)
    {
        //accept a new client
        accSock = accept(listenSock, (struct sockaddr *)&clientaddr, &clntlen);
        if (accSock < 0)
            error("ERROR: accept error\n");

        //give the client to a new thread
        pthread_t tid;
        pthread_create(&tid, NULL, runBanker, (void *)&accSock);
    }

    return 0;

}



