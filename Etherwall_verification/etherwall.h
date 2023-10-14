#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uchar.h>
#include <locale.h>
#include <assert.h>

#include <sodium.h>
#include <json-c/json.h>

#include <iostream>
#include <string>
#include <cryptopp/keccak.h>
#include <cryptopp/hex.h>

#define SUCCESS 0
#define FAIL 1

using namespace std;

typedef struct Account {
    char ciphertext[65];
    char iv[33];
    char mac[65];
    char kdf_method[10];
    int kdf_n;
    int kdf_p;
    int kdf_r;
    char kdf_salt[65];
} Account;


