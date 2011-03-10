#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int matching(unsigned char *prefix, unsigned char *input, int length);
int randomMsg(unsigned char *msg);
int digMsg(unsigned char *msg, unsigned char *dig);

int main()
{
    unsigned char targetMsg[20];
    unsigned char targetDig[16];
    unsigned char matchDigPrefix[16];
    unsigned int matchLen;
    unsigned int input;
    int i;
    int digCounter = 0;


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

    srand(time(NULL));

    do
    {
        randomMsg(targetMsg);
        digMsg(targetMsg, targetDig);
        digCounter++;
    }while (matching(matchDigPrefix, targetDig, matchLen) != 0);


    fprintf(stdout, "\n\nMatch in %d runs\n", digCounter);
    
    fprintf(stdout, "Message in hexadecimal format\n");
    for (i = 0; i < 20; i++)
    {
        fprintf(stdout, "%02x", targetMsg[i]);
    }
    printf("\n");

    fprintf(stdout, "MD5 Digest in hexadecimal format\n");
    for (i = 0; i < 16; i++)
    {
        fprintf(stdout, "%02x", targetDig[i]);
    }
    printf("\n");

    return 0;
}

/*
 * compare prefix and input, according to given length, 
 * give the result
 *
 * @param prefix digest prefix
 * @param input produced message digest
 * @param length how many bytes to be compare
 *
 * @ret the result, 0: matched, otherwise: not matched
 *
 */
int matching(unsigned char *prefix, unsigned char *input, int length)
{
    int i;
    
    for (i = 0; i < length; i++)
    {
        if (prefix[i] - input[i] != 0)
            return prefix[i] - input[i];
    }

    return 0;

}


/*
 * randomly give out a 20 bytes message
 *
 * @param[out] msg randomly produced message
 *
 */
int randomMsg(unsigned char *msg)
{
   int i;

   for (i = 0; i < 20; i++)
       msg[i] = rand() % 256;

   return 0;
}

/*
 * get MD5 digest from given message
 *
 * @param[in] msg given message
 * @param[out] MD5 digest
 *
 */
int digMsg(unsigned char *msg, unsigned char *dig)
{
    EVP_MD_CTX mdctx;
    int md_len;

    OpenSSL_add_all_digests();

    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, EVP_md5(), NULL);
    EVP_DigestUpdate(&mdctx, msg, 20);
    EVP_DigestFinal_ex(&mdctx, dig, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);

    return 0;
}
