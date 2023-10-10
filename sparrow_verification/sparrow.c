#include "sparrow_db.h"
#include "key_derivation.h"
#include <openssl/obj_mac.h>

int main() {

    SQLHANDLE env;
    SQLHANDLE con;
    initialize_environment(&env);

    char db_name[] = "test";
    start_server(db_name);

    //salt parsing
    uint8_t sparrow_salt[16];
    get_salt(sparrow_salt, db_name);

    //password guessing
    uint8_t* password[] = { "1111", "1112" };
    int passwordCount = sizeof(password) / sizeof(password[0]);

    //precompute EC_GROUP
    EC_GROUP* curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!curve) {
        fprintf(stderr, "Failed to create curve\n");
        return -1;
    }
    BIGNUM* privKey = BN_new();
    BIGNUM* x = BN_new();
    BIGNUM* y = BN_new();

    clock_t start = clock();
    /*for (int i = 0; i < passwordCount; i++) {
        clock_t try_t = clock();

        //Key derivation
        char pubKeyComp[68];
        key_derivation(password[i], sparrow_salt, pubKeyComp, curve, privKey, x, y);
        strcat_s(pubKeyComp, sizeof(pubKeyComp), " ");

        //Crack DB
        clock_t try_connect = clock();
        if (test_connection(env, db_name, pubKeyComp, &con)) {
            clock_t end = clock();
            printf("[%d] try [%s] success : %.3f \n", i + 1, password[i], (double)(end - try_t) / CLOCKS_PER_SEC);
            printf("Total Time : %.3f \n", (double)(end - start) / CLOCKS_PER_SEC);
            break;
        }
        else {
            clock_t end = clock();
            printf("[%d] try [%s] fail : %.3f \n", i + 1, password[i], (double)(end - try_t) / CLOCKS_PER_SEC);
            printf("[%d] db connect time : %.3f \n", i + 1, (double)(end - try_connect) / CLOCKS_PER_SEC);
            if (i + 1 % 100 == 0) {
                printf("Total Time : %.3f \n", (double)(end - start) / CLOCKS_PER_SEC);
            }
        }
    }*/
    double totaltime = 0;
    clock_t try_connect;
    clock_t end;
    for (int i = 0; i < 1000; i++) {
        //Key derivation
        char pubKeyComp[68];
        key_derivation(password[1], sparrow_salt, pubKeyComp, curve, privKey, x, y);
        strcat_s(pubKeyComp, sizeof(pubKeyComp), " ");

        //Crack DB
        try_connect = clock();
        if (test_connection(env, db_name, pubKeyComp, &con)) {
            break;
        }
        else {
            end = clock();
            totaltime += (double)(end - try_connect);
            if ((i + 1) % 100 == 0) {
                printf("%d \n", i+1);
                printf("mean time : %.3f \n", totaltime / (i + 1) / CLOCKS_PER_SEC);
            }
        }
    }
    printf("result mean time : %0.3f", totaltime/ 1000 / CLOCKS_PER_SEC);
    BN_free(privKey);
    BN_free(x);
    BN_free(y);
    EC_GROUP_free(curve);

    printf("Press Enter to continue...\n");
    (void)getchar();

    return 0;
}