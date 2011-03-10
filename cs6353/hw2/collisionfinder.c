#include <openssl/ssl.h>
#include <stdio.h>


int main()
{
    unsigned char targetMsg[20];
    unsigned char targetDig[16];
    unsigned char matchDigPrefix[16];
    unsigned int matchLen;
    unsigned int input;
    int i;


    fprintf(stdout, "Enter Number of Bytes to match\n");
    scanf("%d", &matchLen);

    fprintf(stdout, "Enter the byte values to match for %d bytes\n",
                     matchLen);
    for (i = 0; i < matchLen; i++)
    {
        scanf("%u", &input);
                
        matchDigPrefix[i] = (unsigned char)input;
    }

    fprintf(stdout, "TargetDigestPrefix in hexadecimal for %d bytes\n",
                    matchLen);
    for (i = 0; i < matchLen; i++)
    {
        fprintf(stdout, "%02x", matchDigPrefix[i]);
    }
    
    printf("\n");

    return 0;
}
