// ============================================================================
// ListProgram.cpp
// CT077-3-2-DSTR Lab Work #1 - MetroHealth Patient Management System
// PROGRAM 2 of 2: SINGLY LINKED LIST IMPLEMENTATION ONLY (Strictly List-based).
//
// Compile:  g++ -std=c++11 -Wall -o ListProgram ListProgram.cpp
// Run from the directory containing the dataset CSV files:
//   dataset1_facility_a.csv
//   dataset2_facility_b.csv
//   dataset3_facility_c.csv
// ============================================================================

#include "common.hpp"
#include "PatientList.hpp"
#include "AgeGroup_List.hpp"
#include "sortList.hpp"
#include "Search_List.hpp"
#include "expenditure_list.hpp"

using namespace std;

// The three datasets, one linked list per facility
PatientList datasets[NUM_DATASETS];
bool        loaded = false;

// ---------------------------------------------------------------------------
// Loads all three CSV files into three lists and prints a summary table
// ---------------------------------------------------------------------------
void loadAllDatasets() {
    if (loaded) {
        cout << "\nDatasets are already loaded.\n";
        return;
    }

    cout << "\nLoading datasets into LINKED LISTS...\n\n";
    printLine(100);
    cout << left << setw(40) << "Dataset"
         << setw(10) << "Records" << setw(10) << "Skipped"
         << setw(12) << "Node Size"
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
             << setw(12) << sizeof(PatientNode)
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
    cout << "\n=== " << DATASET_NAMES[d] << " (LINKED LIST) ===\n";
    datasets[d].displayAll(50);          // pause every 50 rows
}

// ---------------------------------------------------------------------------
// Memory report: list memory = size x sizeof(PatientNode)
// ---------------------------------------------------------------------------
void showMemoryReport() {
    cout << "\nLINKED LIST MEMORY ESTIMATE  (sizeof(PatientNode) = "
         << sizeof(PatientNode) << " bytes = " << sizeof(Patient)
         << " data + " << (sizeof(PatientNode) - sizeof(Patient))
         << " pointer/padding)\n";
    printLine(100);
    cout << left << setw(40) << "Dataset"
         << setw(10) << "Nodes"
         << right << setw(14) << "Total"
         << setw(14) << "Data only"
         << setw(14) << "Overhead" << "\n";
    printLine(100);

    size_t total = 0;
    for (int i = 0; i < NUM_DATASETS; i++) {
        cout << left << setw(40) << DATASET_NAMES[i]
             << setw(10) << datasets[i].size()
             << right
             << setw(14) << formatBytes(datasets[i].memoryBytes())
             << setw(14) << formatBytes(datasets[i].dataBytes())
             << setw(14) << formatBytes(datasets[i].overheadBytes()) << "\n";
        total += datasets[i].memoryBytes();
    }
    printLine(100);
    cout << "Total list memory (all nodes): " << formatBytes(total) << "\n";
}

// ---------------------------------------------------------------------------
// Task 4: Patient Demographic & Billing Categorization menu handler
// ---------------------------------------------------------------------------
void demographicBillingMenu_List() {
    int d = chooseDataset();
    if (d < 0) return;
    runTask4_List(datasets[d], DATASET_NAMES[d]);
}

// ---------------------------------------------------------------------------
// Healthcare Expenditure & Service Analysis menu handler
// ---------------------------------------------------------------------------
void healthcareExpenditureMenu_List() {
    lstRunServiceAnalysisMenu(datasets);
}

// ---------------------------------------------------------------------------
// Clinical Insights & Recommendations Summary handler
// ---------------------------------------------------------------------------
void clinicalInsightsMenu_List() {
    displayClinicalInsightsReportList(datasets);
    waitForEnter();
}

// ---------------------------------------------------------------------------
// Main Menu
// ---------------------------------------------------------------------------
void showMenu() {
    cout << "\n==================================================\n"
         << " MetroHealth Patient Management [LINKED LIST]     \n"
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

        // Every option except Load/Exit needs the data in memory first
        if (choice >= 2 && choice <= 8 && !loaded) {
            cout << "\nPlease load the datasets first (Option 1).\n";
            continue;
        }

        switch (choice) {
            case 1: loadAllDatasets(); break;
            case 2: displayDataset(); break;
            case 3: showMemoryReport(); break;
            case 4: demographicBillingMenu_List(); break;
            case 5: healthcareExpenditureMenu_List(); break;
            case 6: listSortingMenu(datasets); break;
            case 7: listSearchingMenu(datasets); break;
            case 8: clinicalInsightsMenu_List(); break;
            case 0: cout << "\nExiting MetroHealth [LINKED LIST]. Goodbye!\n"; break;
            default: cout << "\nInvalid choice, please enter 0-8.\n";
        }
    } while (choice != 0);

    return 0;      // PatientList destructors free all heap memory automatically
}
