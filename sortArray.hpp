#ifndef SORT_ARRAY_HPP
#define SORT_ARRAY_HPP

// Algorithm 1: Bubble Sort  O(n^2)
// Algorithm 2: Merge Sort   O(n log n)
// Sort keys: 1 = Age, 2 = Length of Stay, 3 = Total Medical Cost

#include "PatientArray.hpp"

using namespace std;

const int SORT_REPEAT = 250;

// Helpers

inline double getSortValue(const Patient& p, int key) {
    if (key == 1) return p.age;
    if (key == 2) return p.lengthOfStay;
    return totalCost(p);
}

inline const char* getKeyName(int key) {
    if (key == 1) return "Age";
    if (key == 2) return "Length of Stay";
    return "Total Cost";
}

inline bool isWrongOrder(const Patient& a, const Patient& b, int key, bool ascending) {
    if (ascending) return getSortValue(a, key) > getSortValue(b, key);
    return getSortValue(a, key) < getSortValue(b, key);
}

// Bubble Sort
inline void arrayBubbleSort(PatientArray& arr, int key, bool ascending,
                            long& comparisons, long& swaps) {
    int n = arr.size();
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        for (int i = 0; i < n - 1 - pass; i++) {
            comparisons++;
            if (isWrongOrder(arr[i], arr[i + 1], key, ascending)) {
                Patient temp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swaps++;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// Merge Sort

inline void arrayMerge(PatientArray& arr, Patient* temp, int left, int mid, int right,
                       int key, bool ascending, long& comparisons, long& moves) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        comparisons++;
        if (!isWrongOrder(arr[i], arr[j], key, ascending))
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
        moves++;
    }
    while (i <= mid)   { temp[k++] = arr[i++]; moves++; }
    while (j <= right) { temp[k++] = arr[j++]; moves++; }

    for (int x = left; x <= right; x++) {
        arr[x] = temp[x];
        moves++;
    }
}

inline void arrayMergeSortRec(PatientArray& arr, Patient* temp, int left, int right,
                              int key, bool ascending, long& comparisons, long& moves) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    arrayMergeSortRec(arr, temp, left, mid, key, ascending, comparisons, moves);
    arrayMergeSortRec(arr, temp, mid + 1, right, key, ascending, comparisons, moves);
    arrayMerge(arr, temp, left, mid, right, key, ascending, comparisons, moves);
}

inline void arrayMergeSort(PatientArray& arr, int key, bool ascending,
                           long& comparisons, long& moves) {
    if (arr.size() < 2) return;
    Patient* temp = new Patient[arr.size()];
    arrayMergeSortRec(arr, temp, 0, arr.size() - 1, key, ascending,
                      comparisons, moves);
    delete[] temp;
}

// Checking and displaying
inline bool arrayIsSorted(const PatientArray& arr, int key, bool ascending) {
    for (int i = 0; i < arr.size() - 1; i++)
        if (isWrongOrder(arr[i], arr[i + 1], key, ascending)) return false;
    return true;
}

inline void arrayShowFirst(const PatientArray& arr, int rows) {
    printPatientTableHeader();
    for (int i = 0; i < rows && i < arr.size(); i++)
        printPatientRow(i + 1, arr[i]);
    printLine(PATIENT_TABLE_WIDTH);
    cout << "(showing first " << rows << " of " << arr.size() << ")\n";
}

struct ArraySortResult {
    double timeUs;
    long   comparisons;
    long   swapsOrMoves;
    size_t extraMemory;
    bool   sorted;
};

