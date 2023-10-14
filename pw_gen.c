#include <stdio.h>
#include <string.h>
#include <time.h>

#define TO_ORIGINAL 33
#define NUM_OF_ASCII 94

int main() {
    int len = 8;
    char generatedPassword[len + 1];
    char lastPassword[len + 1];

    for (int i = 0; i < len; i++) {
        generatedPassword[i] = TO_ORIGINAL;
        lastPassword[i] = TO_ORIGINAL + NUM_OF_ASCII - 1;
    }
    generatedPassword[len] = '\0';
    lastPassword[len] = '\0';

    clock_t start = clock();

    do {
        // Print the current password
        printf("%s\n", generatedPassword);

        // Increment the generated password
        for (int i = len - 1; i >= 0; i--) {
            if (generatedPassword[i] < TO_ORIGINAL + NUM_OF_ASCII - 1) {
                generatedPassword[i]++;
                break;
            } else {
                generatedPassword[i] = TO_ORIGINAL;
            }
        }
    } while (strcmp(generatedPassword, lastPassword) != 0);

    clock_t stop = clock();
    double duration = (double)(stop - start) / CLOCKS_PER_SEC * 1000;

    printf("Time taken by function: %.2f milliseconds\n", duration);

    return 0;
}
