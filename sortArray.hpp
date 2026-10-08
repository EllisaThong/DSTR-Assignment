#ifndef SORT_ARRAY_HPP
#define SORT_ARRAY_HPP

// ============================================================================
// Sort_Array.hpp  -  Member 4, Task 6: Sorting experiments (ARRAY version)
//
// Algorithm 1: Bubble Sort  O(n^2)      - compare neighbours, swap if wrong order
// Algorithm 2: Merge Sort   O(n log n)  - split in half, sort halves, merge
//
// Sort keys: 1 = Age, 2 = Length of Stay, 3 = Total Medical Cost
// No <vector>, <algorithm> or std::sort is used.
// ============================================================================

#include "PatientArray.hpp"

using namespace std;

const int SORT_REPEAT = 250;   // average many runs so the timer does not show 0

// ---------------------------------------------------------------------------
// Helpers shared by both algorithms
// ---------------------------------------------------------------------------

// Returns the value we are sorting by, so one compare works for all 3 keys
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

// true if a should come AFTER b (wrong order -> needs swapping)
inline bool isWrongOrder(const Patient& a, const Patient& b, int key, bool ascending) {
    if (ascending) return getSortValue(a, key) > getSortValue(b, key);
    return getSortValue(a, key) < getSortValue(b, key);
}

// ---------------------------------------------------------------------------
// Bubble Sort on the array
// ---------------------------------------------------------------------------
inline void arrayBubbleSort(PatientArray& arr, int key, bool ascending,
                            long& comparisons, long& swaps) {
    int n = arr.size();
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;                       // early-exit flag
        for (int i = 0; i < n - 1 - pass; i++) {
            comparisons++;
            if (isWrongOrder(arr[i], arr[i + 1], key, ascending)) {
                Patient temp = arr[i];              // swap the two records
                arr[i] = arr[i + 1];
                arr[i + 1] = temp;
                swaps++;
                swapped = true;
            }
        }
        if (!swapped) break;    // no swap in a whole pass = already sorted
    }
}

// ---------------------------------------------------------------------------
// Merge Sort on the array
// ---------------------------------------------------------------------------

// Merge two sorted parts arr[left..mid] and arr[mid+1..right]
inline void arrayMerge(PatientArray& arr, Patient* temp, int left, int mid, int right,
                       int key, bool ascending, long& comparisons, long& moves) {
    int i = left;         // index in left half
    int j = mid + 1;      // index in right half
    int k = left;         // index in temp

    while (i <= mid && j <= right) {
        comparisons++;
        // take from the left half when equal -> stable sort
        if (!isWrongOrder(arr[i], arr[j], key, ascending))
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
        moves++;
    }
    while (i <= mid)   { temp[k++] = arr[i++]; moves++; }   // leftovers
    while (j <= right) { temp[k++] = arr[j++]; moves++; }

    for (int x = left; x <= right; x++) {                   // copy back
        arr[x] = temp[x];
        moves++;
    }
}

inline void arrayMergeSortRec(PatientArray& arr, Patient* temp, int left, int right,
                              int key, bool ascending, long& comparisons, long& moves) {
    if (left >= right) return;                  // 0 or 1 element: sorted
    int mid = (left + right) / 2;
    arrayMergeSortRec(arr, temp, left, mid, key, ascending, comparisons, moves);
    arrayMergeSortRec(arr, temp, mid + 1, right, key, ascending, comparisons, moves);
    arrayMerge(arr, temp, left, mid, right, key, ascending, comparisons, moves);
}

inline void arrayMergeSort(PatientArray& arr, int key, bool ascending,
                           long& comparisons, long& moves) {
    if (arr.size() < 2) return;
    Patient* temp = new Patient[arr.size()];    // extra O(n) memory
    arrayMergeSortRec(arr, temp, 0, arr.size() - 1, key, ascending,
                      comparisons, moves);
    delete[] temp;
}

// ---------------------------------------------------------------------------
// Checking and displaying
// ---------------------------------------------------------------------------
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

// One row of results for the performance table
struct ArraySortResult {
    double timeUs;          // average time of one sort
    long   comparisons;
    long   swapsOrMoves;
    size_t extraMemory;     // extra memory the algorithm needs
    bool   sorted;
};

