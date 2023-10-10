#include "key_derivation.h"

void get_salt(uint8_t* sparrow_salt, const char* db_name) {

    char file_name[256];
    snprintf(file_name, sizeof(file_name), "%s.mv.db", db_name);

    FILE* db;
    errno_t err = fopen_s(&db, file_name, "rb");

    if (err != 0 || !db) {
        perror("Failed to open file");
        return;
    }

    fseek(db, 24, SEEK_SET);
    fread(sparrow_salt, 1, 16, db);
    fclose(db);
}

int key_derivation(const uint8_t* password, const uint8_t* salt, char* pubKeyComp, EC_GROUP* curve, BIGNUM* privKey, BIGNUM* x, BIGNUM* y) {

    uint8_t derivedKey[32];
    uint32_t time_cost = 10;
    uint32_t memory_cost = 256 * 1024;
    uint32_t parallelism = 4;
    uint32_t hash_length = 32;

    //Create derivedKey
    int result = argon2id_hash_raw(time_cost, memory_cost, parallelism, password, strlen((char*)password), salt, 16, derivedKey, hash_length);
    if (result != ARGON2_OK) {
        fprintf(stderr, "Error: %s\n", argon2_error_message(result));
        return -1;
    }

    //Convert derivedKey to BIGNUM
    BN_bin2bn(derivedKey, sizeof(derivedKey), privKey);

    //Create pubKey
    EC_POINT* pubKey = EC_POINT_new(curve);
    EC_POINT_mul(curve, pubKey, privKey, NULL, NULL, NULL);


    //Get affine coordinates (x, y) of pubKey
    if (!EC_POINT_get_affine_coordinates(curve, pubKey, x, y, NULL)) {
        fprintf(stderr, "Failed to get affine coordinates.\n");
        return -1;
    }

    //Compress pubKey
    int parity = BN_is_odd(y);
    char* x_hex = BN_bn2hex(x);
    snprintf(pubKeyComp, 67, "0%d%s", 2 + parity, x_hex);

    for (int i = 0; pubKeyComp[i]; i++) {
        pubKeyComp[i] = tolower(pubKeyComp[i]);
    }

    //Clean
    EC_POINT_free(pubKey);
    OPENSSL_free(x_hex);

    return 0;
}