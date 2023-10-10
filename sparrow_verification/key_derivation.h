#pragma once
#include <openssl/ec.h>
#include <argon2.h>
#include <ctype.h>

void get_salt(uint8_t* sparrow_salt, const char* db_name);
int key_derivation(const uint8_t* password, const uint8_t* salt, char* pubKeyComp, EC_GROUP* curve, BIGNUM* privKey, BIGNUM* x, BIGNUM* y);
