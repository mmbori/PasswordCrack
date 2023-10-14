#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "etherwall.h"

int etherwall_parsing(Account* acc, const char* path) {
    FILE* file;
    errno_t err = fopen_s(&file, path, "r");
    if (err != 0 || !file) {
        printf("Error opening file.\n");
        return FAIL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1); // +1 for null-terminator
    if (!data) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return FAIL;
    }
    // +1 for null-terminator
    size_t bytesRead = fread(data, 1, length, file);
    if (bytesRead != length) {
        printf("Error reading file.\n");
        free(data);
        fclose(file);
        return FAIL;
    }

    fclose(file);
    data[length] = '\0';  // Null-terminate the read data

    struct json_object* json = json_tokener_parse(data);
    if (!json) {
        printf("Error parsing JSON.\n");
        free(data);
        return FAIL;
    }

    // JSON parsing using json-c
    struct json_object* crypto;
    json_object_object_get_ex(json, "crypto", &crypto);
    strcpy_s(acc->ciphertext, sizeof(acc->ciphertext), json_object_get_string(json_object_object_get(crypto, "ciphertext")));
    strcpy_s(acc->iv, sizeof(acc->iv), json_object_get_string(json_object_object_get(json_object_object_get(crypto, "cipherparams"), "iv")));
    strcpy_s(acc->mac, sizeof(acc->mac), json_object_get_string(json_object_object_get(crypto, "mac")));
    strcpy_s(acc->kdf_method, sizeof(acc->kdf_method), json_object_get_string(json_object_object_get(crypto, "kdf")));

    struct json_object* kdfparams = json_object_object_get(crypto, "kdfparams");
    acc->kdf_n = json_object_get_int(json_object_object_get(kdfparams, "n"));
    acc->kdf_p = json_object_get_int(json_object_object_get(kdfparams, "p"));
    acc->kdf_r = json_object_get_int(json_object_object_get(kdfparams, "r"));
    strcpy_s(acc->kdf_salt, sizeof(acc->kdf_salt), json_object_get_string(json_object_object_get(kdfparams, "salt")));

    json_object_put(json);  // Clean up
    free(data);

    return SUCCESS;
}

void hex_to_byte(const char* hex, unsigned char* byte, int len) {
    for (int i = 0; i < len; i++) {
        sscanf_s(hex + 2 * i, "%2hhx", &byte[i]);
    }
}

int etherwall_key_derivation(uint8_t* pwd, int pwd_len, uint8_t* aesKey, Account* etherwall)
{
    unsigned char salt[32] = { 0 };
    // Convert kdf_salt from hex to byte
    hex_to_byte(etherwall->kdf_salt, salt, sizeof(salt));

    if(crypto_pwhash_scryptsalsa208sha256_ll((const uint8_t*)pwd, (size_t)pwd_len, (const uint8_t*)salt, (size_t)32, (uint64_t)etherwall->kdf_n, 
        (uint64_t)etherwall->kdf_r, (uint64_t)etherwall->kdf_p, (uint8_t*)aesKey, (size_t)32) != 0) {
        cout << "Scryt error " << endl;
    }
    return SUCCESS;
}

int etherwall_key_verification(uint8_t* aesKey, Account* etherwall)
{
    unsigned char verify_input[48] = { 0 }; // 16 bytes for derivedKey[16:32] and 32 bytes for ciphertext

    // Prepare data for MAC computation: derivedKey[16:32] + ciphertext
    memcpy(verify_input, aesKey + 16, 16);
    hex_to_byte(etherwall->ciphertext, verify_input + 16, 32);

    // keccak =============================================
    CryptoPP::Keccak_256 keccak;

    uint8_t digest[CryptoPP::Keccak_256::DIGESTSIZE];

    keccak.CalculateDigest(digest, verify_input, 48);

    std::string output;
    CryptoPP::HexEncoder encoder;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    for (char& c : output) {
        c = std::tolower(c);
    }

    string parse_mac = etherwall->mac;

    if (output == parse_mac) {
        cout << "Password Verify!" << endl;
    }
    else {
        cout << "Wrong Password" << endl;
        return FAIL;
    }
    return SUCCESS;
}

int etherwall_verification(const uint8_t* pwd, int pwdLen)
{
    Account acc;
    if (etherwall_parsing(&acc, "UTC--2023-07-20T14-15-32.247165600Z--064792435aa50541d0949ec1240c4993618092f5") != SUCCESS) {
        cout << "Parsing error" << endl;
    }

    uint8_t aesKey[50] = { 0, };

    //for single password test
    uint8_t password[10] = "1111";
    int pwd_len = 4;

    etherwall_key_derivation(password, pwd_len, aesKey, &acc);
    etherwall_key_verification(aesKey, &acc);

    return 0;
}