// algorithm: 1 = Bubble, 2 = Merge
inline ArraySortResult runArraySort(PatientArray& source, int algorithm, int key,
                                    bool ascending, bool showRecords) {
    ArraySortResult r;
    r.comparisons = 0;
    r.swapsOrMoves = 0;
    r.sorted = true;

    // Copy before timing.
    PatientArray* copies[SORT_REPEAT];
    for (int i = 0; i < SORT_REPEAT; i++) copies[i] = source.clone();

    if (showRecords) {
        cout << "\nBEFORE sorting:\n";
        arrayShowFirst(*copies[0], 10);
    }

    Timer t;
    t.start();
    for (int i = 0; i < SORT_REPEAT; i++) {
        long c = 0, s = 0;
        if (algorithm == 1) arrayBubbleSort(*copies[i], key, ascending, c, s);
        else                arrayMergeSort(*copies[i], key, ascending, c, s);
        if (i == 0) { r.comparisons = c; r.swapsOrMoves = s; }
    }
    r.timeUs = t.elapsedUs() / SORT_REPEAT;

    for (int i = 0; i < SORT_REPEAT; i++)
        if (!arrayIsSorted(*copies[i], key, ascending)) r.sorted = false;

    if (showRecords) {
        cout << "\nAFTER sorting by " << getKeyName(key)
             << (ascending ? " (ascending):\n" : " (descending):\n");
        arrayShowFirst(*copies[0], 10);
    }

    if (algorithm == 1) r.extraMemory = sizeof(Patient);
    else                r.extraMemory = source.size() * sizeof(Patient);

    for (int i = 0; i < SORT_REPEAT; i++) delete copies[i];
    return r;
}

inline void printArraySortHeader() {
    cout << "Est. = Estimated. Benchmark copies and allocation overhead are excluded.\n"
         << "Extra Mem counts temporary Patient storage; recursion is excluded.\n";
    printLine(116);
    cout << left << setw(10) << "Structure"
         << setw(10) << "Algorithm"
         << setw(16) << "Sort Key"
         << right << setw(14) << "Time (us)"
         << setw(14) << "Comparisons"
         << setw(14) << "Swaps/Moves"
         << setw(12) << "Data Mem"
         << setw(18) << "Extra Mem (Est.)"
         << setw(8)  << "Sorted?" << "\n";
    printLine(116);
}

inline void printArraySortRow(const char* algoName, int key, const ArraySortResult& r,
                              size_t dataMemory) {
    cout << left << setw(10) << "Array"
         << setw(10) << algoName
         << setw(16) << getKeyName(key)
         << right << fixed << setprecision(2)
         << setw(14) << r.timeUs
         << setw(14) << r.comparisons
         << setw(14) << r.swapsOrMoves
         << setw(12) << formatBytes(dataMemory)
         << setw(18) << formatBytes(r.extraMemory)
         << setw(8)  << (r.sorted ? "YES" : "NO") << "\n";
}

inline void printSpeedup(const ArraySortResult& bubble, const ArraySortResult& merge) {
    if (merge.timeUs > 0)
        cout << ">> Merge Sort was " << fixed << setprecision(1)
             << bubble.timeUs / merge.timeUs << "x faster and used "
             << bubble.comparisons - merge.comparisons
             << " fewer comparisons than Bubble Sort.\n";
}

// Menu
inline int arrayReadSortingChoice(int maxChoice) {
    while (true) {
        cout << "Enter choice: ";
        int choice = readInt();
        if (choice >= 0 && choice <= maxChoice) return choice;
        cout << "\nInvalid choice, please enter 0-" << maxChoice << ".\n";
    }
}

inline void arrayViewAllSortedRecords(const PatientArray& source, int algorithm,
                                     int key, bool ascending) {
    cout << "\n  1. View all sorted records (20 per page)\n"
         << "  0. Back\n";
    int choice = arrayReadSortingChoice(1);
    switch (choice) {
    case 1: {
        PatientArray* records = source.clone();
        long comparisons = 0, moves = 0;
        if (algorithm == 1)
            arrayBubbleSort(*records, key, ascending, comparisons, moves);
        else
            arrayMergeSort(*records, key, ascending, comparisons, moves);

        cout << "\nALL RECORDS sorted by " << getKeyName(key)
             << (ascending ? " (ascending):\n" : " (descending):\n");
        records->displayAll(20);
        delete records;
        waitForEnter();
        break;
    }
    case 0:
        break;
    }
}

inline void arrayCombineDatasets(const PatientArray datasets[], PatientArray& combined) {
    combined.clear();
    for (int d = 0; d < NUM_DATASETS; d++) {
        for (int i = 0; i < datasets[d].size(); i++)
            combined.insertAtEnd(datasets[d][i]);
    }
}

