#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
	RSA *myRSA;
    unsigned char cleartext[256] = { 0 };
    unsigned char encrypted[256] = { 0 };
    unsigned char decrypted[256] = { 0 };
    int resultEncrypt = 0;
    int resultDecrypt = 0;
    int i;
    int en_size;
    char err[300];

    srand(time(NULL));

    printf("Message you want to encrypt:\n");
    for (i = 0; i < 256; i++)
    {
        cleartext[i] = rand() % 256;
        printf("0x%02x ", cleartext[i]);
    }
    printf("\n");

    myRSA = RSA_generate_key(1024, 65537, NULL, NULL);
    
    en_size = RSA_size(myRSA);


    resultEncrypt = RSA_public_encrypt(en_size,
                                       cleartext, 
                                       encrypted, 
                                       myRSA, 
                                       RSA_NO_PADDING
                                       );
    resultEncrypt = RSA_public_encrypt(en_size,
                                       cleartext + en_size, 
                                       encrypted + en_size, 
                                       myRSA, 
                                       RSA_NO_PADDING
                                       );

    printf("%d from public encrypt.\n", resultEncrypt);


    resultDecrypt = RSA_private_decrypt(resultEncrypt, 
                                        encrypted, 
                                        decrypted, 
                                        myRSA, 
                                        RSA_NO_PADDING
                                        );
    resultDecrypt = RSA_private_decrypt(resultEncrypt, 
                                        encrypted + en_size, 
                                        decrypted + en_size, 
                                        myRSA, 
                                        RSA_NO_PADDING
                                        );

    printf("%d from private decrypt:\n", resultDecrypt, decrypted);

    for (i = 0; i < 256; i++)
        printf("0x%02x ", decrypted[i]);
    printf("\n");


//    printf("Cleartext is: %s", cleartext);
    resultEncrypt = RSA_private_encrypt(en_size, 
                                       cleartext, 
                                       encrypted, 
                                       myRSA, 
                                       RSA_NO_PADDING
                                       );
    resultEncrypt = RSA_private_encrypt(en_size, 
                                       cleartext + en_size, 
                                       encrypted + en_size, 
                                       myRSA, 
                                       RSA_NO_PADDING
                                       );

    printf("%d from private encrypt.\n", resultEncrypt, encrypted);


    resultDecrypt = RSA_public_decrypt(en_size, 
                                        encrypted, 
                                        decrypted, 
                                        myRSA, 
                                        RSA_NO_PADDING
                                        );
    resultDecrypt = RSA_public_decrypt(en_size, 
                                        encrypted + en_size, 
                                        decrypted + en_size, 
                                        myRSA, 
                                        RSA_NO_PADDING
                                        );

    printf("%d from public decrypt:\n", resultDecrypt);
    for (i = 0; i < 256; i++)
        printf("0x%02x ", decrypted[i]);
    printf("\n");

    RSA_free ( myRSA );

    return 0;
}
