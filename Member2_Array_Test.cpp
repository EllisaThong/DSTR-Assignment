// =====================================================================
// Member2_Array_Test.cpp  --  runs ONLY the Patient Demographic & Billing
// part (Array version) so it can be tested and screenshotted on its own.
// Not part of the submission: the team's ArrayProgram.cpp calls
// runTask4_Array() from its main menu.
//
// Compile:  g++ -std=c++17 Member2_Array_Test.cpp -o Member2_Array_Test
// =====================================================================
#include "common.hpp"
#include "PatientArray.hpp"
#include "AgeGroup_Array.hpp"
using namespace std;

int main() {
    PatientArray datasets[NUM_DATASETS];

    // Load the 3 CSV files (they must be in the same folder)
    for (int i = 0; i < NUM_DATASETS; i++) {
        int skipped = 0;
        int n = datasets[i].loadFromCSV(DATASET_FILES[i], skipped);
        if (n < 0) cout << "ERROR: cannot open " << DATASET_FILES[i] << endl;
        else       cout << "Loaded " << n << " patients from " << DATASET_FILES[i] << endl;
    }

    int choice;
    do {
        cout << endl << "Select dataset:" << endl;
        for (int i = 0; i < NUM_DATASETS; i++)
            cout << "  " << (i + 1) << ". " << DATASET_NAMES[i] << endl;
        cout << "  0. Exit" << endl << "Choice: ";
        choice = readInt();
        if (choice >= 1 && choice <= NUM_DATASETS)
            runTask4_Array(datasets[choice - 1], DATASET_NAMES[choice - 1]);
        else if (choice != 0)
            cout << "Invalid choice." << endl;
    } while (choice != 0);
    return 0;
}