// algorithm: 1 = Bubble, 2 = Merge
inline ArraySortResult runArraySort(PatientArray& source, int algorithm, int key,
                                    bool ascending, bool showRecords) {
    ArraySortResult r;
    r.comparisons = 0;
    r.swapsOrMoves = 0;
    r.sorted = true;

    // Make the copies BEFORE timing, so copying time is not counted
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
        if (i == 0) { r.comparisons = c; r.swapsOrMoves = s; }  // same every run
    }
    r.timeUs = t.elapsedUs() / SORT_REPEAT;

    for (int i = 0; i < SORT_REPEAT; i++)
        if (!arrayIsSorted(*copies[i], key, ascending)) r.sorted = false;

    if (showRecords) {
        cout << "\nAFTER sorting by " << getKeyName(key)
             << (ascending ? " (ascending):\n" : " (descending):\n");
        arrayShowFirst(*copies[0], 10);
    }

    // Bubble needs only one temp Patient; Merge needs a temp array of n
    if (algorithm == 1) r.extraMemory = sizeof(Patient);
    else                r.extraMemory = source.size() * sizeof(Patient);

    for (int i = 0; i < SORT_REPEAT; i++) delete copies[i];
    return r;
}

inline void printArraySortHeader() {
    printLine(110);
    cout << left << setw(10) << "Structure"
         << setw(10) << "Algorithm"
         << setw(16) << "Sort Key"
         << right << setw(14) << "Time (us)"
         << setw(14) << "Comparisons"
         << setw(14) << "Swaps/Moves"
         << setw(12) << "Data Mem"
         << setw(12) << "Extra Mem"
         << setw(8)  << "Sorted?" << "\n";
    printLine(110);
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
         << setw(12) << formatBytes(r.extraMemory)
         << setw(8)  << (r.sorted ? "YES" : "NO") << "\n";
}

inline void printSpeedup(const ArraySortResult& bubble, const ArraySortResult& merge) {
    if (merge.timeUs > 0)
        cout << ">> Merge Sort was " << fixed << setprecision(1)
             << bubble.timeUs / merge.timeUs << "x faster and used "
             << bubble.comparisons - merge.comparisons
             << " fewer comparisons than Bubble Sort.\n";
}

// ---------------------------------------------------------------------------
// Menu (called from ArrayProgram.cpp)
// ---------------------------------------------------------------------------
inline void arraySortingMenu(PatientArray datasets[]) {
    int choice;
    do {
        cout << "\n------ SORTING EXPERIMENTS [ARRAY] ------\n"
             << "  1. Bubble Sort (show before/after)\n"
             << "  2. Merge Sort  (show before/after)\n"
             << "  3. Compare Bubble vs Merge\n"
             << "  4. Full benchmark (all datasets, all keys)\n"
             << "  0. Back\n"
             << "Choice: ";
        choice = readInt();

        if (choice >= 1 && choice <= 3) {
            cout << "\nDataset (1-3): ";
            int d = readInt();
            cout << "Sort by 1.Age  2.Length of Stay  3.Total Cost: ";
            int key = readInt();
            cout << "Order 1.Ascending  2.Descending: ";
            bool asc = (readInt() != 2);
            if (d < 1 || d > 3 || key < 1 || key > 3) {
                cout << "Invalid input.\n";
                continue;
            }
            PatientArray& data = datasets[d - 1];

            if (choice == 1 || choice == 2) {
                ArraySortResult r = runArraySort(data, choice, key, asc, true);
                cout << "\nPERFORMANCE (average of " << SORT_REPEAT << " runs)\n";
                printArraySortHeader();
                printArraySortRow(choice == 1 ? "Bubble" : "Merge", key, r,
                                  data.memoryBytes());
                printLine(110);
            } else {
                ArraySortResult b = runArraySort(data, 1, key, asc, false);
                ArraySortResult m = runArraySort(data, 2, key, asc, false);
                cout << "\nBUBBLE vs MERGE on " << DATASET_NAMES[d - 1]
                     << " (average of " << SORT_REPEAT << " runs)\n";
                printArraySortHeader();
                printArraySortRow("Bubble", key, b, data.memoryBytes());
                printArraySortRow("Merge",  key, m, data.memoryBytes());
                printLine(110);
                printSpeedup(b, m);
            }
        } else if (choice == 4) {
            cout << "\nFULL BENCHMARK [ARRAY] (ascending, average of "
                 << SORT_REPEAT << " runs)\n";
            printArraySortHeader();
            for (int d = 0; d < NUM_DATASETS; d++) {
                cout << DATASET_NAMES[d] << "\n";
                for (int key = 1; key <= 3; key++) {
                    ArraySortResult b = runArraySort(datasets[d], 1, key, true, false);
                    ArraySortResult m = runArraySort(datasets[d], 2, key, true, false);
                    printArraySortRow("Bubble", key, b, datasets[d].memoryBytes());
                    printArraySortRow("Merge",  key, m, datasets[d].memoryBytes());
                }
            }
            printLine(110);
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

#endif
