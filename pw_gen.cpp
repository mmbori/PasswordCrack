#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

#define TO_ORIGINAL 33
#define NUM_OF_ASCII 94

int main() {
    int len = 4;
    string generatedPassword(len, char(TO_ORIGINAL));
    string lastPassword(len, char(TO_ORIGINAL + NUM_OF_ASCII - 1));

    auto start = high_resolution_clock::now();

    while (generatedPassword != lastPassword) {
        // Increment the generated password
        for (int i = len - 1; i >= 0; i--) {
            if (generatedPassword[i] < char(TO_ORIGINAL + NUM_OF_ASCII - 1)) {
                generatedPassword[i]++;
                break;
            } else {
                generatedPassword[i] = char(TO_ORIGINAL);
            }
        }
    }

    // Print the last password
    cout << generatedPassword << endl;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;

    return 0;
}