inline void arraySortingMenu(PatientArray datasets[]) {
    int choice;
    do {
        cout << "\n";
        printLine(50, '=');
        cout << "   Sorting Experiments & Benchmark [ARRAY]\n";
        printLine(50, '=');
        cout << "  1. Bubble Sort (show before/after)\n"
             << "  2. Merge Sort (show before/after)\n"
             << "  3. Compare Bubble Sort vs Merge Sort\n"
             << "  4. Full benchmark (facilities + combined)\n"
             << "  0. Back\n";
        printLine(50, '=');
        choice = arrayReadSortingChoice(4);

        switch (choice) {
        case 1:
        case 2:
        case 3: {
            cout << "\nSelect dataset:\n";
            for (int d = 0; d < NUM_DATASETS; d++)
                cout << "  " << (d + 1) << ". " << DATASET_NAMES[d] << "\n";
            int combinedCount = 0;
            for (int d = 0; d < NUM_DATASETS; d++)
                combinedCount += datasets[d].size();
            cout << "  " << (NUM_DATASETS + 1) << ". All facilities combined ("
                 << combinedCount << " records)\n";
            cout << "  0. Back\n";
            int d = arrayReadSortingChoice(NUM_DATASETS + 1);
            if (d == 0) continue;

            cout << "\nSelect sorting criterion:\n"
                 << "  1. Age\n"
                 << "  2. Visit Duration (Length of Stay)\n"
                 << "  3. Total Medical Cost\n"
                 << "  0. Back\n";
            int key = arrayReadSortingChoice(3);
            if (key == 0) continue;

            cout << "\nSelect sorting order:\n"
                 << "  1. Ascending\n"
                 << "  2. Descending\n"
                 << "  0. Back\n";
            int order = arrayReadSortingChoice(2);
            if (order == 0) continue;
            bool asc = (order == 1);
            PatientArray combined;
            if (d == NUM_DATASETS + 1) arrayCombineDatasets(datasets, combined);
            PatientArray& data = (d == NUM_DATASETS + 1) ? combined : datasets[d - 1];
            const char* datasetName = (d == NUM_DATASETS + 1)
                ? "All facilities combined" : DATASET_NAMES[d - 1];
            cout << "\nDataset: " << datasetName << " (" << data.size() << " records)\n";

            if (choice == 1 || choice == 2) {
                ArraySortResult r = runArraySort(data, choice, key, asc, true);
                cout << "\nPERFORMANCE (average of " << SORT_REPEAT << " runs)\n";
                printArraySortHeader();
                printArraySortRow(choice == 1 ? "Bubble" : "Merge", key, r,
                                  data.memoryBytes());
                printLine(116);
                arrayViewAllSortedRecords(data, choice, key, asc);
                continue;
            } else {
                ArraySortResult b = runArraySort(data, 1, key, asc, false);
                ArraySortResult m = runArraySort(data, 2, key, asc, false);
                cout << "\nBUBBLE vs MERGE on " << datasetName
                     << " (average of " << SORT_REPEAT << " runs)\n";
                printArraySortHeader();
                printArraySortRow("Bubble", key, b, data.memoryBytes());
                printArraySortRow("Merge",  key, m, data.memoryBytes());
                printLine(116);
                printSpeedup(b, m);
            }
            break;
        }
        case 4: {
            cout << "\nFULL BENCHMARK [ARRAY] (ascending, average of "
                 << SORT_REPEAT << " runs)\n";
            printArraySortHeader();
            PatientArray combined;
            arrayCombineDatasets(datasets, combined);
            for (int d = 0; d <= NUM_DATASETS; d++) {
                PatientArray& data = (d == NUM_DATASETS) ? combined : datasets[d];
                const char* datasetName = (d == NUM_DATASETS)
                    ? "All facilities combined" : DATASET_NAMES[d];
                cout << datasetName << " (" << data.size() << " records)\n";
                for (int key = 1; key <= 3; key++) {
                    ArraySortResult b = runArraySort(data, 1, key, true, false);
                    ArraySortResult m = runArraySort(data, 2, key, true, false);
                    printArraySortRow("Bubble", key, b, data.memoryBytes());
                    printArraySortRow("Merge",  key, m, data.memoryBytes());
                }
            }
            printLine(116);
            break;
        }
        case 0:
            break;
        }
        if (choice != 0) waitForEnter();
    } while (choice != 0);
}

#endif
