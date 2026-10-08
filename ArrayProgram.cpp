// ============================================================================
// ArrayProgram.cpp
// CT077-3-2-DSTR Lab Work #1 - MetroHealth Patient Management System
// PROGRAM 1 of 2: DYNAMIC ARRAY IMPLEMENTATION ONLY (Strictly Array-based).
//
// Compile:  g++ -std=c++11 -Wall -o ArrayProgram ArrayProgram.cpp
// Run from the directory containing the dataset CSV files:
//   dataset1_facility_a.csv
//   dataset2_facility_b.csv
//   dataset3_facility_c.csv
// ============================================================================

#include "common.hpp"
#include "PatientArray.hpp"
#include "AgeGroup_Array.hpp"
#include "sortArray.hpp"
#include "Search_Array.hpp"
#include "expenditure_array.hpp"

using namespace std;

// The three datasets, one dynamic array per facility
PatientArray datasets[NUM_DATASETS];
bool         loaded = false;

// ---------------------------------------------------------------------------
// Loads all three CSV files into three arrays and prints a summary table
// ---------------------------------------------------------------------------
void loadAllDatasets() {
    if (loaded) {
        cout << "\nDatasets are already loaded.\n";
        return;
    }

    cout << "\nLoading datasets into ARRAYS...\n\n";
    printLine(100);
    cout << left << setw(40) << "Dataset"
         << setw(10) << "Records" << setw(10) << "Skipped"
         << setw(10) << "Resizes" << setw(12) << "Capacity"
         << right << setw(18) << "Load Time (ms)" << "\n";
    printLine(100);

    bool allOk = true;
    for (int i = 0; i < NUM_DATASETS; i++) {
        int skipped = 0;
        Timer t;
        t.start();
        int n = datasets[i].loadFromCSV(DATASET_FILES[i], skipped);
        double ms = t.elapsedMs();

        if (n < 0) {
            cout << left << setw(40) << DATASET_NAMES[i]
                 << "ERROR: cannot open " << DATASET_FILES[i] << "\n";
            allOk = false;
            continue;
        }
        cout << left << setw(40) << DATASET_NAMES[i]
             << setw(10) << n << setw(10) << skipped
             << setw(10) << datasets[i].resizes()
             << setw(12) << datasets[i].capacity()
             << right << fixed << setprecision(4)
             << setw(18) << ms << "\n";
    }
    printLine(100);
    loaded = allOk;
    if (!allOk)
        cout << "Some files failed to load. Place the CSV files in the current folder.\n";
}

// ---------------------------------------------------------------------------
// Asks which dataset to use; returns 0..2, or -1 if the choice is invalid
// ---------------------------------------------------------------------------
int chooseDataset() {
    cout << "\nSelect dataset:\n";
    for (int i = 0; i < NUM_DATASETS; i++)
        cout << "  " << (i + 1) << ". " << DATASET_NAMES[i] << "\n";
    cout << "Choice: ";
    int c = readInt();
    if (c < 1 || c > NUM_DATASETS) {
        cout << "Invalid dataset choice.\n";
        return -1;
    }
    return c - 1;
}

void displayDataset() {
    int d = chooseDataset();
    if (d < 0) return;
    cout << "\n=== " << DATASET_NAMES[d] << " (ARRAY) ===\n";
    datasets[d].displayAll(50);          // pause every 50 rows
}

// ---------------------------------------------------------------------------
// Memory report: allocated vs used vs wasted array capacity
// ---------------------------------------------------------------------------
void showMemoryReport() {
    cout << "\nARRAY MEMORY ESTIMATE  (sizeof(Patient) = "
         << sizeof(Patient) << " bytes)\n";
    printLine(100);
    cout << left << setw(40) << "Dataset"
         << setw(10) << "Size" << setw(10) << "Capacity"
         << right << setw(14) << "Allocated"
         << setw(14) << "Used"
         << setw(12) << "Wasted" << "\n";
    printLine(100);

    size_t total = 0;
    for (int i = 0; i < NUM_DATASETS; i++) {
        cout << left << setw(40) << DATASET_NAMES[i]
             << setw(10) << datasets[i].size()
             << setw(10) << datasets[i].capacity()
             << right
             << setw(14) << formatBytes(datasets[i].memoryBytes())
             << setw(14) << formatBytes(datasets[i].usedBytes())
             << setw(12) << formatBytes(datasets[i].wastedBytes()) << "\n";
        total += datasets[i].memoryBytes();
    }
    printLine(100);
    cout << "Total array memory allocated: " << formatBytes(total) << "\n";
}

// ---------------------------------------------------------------------------
// Task 4: Patient Demographic & Billing Categorization menu handler
// ---------------------------------------------------------------------------
void demographicBillingMenu_Array() {
    int d = chooseDataset();
    if (d < 0) return;
    runTask4_Array(datasets[d], DATASET_NAMES[d]);
}

// ---------------------------------------------------------------------------
// Healthcare Expenditure & Service Analysis menu handler
// ---------------------------------------------------------------------------
void healthcareExpenditureMenu_Array() {
    expRunServiceAnalysisMenu(datasets);
}

// ---------------------------------------------------------------------------
// Clinical Insights & Recommendations Summary handler
// ---------------------------------------------------------------------------
void clinicalInsightsMenu_Array() {
    displayClinicalInsightsReportArray(datasets);
    waitForEnter();
}

// ---------------------------------------------------------------------------
// Main Menu
// ---------------------------------------------------------------------------
void showMenu() {
    cout << "\n==================================================\n"
         << "   MetroHealth Patient Management System [ARRAY]  \n"
         << "==================================================\n"
         << "  1. Load all datasets into memory\n"
         << "  2. Display all patient records\n"
         << "  3. Memory consumption & overhead report\n"
         << "  4. Patient demographic & billing categorization\n"
         << "  5. Healthcare expenditure & service analysis\n"
         << "  6. Sorting experiments & benchmark\n"
         << "  7. Searching experiments & benchmark\n"
         << "  8. Clinical insights & recommendations summary\n"
         << "  0. Exit\n"
         << "==================================================\n"
         << "Enter choice: ";
}

int main() {
    int choice;
    do {
        showMenu();
        choice = readInt();

        if (choice >= 2 && choice <= 8 && !loaded) {
            cout << "\nPlease load the datasets first (Option 1).\n";
            continue;
        }

        switch (choice) {
            case 1: loadAllDatasets(); break;
            case 2: displayDataset(); break;
            case 3: showMemoryReport(); break;
            case 4: demographicBillingMenu_Array(); break;
            case 5: healthcareExpenditureMenu_Array(); break;
            case 6: arraySortingMenu(datasets); break;
            case 7: arraySearchingMenu(datasets); break;
            case 8: clinicalInsightsMenu_Array(); break;
            case 0: cout << "\nExiting MetroHealth [ARRAY]. Goodbye!\n"; break;
            default: cout << "\nInvalid choice, please enter 0-8.\n";
        }
    } while (choice != 0);

    return 0;
}
