/*
 * Author: Zi Yan
 *
 * Date: 2-16-2011
 *
 * Description: Hotel program
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

#define BUFFER_SZ 10240
#define ROOMNO    30


/*
 * Hotel room data structure
 *
 */
typedef struct hotel_room{
    int avail;
    char date[11];
    char creditcard[20];
    char zipcode[10];
    char conf[10];
}room;


room hRoom[ROOMNO];

int h_port, b_port;
char b_host[100];

//lock
pthread_mutex_t roomLock = PTHREAD_MUTEX_INITIALIZER;


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
 * communicate with remote bank program
 *
 * @para type indicate deposit or withdraw
 * @para accNum credit card number
 * @para zipcode zipcode for the credit card
 * @para amount the amount of money
 * 
 * @ret commuication result
 *
 */
int bank(int type, char *accNum, char *zipcode, int amount)
{
    int sock;
    struct hostent *server;
    struct sockaddr_in serveraddr;

    //connecting operation
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        error("ERROR: opening socket error\n");
    server = gethostbyname(b_host);
    if (server == NULL)
        error("ERROR: no such host\n");

    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(b_port);

    if (connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        error("ERROR: connecting error\n");


    if (type == 0)//reserve
    {
        char msg[200];
        sprintf(msg, "SUBSTRACT$%s$%s$%d$",
                     accNum,
                     zipcode,
                     amount);
        fprintf(stderr, "LOG: message sent to bank:\n%s\n", msg);
        int sendNum = send(sock, msg, sizeof(msg), 0);
        if (sendNum != sizeof(msg))
            error("ERROR: sending error\n");
        
    }
    else if (type == 1)//cancel
    {
        char msg[200];
        sprintf(msg, "ADD$%s$%s$%d$",
                     accNum,
                     zipcode,
                     amount);
        fprintf(stderr, "LOG: message sent to bank:\n%s\n", msg);
        int sendNum = send(sock, msg, sizeof(msg), 0);
        if (sendNum != sizeof(msg))
            error("ERROR: sending error\n");

    }
    else
    {
        error("ERROR: undefined bank operation\n");
    }

    //bank's response
    char recvBuf[200];
    int recvNum = recv(sock, recvBuf, 200, 0);
    if (recvNum < 0)
        error("ERROR: receiving data error\n");

    
    fprintf(stderr, "LOG: bank response:\n%s\n", recvBuf);
    if (strncmp(recvBuf, "OK", 2) == 0)
    {
        return 1;
    }
    else if (strncmp(recvBuf, "NO", 2) == 0)
    {
        return -1;
    }
    else
    {
        error("ERROR: unknown bank response\n");
    }
}

/*
 * reserving a room 
 *
 * @para rData received message
 * @para sData the message to be sent
 *
 */
void reserve(char *rData, char *sData)
{
    char *pElem, *andPos;
    char month[3], day[3], year[5], date[20];
    char accNum[20], zipcode[10];
    int i;
    int roomno, confnum, bank_ret;

    fprintf(stderr, "Reserve begin\n");

    //parse the request
    memset(month, 0, 3);
    pElem = strstr(rData, "Rmonth");
    pElem = pElem + 7;//size of Rmonth=
    strncpy(month, pElem, 2);
    if (month[1] == '&')
        month[1] = 0;

    memset(day, 0, 3);
    pElem = strstr(rData, "Rday");
    pElem = pElem + 5;//size of Rday=
    strncpy(day, pElem, 2);
    if (day[1] == '&')
        day[1] = 0;

    memset(year, 0, 5);
    pElem = strstr(rData, "Ryear");
    pElem = pElem + 6;//size of Ryear=
    strncpy(year, pElem, 4);

    memset(date, 0, 20);
    sprintf(date, "%s-%s-%s", month, day, year);
    if (strcmp(date, "3-21-2011") != 0)
    {
        sprintf(sData, "There is no room.\n");
        return;
    }

    fprintf(stderr, "LOG:Date: %s\n", date);

    memset(accNum, 0, 20);
    pElem = strstr(rData, "Raccountnumber");
    pElem = pElem + 15;
    andPos = strchr(pElem, '&');
    strncpy(accNum, pElem, andPos - pElem); 


    memset(zipcode, 0, 10);
    pElem = strstr(rData, "Rzipcode");
    pElem = pElem + 9;
    andPos = strchr(pElem, '&');
    strncpy(zipcode, pElem, andPos - pElem); 



    //find a room
    //lock needed
    pthread_mutex_lock(&roomLock);
    for (i = 0; i < ROOMNO; i++)
        if (hRoom[i].avail == 0)//find a room to reserve
        {
            roomno = i;
            hRoom[i].avail = 1;
            sprintf(hRoom[i].date, "%s-%s-%s", month, day, year);
            strcpy(hRoom[i].creditcard, accNum);
            strcpy(hRoom[i].zipcode, zipcode);
            sprintf(hRoom[i].conf, "%d%s%s%s", i, month, day, year);

            sprintf(sData, "You RESERVE a room with No:%d\n"
                           "on %s-%s-%s\n"
                           "confirmation No is:%s\n"
                           "$50 is charged\n",
                           roomno,
                           month, day, year,
                           hRoom[roomno].conf);
            break;
        }
    //unlock
    pthread_mutex_unlock(&roomLock);
    
    if (i == ROOMNO)
        strcpy(sData, "No room is left. Sorry.\n");
    else //communicate with bank
        bank_ret = bank(0, accNum, zipcode, 50);

    if (bank_ret < 0)
    {
        strcpy(sData, "Error with your bank account.\n");
        hRoom[roomno].avail = 0;
    }
}

/*
 * cancelling a room
 *
 * @para rData received message
 * @para sData the message to be sent
 *
 */
void cancel(char *rData, char *sData)
{
    char *pElem, *andPos;
    char month[3], day[3], year[5], date[20];
    char roomNum[5], confNum[20];
    char accNum[20], zipcode[10];
    int i;
    int roomno, confnum, bank_ret;

    fprintf(stderr, "Cancel begin\n");

    //parse the request
    memset(month, 0, 3);
    pElem = strstr(rData, "Cmonth");
    pElem = pElem + 7;//size of Rmonth=
    strncpy(month, pElem, 2);
    if (month[1] == '&')
        month[1] = 0;

    memset(day, 0, 3);
    pElem = strstr(rData, "Cday");
    pElem = pElem + 5;//size of Rday=
    strncpy(day, pElem, 2);
    if (day[1] == '&')
        day[1] = 0;

    memset(year, 0, 5);
    pElem = strstr(rData, "Cyear");
    pElem = pElem + 6;//size of Ryear=
    strncpy(year, pElem, 4);


    memset(date, 0, 20);
    sprintf(date, "%s-%s-%s", month, day, year);
    if (strcmp(date, "3-21-2011") != 0)
    {
        sprintf(sData, "No room is reserved.\n");
        return;
    }
    fprintf(stderr, "LOG:Date: %s\n", date);

    memset(confNum, 0, 20);
    pElem = strstr(rData, "Cconfirmationnumber");
    pElem = pElem + 20;
    andPos = strchr(pElem, '&');
    strncpy(confNum, pElem, andPos - pElem); 


    memset(roomNum, 0, 5);
    pElem = strstr(rData, "Croomnumber");
    pElem = pElem + 12;
    andPos = strchr(pElem, '&');
    strncpy(roomNum, pElem, andPos - pElem); 
    roomno = atoi(roomNum);

    memset(accNum, 0, 20);
    memset(zipcode, 0, 10);
    //find the room
    //lock needed
    pthread_mutex_lock(&roomLock);
    if ((strcmp(hRoom[roomno].conf, confNum) == 0)
        && (hRoom[roomno].avail == 1))
    {
        hRoom[roomno].avail = 0;
        strcpy(accNum, hRoom[roomno].creditcard);
        strcpy(zipcode, hRoom[roomno].zipcode);

        fprintf(stderr, "Room found\n");
    }
    else
    {
        strcpy(sData, "No such a room is reserved.\n");
        pthread_mutex_unlock(&roomLock);
        return;
    }
    //unlock
    pthread_mutex_unlock(&roomLock);


    bank_ret = bank(1, accNum, zipcode, 50);

    if (bank_ret < 0)
        strcpy(sData, "Error with your bank account.\n");
    else
        strcpy(sData, "Your reservation is cancelled.\n"
                      "Your payment is refunded.\n");

//    fprintf(stderr, "%s", sData);
}

/*
 * produce sending message
 *
 * @para rData received message
 * @para sData the message to be sent
 *
 */
void replyReq(char *rData, char *sData)
{
    char *req;
    char content[11];

    req = strstr(rData, "whattodo");
    if (req == NULL)
    {
        //error("ERROR: no whattodo found\n");
        sprintf(sData, "Data corrupted\n");
        return;
    }
    req = req + 9;

    strncpy(content, req, 11);

    //get a reservation msg
    if (strcmp(content, "reservation") == 0)
    {
        reserve(rData, sData);
    }
    //get a cancelation msg
    else if (strcmp(content, "cancelation") == 0)
    {
        cancel(rData, sData);
    }
    //undefined msg
    else
    {
        error("undefined request\n");
    }

}

/*
 * the function for interacting with the browser
 *
 * @para data client socket
 *
 */ 
void *runClient(void *data)
{
    int clntSock;
    char buf[BUFFER_SZ];
    int readNum;
    int state = 0;

    memcpy(&clntSock, data, sizeof(clntSock));

    while (state != 2)
    {
        //read msg sent from the browser
        readNum = recv(clntSock, buf, BUFFER_SZ, 0);
        if (readNum < 0)
            error("ERROR: reading from socket error\n");


        fprintf(stderr,"LOG:Get msg: size:%d\n%s\n", readNum, buf);


        //GET method process
        if (strncmp(buf, "GET", 3) == 0)
        {
            char *fileBuf = NULL;
        
            int fileLen = getIndex(&fileBuf);


            int sendNum;
            
            fprintf(stderr, "LOG:index.html sent\n");
            sendNum = send(clntSock, fileBuf, fileLen, 0);
            if (sendNum < fileLen)
            {
                error("ERROR: sending file error\n");
            }

            if (fileBuf != NULL)
            {
                free(fileBuf);
                fileBuf = NULL;
            }
            
            state = 2;

        }
        //POST method process
        else if (strncmp(buf, "POST", 4) == 0)
        {

            char replyBuf[1024];
            replyReq(buf, replyBuf);
            fprintf(stderr, "LOG:Send msg:%s\n", replyBuf);
            send(clntSock, replyBuf, strlen(replyBuf), 0);

            state = 2;
        }
        else
        {
            error("ERROR: undefined operation\n");
        }
    }
    
    fprintf(stderr, "LOG:Finished\n");
    fprintf(stderr, "******************\n");
    close(clntSock);
}

/*
 * put the content of index.html into the buffer
 *
 * @para buffer the buffer for file content
 * @ret file size
 *
 */
int getIndex(char **buffer)
{
    FILE *index;
    long file_sz;
    
    index = fopen("index.html", "r");
    if (index == NULL)
        error("ERROR: index.html not found\n");

    fseek(index, 0, SEEK_END);
    file_sz = ftell(index);

    *buffer = (char *)malloc(file_sz);
    if (*buffer == NULL)
        error("ERROR: out of mem\n");
    
    rewind(index);

    int len = fread(*buffer, 1, file_sz, index);
    if (len != file_sz)
        error("ERROR: file reading error\n");

    fclose(index);
     
    return file_sz;
}


int main(int argc, char** argv)
{
    int listenSock, accSock;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    struct hostent *hostp;
    int clntlen;
    int i;

    //check arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage: hotel <hotel_port> <bank_host>" 
                " <bank_port>!\n");
        exit(1);
    }

    //initialize the room info
    for (i = 0; i < ROOMNO; i++)
    {
        hRoom[i].avail = 0;//empty
    }

    //get hotel port, bank address, bank port
    h_port = atoi(argv[1]);
    strcpy(b_host, argv[2]);
    b_port = atoi(argv[3]);

    //construct a socket
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock < 0)
        error("ERROR: cannot open socket\n");

    //set socket option
    int optval = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));

    bzero((char *)&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) h_port);

    //binding address with socket
    if (bind(listenSock, (struct sockaddr *)&serveraddr,
                sizeof(serveraddr)) < 0)
        error("ERROR: binding error\n");

    //listen to hotel port
    if (listen(listenSock, 5) < 0)
        error("ERROR: listen error\n");

    clntlen = sizeof(clientaddr);

    while(1)
    {
        //accept new client
        accSock = accept(listenSock, (struct sockaddr *)&clientaddr, &clntlen);
        if (accSock < 0)
            error("ERROR: accept error\n");

        //send a new client to a thread
        pthread_t tid;
        pthread_create(&tid, NULL, runClient, (void *)&accSock);
    }

    return 0;

}

