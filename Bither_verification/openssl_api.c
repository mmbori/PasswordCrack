#include "openssl_api.h"


int openssl_aes256(uint8_t* out, int outlen, uint8_t* in, int inlen, uint8_t* key, uint8_t* iv)
{
    EVP_CIPHER_CTX* openssl_aes_ctx;
    int olen = outlen;
    if (!(openssl_aes_ctx = EVP_CIPHER_CTX_new()))
    {
        return -1;
    }
    if (1 != EVP_CipherInit(openssl_aes_ctx, EVP_aes_256_cbc(), key, iv, AES_DECRYPT))
    {
        return -2;
    }
    EVP_CIPHER_CTX_set_padding(openssl_aes_ctx, 0);     // padding deactivate
    if (1 != EVP_CipherUpdate(openssl_aes_ctx, out, &olen, in, inlen))
    {
        return -3;
    }
    if (1 != EVP_CipherFinal_ex(openssl_aes_ctx, out, &olen))
    {
        return -4;
    }
    return 1;
}


int openssl_scrypt(uint8_t* key, int key_len, uint8_t* salt, int salt_len, uint8_t* password, int password_len)
{
    uint64_t maxmem[16];
    if (!(EVP_PBE_scrypt(password, password_len, salt, salt_len,
        SCRYPT_N, SCRYPT_N, SCRYPT_R, maxmem, key, key_len)))
        return -1;

    return 1;
}

//int openssl_keypairtest_secp256k1(uint8_t* privkey, int privkey_len, uint8_t* pubkey, int pubkey_len) 
//{
//    ECDSA_SIG* sig = ECDSA_SIG_new();
//    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
//    EC_GROUP* ecgrp = EC_GROUP_new_by_curve_name(NID_secp256k1);
//    EC_POINT* point = EC_POINT_new(ecgrp);
//    int ret = 0;
//
//    BIGNUM* priv = BN_bin2bn(privkey, 32, NULL);
//    EC_POINT_oct2point(ecgrp, point, pubkey, pubkey_len, NULL);
//    EC_KEY_set_private_key(eckey, priv);
//    EC_KEY_set_public_key(eckey, point);
//    uint8_t digest[32] = "hello";
//    sig = ECDSA_do_sign(digest, 32, eckey);
//    ret = ECDSA_do_verify(digest, 32, sig, eckey);
//
//    if (ret == 1) {
//        printf("it verified\n");
//        return 1;
//    }
//    return 0;
//}

// bither
//int keypairTest(uint8_t* privkey, uint8_t* pubkey)
//{
//    BN_CTX* bnCtx = BN_CTX_new();
//
//    ECDSA_SIG* sig = ECDSA_SIG_new();
//    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
//    EC_GROUP* ecgrp = EC_GROUP_new_by_curve_name(NID_secp256k1);
//    EC_POINT* point = EC_POINT_new(ecgrp);
//    int ret = 0;
//
//    BIGNUM* priv = BN_bin2bn(privkey, 32, NULL);
//    EC_POINT_oct2point(ecgrp, point, pubkey, BITHER_PUBKEY_LENGTH, NULL);
//    EC_KEY_set_private_key(eckey, priv);
//    EC_KEY_set_public_key(eckey, point);
//    uint8_t digest[32] = "hello";
//    sig = ECDSA_do_sign(digest, 32, eckey);
//    ret = ECDSA_do_verify(digest, 32, sig, eckey);
//
//    if (ret == 1) {
//        printf("** ECC key pair verified! **\n");
//        return 1;
//    }
//    else
//    {
//        printf("** ECC key pair test failed! **\n");
//        return -1;
//    }
//}
