//#define _CRT_SECURE_NO_WARNINGS
//#pragma warning(disable : 4996)
//
//#include "bither.h"
//
//#define TO_ORIGINAL 33
//#define NUM_OF_ASCII 94
//#define PASSLEN 6
//
//#define MAX_LINES 50
//#define LINE_LENGTH 25
//
//
//int main()
//{
//    double total_clock = 0;
//    double total_time = 0;
//    int len;
//    char generatedPassword[11];
//    char real_password[21];
//    char lastPassword[11];
//    uint8_t aesKey[33] = { 0, };
//    clock_t start, end;
//    time_t stt, endt;
//    BitherInfo bither = { 0, };
//
//
//    // to open all test files
//    WIN32_FIND_DATA findFileData;
//    HANDLE hFind = FindFirstFile("C:\\Users\\Bori\\Desktop\\bither_crack\\bither_crack\\bither_db\\*", &findFileData);
//
//    FILE* result_file = fopen("./bither_result.txt", "a");
//
//    if (hFind == INVALID_HANDLE_VALUE) {
//        printf("FindFirstFiel failed (%d)\n", GetLastError());
//        return FAIL;
//    }
//
//    int count_len = 0;
//
//    do {
//        total_clock = 0;
//        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//            printf("File name : %s\n", findFileData.cFileName);
//
//            char filePath[MAX_PATH];
//            sprintf(filePath, ".\\bither_db\\%s", findFileData.cFileName);
//
//            // open keystore file
//            FILE* file = fopen(filePath, "r");
//            if (file) {
//                len = count_len++ / 10 + 6;
//
//                //printf("Target Keystore : %s\n", filePath);
//                fprintf(result_file, "Target Keystore : %s\n", filePath);
//
//                // parsing target keystore file
//                if (bither_data_parsing(&bither, (const char*)filePath) == FAIL)
//                {
//                    printf("*** Data parsing error ***\n");
//                    return FAIL;
//                }
//
//                // Start password generation per each file
//                for (int i = 0; i < len; i++) {
//                    generatedPassword[i] = TO_ORIGINAL;
//                    lastPassword[i] = TO_ORIGINAL + NUM_OF_ASCII - 1;
//
//                    real_password[2 * i] = ((uint16_t)generatedPassword[i] & 0xff00) >> 8;
//                    real_password[2 * i + 1] = ((uint16_t)generatedPassword[i] & 0x00ff);
//                }
//                generatedPassword[len] = '\0';
//                lastPassword[len] = '\0';
//
//                do {
//                    // Print the current password
//                    //printf("%s\n", generatedPassword);
//                    start = clock();
//                    bither_key_derivation(real_password, 2 * len, aesKey, &bither);
//                    if (bither_key_verification(aesKey, &bither) == SUCCESS)
//                    {
//                        end = clock();
//                        total_clock += (end - start);
//                        printf("Find Password!\n");
//                        printf("Password : %s\n", generatedPassword);
//                        break;
//                    }
//                    end = clock();
//                    total_clock += (end - start);
//
//                    // Increment the generated password
//                    for (int i = len - 1; i >= 0; i--) {
//                        if (generatedPassword[i] < TO_ORIGINAL + NUM_OF_ASCII - 1) {
//                            generatedPassword[i]++;
//                            real_password[2 * i] = ((uint16_t)generatedPassword[i] & 0xff00) >> 8;
//                            real_password[2 * i + 1] = ((uint16_t)generatedPassword[i] & 0x00ff);
//                            break;
//                        }
//                        else {
//                            generatedPassword[i] = TO_ORIGINAL;
//                            real_password[2 * i] = ((uint16_t)generatedPassword[i] & 0xff00) >> 8;
//                            real_password[2 * i + 1] = ((uint16_t)generatedPassword[i] & 0x00ff);
//                        }
//                    }
//                } while (strcmp(generatedPassword, lastPassword) != 0);
//            
//                printf("Total clock taken to find Password : %.2f clock\n", total_clock);
//                fprintf(result_file, "Total clock taken to find Password : %.2f clock\n", total_clock);
//                
//                fclose(file);
//            }
//            else {
//                printf("Failed to open %s file\n", filePath);
//            }
//        }
//    } while (FindNextFile(hFind, &findFileData) != 0);
//
//    fclose(result_file);
//
//    FindClose(hFind);
//    return 0;
//}
