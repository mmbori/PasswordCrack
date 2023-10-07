#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "bither.h"

// ------------------------------------------------------------------------------------------------------------------------------------------

void BITHERstr2bin(uint8_t* output, const int output_len, char* input, const int start, const int odd)
{
    char tmp[4] = { 0, };
    tmp[0] = '0';
    tmp[1] = 'x';
    if (odd == 0)
    {
        for (int i = 0; i < output_len; i++)
        {
            tmp[2] = input[(start + i) * 2];
            tmp[3] = input[(start + i) * 2 + 1];
            output[i] = strtol(tmp, NULL, 16);
        }
    }
    else
    {
        for (int i = 0; i < output_len; i++)
        {
            tmp[2] = input[(start + i) * 2 - 1];
            tmp[3] = input[(start + i) * 2];
            output[i] = strtol(tmp, NULL, 16);
        }
    }
}

int exec_callback(void* udp, int c_num, char** c_vals, char** c_names)
{
    memcpy(udp, c_vals[0], strlen(c_vals[0]));

    return 0;
}

// db file parsing
int bither_data_parsing(BitherInfo* bither, const char* file_name)
{
    uint8_t original[147];

    sqlite3* db;
    char* err_msg = 0;

    // file open
    int rc = sqlite3_open(file_name, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "cannot open database: %s\n", sqlite3_errmsg(db));
        return -11;
    }

    // SLQ query : extract encrypt_private_key value (length 146)
    char* query_1 = "select encrypt_private_key from addresses order by rowid desc limit 1";

    // save encrypt_private_key value in original
    rc = sqlite3_exec(db, query_1, exec_callback, original, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "failed to select data\n");
        fprintf(stderr, "sql error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    sqlite3_close(db);

    // convert parsing encPrivKey, IV, salt value
    BITHERstr2bin(bither->encPrivKey, BITHER_PRIVKEY_LEN, original, 0, 0);    // encPrivkey
    BITHERstr2bin(bither->iv, BITHER_IV_LEN, original, BITHER_PRIVKEY_LEN + 1, 1);           // IV
    BITHERstr2bin(bither->salt, BITHER_SALT_LEN, original, BITHER_PRIVKEY_LEN + 1 + BITHER_IV_LEN, 0);          // salt

    //// to check
    //printf("\nECPrivate key : \n");
    //for (int i = 0; i < BITHER_PRIVKEY_LEN; i++)
    //    printf("%02x ", bither->encPrivKey[i]);
    //printf("\nIV : \n");
    //for (int i = 0; i < BITHER_IV_LEN; i++)
    //    printf("%02x ", bither->iv[i]);
    //printf("\nSalt : \n");
    //for (int i = 0; i < BITHER_SALT_LEN; i++)
    //    printf("%02x ", bither->salt[i]);
    //printf("\n");

    return SUCCESS;
}


// using scrypt to derive key
int bither_key_derivation(uint8_t* pwd, int pwd16_len, uint8_t* aesKey, BitherInfo* bither)
{
    EVP_KDF* kdf;
    EVP_KDF_CTX* kctx;
    //uint8_t out[32];
    OSSL_PARAM params[6], * p = params;

    kdf = EVP_KDF_fetch(NULL, "SCRYPT", NULL);
    kctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);

    //uint8_t salt[8] = { 0x43, 0x7B, 0xEE, 0xCE, 0xC3, 0xA1, 0xD3, 0xC5 };

    uint64_t* scrypt_n = 16384;
    uint64_t** scrypt_n_p = &scrypt_n;

    uint32_t* scrypt_r = 8;
    uint32_t** scrypt_r_p = &scrypt_r;

    uint32_t* scrypt_p = 1;
    uint32_t** scrypt_p_p = &scrypt_p;

    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, pwd, pwd16_len);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, bither->salt, (size_t)8);
    *p++ = OSSL_PARAM_construct_uint64(OSSL_KDF_PARAM_SCRYPT_N, scrypt_n_p);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_SCRYPT_R, scrypt_r_p);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_SCRYPT_P, scrypt_p_p);
    *p = OSSL_PARAM_construct_end();

    if (EVP_KDF_derive(kctx, aesKey, 32, params) <= 0) {
        printf("\n*** EVP_KDF_derive error ***\n");
        return FAIL;
    }

    EVP_KDF_CTX_free(kctx);

    //// to check
    //printf("\nAES key : ");
    //for (int i = 0; i < 32; i++)
    //    printf("%02x ", aesKey[i]);
    //printf("\n");

    return SUCCESS;
}

int bither_key_verification(uint8_t* aesKey, BitherInfo* bither)
{
    uint8_t privKey[48] = { 0 };

    if (!(openssl_aes256(privKey, BITHER_PRIVKEY_LEN, bither->encPrivKey, BITHER_PRIVKEY_LEN, aesKey, bither->iv)))
    {
        printf("*** Decrypt private key error ***\n");
        return FAIL;
    }

    for (int i = 32; i < 48; i++) {
        if (privKey[i] != 0x10)
            return FAIL;
    }

    return SUCCESS;
}


int bither_crack(uint8_t* file_name, uint8_t* pwd)
{
    BitherInfo bither = { 0, };
    uint8_t aesKey[32] = { 0, };

    // convert password from UTF-8 to UTF-16
    int pwd16_len = sizeof(pwd) * 2;
    uint8_t* pwd_16 = (uint8_t*)malloc(pwd16_len);

    for (int i = 0; i < sizeof(pwd); i++)
    {
        pwd_16[2 * i] = 0x00;
        pwd_16[2 * i + 1] = pwd[i];
    }

    //// to check
    //printf("\nUTF-16 password : ");
    //for (int i = 0; i < pwd16_len; i++)
    //    printf("%02x ", pwd_16[i]);
    //printf("\n");


    // data parsing
    if (!(bither_data_parsing(&bither, file_name)))
    {
        printf("*** Data parsing error ***\n");
        return FAIL;
    }

    // aesKey derivation
    bither_key_derivation(pwd_16, pwd16_len, aesKey, &bither);

    // password verification
    if (!bither_key_verification(aesKey, &bither)) 
    {
        printf("\nWrong password\n");
        return FAIL;
    }

    printf("\n*** Find Password! ***\n");
    printf("\nPassword : %s\n", pwd);
    
    return SUCCESS;
}



int main()
{
    uint8_t* file_name = "address.db";
    uint8_t* pwd = "qwer1234";

    bither_crack(file_name, pwd);

    return 1;
}