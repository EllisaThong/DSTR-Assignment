#ifndef SEARCH_ARRAY_HPP
#define SEARCH_ARRAY_HPP

// ============================================================================
// Search_Array.hpp - Member 5, Task 7: Searching experiments (ARRAY version)
//
// 1. Linear Search on Unsorted Array: O(n)
//    - Search by Age Range / Age Group
//    - Search by Specific Age
//    - Search by Care Type
//    - Search by Visit Duration threshold (> X hours)
//
// 2. Binary Search on Sorted Array (by Age): O(log n)
//    - LowerBound and UpperBound to locate age range [minAge, maxAge]
//    - Exact age search
//
// 3. Performance Benchmark:
//    - Compares execution time (us), comparisons, and memory overhead.
// ============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include "PatientArray.hpp"
#include "sortArray.hpp"

using namespace std;

struct SearchPerformanceResult {
    string searchType;
    string algorithm;
    double timeUs;
    int    comparisons;
    int    matchesFound;
    size_t memoryBytes;
};

// ===================== Helper Display =====================
inline void printSearchResultsHeader() {
    printLine(PATIENT_TABLE_WIDTH);
    cout << left
         << setw(6)  << "No."
         << setw(11) << "Patient ID"
         << setw(5)  << "Age"
         << setw(9)  << "Group"
         << setw(17) << "Care Type"
         << setw(10) << "Stay (h)"
         << setw(13) << "Rate (MYR/h)"
         << setw(10) << "Visits/yr"
         << right << setw(18) << "Total Cost (MYR)" << "\n";
    printLine(PATIENT_TABLE_WIDTH);
}

// ===================== Linear Search on Array =====================
inline int linearSearchAgeRange_Array(const PatientArray& arr, int minAge, int maxAge,
                                     int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printSearchResultsHeader();

    for (int i = 0; i < arr.size(); i++) {
        comparisons++;
        if (arr[i].age >= minAge && arr[i].age <= maxAge) {
            matches++;
            if (display) printPatientRow(matches, arr[i]);
        }
    }
    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

inline int linearSearchCareType_Array(const PatientArray& arr, const string& careType,
                                      int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printSearchResultsHeader();

    for (int i = 0; i < arr.size(); i++) {
        comparisons++;
        if (careType == arr[i].careType) {
            matches++;
            if (display) printPatientRow(matches, arr[i]);
        }
    }
    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

inline int linearSearchDuration_Array(const PatientArray& arr, double minHours,
                                      int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printSearchResultsHeader();

    for (int i = 0; i < arr.size(); i++) {
        comparisons++;
        if (arr[i].lengthOfStay >= minHours) {
            matches++;
            if (display) printPatientRow(matches, arr[i]);
        }
    }
    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

// ===================== Binary Search on Sorted Array =====================
inline int lowerBoundAge_Array(const PatientArray& arr, int targetAge, int& comparisons) {
    int low = 0;
    int high = arr.size() - 1;
    int result = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        comparisons++;
        if (arr[mid].age >= targetAge) {
            result = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    return result;
}

inline int upperBoundAge_Array(const PatientArray& arr, int targetAge, int& comparisons) {
    int low = 0;
    int high = arr.size() - 1;
    int result = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        comparisons++;
        if (arr[mid].age <= targetAge) {
            result = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

inline int binarySearchAgeRange_Array(const PatientArray& sortedArr, int minAge, int maxAge,
                                      int& comparisons, bool display) {
    comparisons = 0;
    int first = lowerBoundAge_Array(sortedArr, minAge, comparisons);
    int last  = upperBoundAge_Array(sortedArr, maxAge, comparisons);

    if (first == -1 || last == -1 || first > last) {
        if (display) cout << "No patients found in age range [" << minAge << " - " << maxAge << "].\n";
        return 0;
    }

    int matches = last - first + 1;
    if (display) {
        printSearchResultsHeader();
        int row = 0;
        for (int i = first; i <= last; i++) {
            row++;
            printPatientRow(row, sortedArr[i]);
        }
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

// ===================== Performance Benchmark =====================
inline void printSearchPerformanceTable(const SearchPerformanceResult results[], int count) {
    printLine(90);
    cout << left << setw(28) << "Query Type"
         << setw(16) << "Algorithm"
         << right << setw(12) << "Time (us)"
         << setw(14) << "Comparisons"
         << setw(10) << "Matches"
         << setw(10) << "Memory" << "\n";
    printLine(90);

    for (int i = 0; i < count; i++) {
        cout << left << setw(28) << results[i].searchType
             << setw(16) << results[i].algorithm
             << right << fixed << setprecision(2)
             << setw(12) << results[i].timeUs
             << setw(14) << results[i].comparisons
             << setw(10) << results[i].matchesFound
             << setw(10) << formatBytes(results[i].memoryBytes) << "\n";
    }
    printLine(90);
}

// ===================== Interactive Search Menu =====================
inline void arraySearchingMenu(PatientArray datasets[]) {
    int choice;
    do {
        cout << "\n------ SEARCHING EXPERIMENTS [ARRAY] ------\n"
             << "  1. Search by Age Range (Linear Search - Unsorted)\n"
             << "  2. Search by Age Range (Binary Search - Sorted by Age)\n"
             << "  3. Search by Specific Care Type (Linear Search)\n"
             << "  4. Search by Visit Duration Threshold (Linear Search)\n"
             << "  5. Compare Linear vs Binary Search Performance\n"
             << "  0. Back\n"
             << "Choice: ";
        choice = readInt();

        if (choice >= 1 && choice <= 4) {
            cout << "\nDataset (1-3): ";
            int d = readInt();
            if (d < 1 || d > 3) {
                cout << "Invalid dataset.\n";
                continue;
            }
            PatientArray& data = datasets[d - 1];

            if (choice == 1) {
                cout << "Enter Minimum Age (0-100): ";
                int minAge = readInt();
                cout << "Enter Maximum Age (0-100): ";
                int maxAge = readInt();
                if (minAge < 0 || maxAge > 100 || minAge > maxAge) {
                    cout << "Invalid age range.\n";
                    continue;
                }
                int comparisons = 0;
                Timer t;
                t.start();
                int matches = linearSearchAgeRange_Array(data, minAge, maxAge, comparisons, true);
                double us = t.elapsedUs();
                cout << "\n[Performance] Time: " << fixed << setprecision(2) << us
                     << " us | Comparisons: " << comparisons << " | Matches: " << matches << "\n";
            } else if (choice == 2) {
                cout << "Enter Minimum Age (0-100): ";
                int minAge = readInt();
                cout << "Enter Maximum Age (0-100): ";
                int maxAge = readInt();
                if (minAge < 0 || maxAge > 100 || minAge > maxAge) {
                    cout << "Invalid age range.\n";
                    continue;
                }
                // Prepare sorted copy by Age
                PatientArray sortedCopy;
                for (int i = 0; i < data.size(); i++) sortedCopy.insertAtEnd(data[i]);
                long dummyC = 0, dummyM = 0;
                arrayMergeSort(sortedCopy, 1, true, dummyC, dummyM);

                int comparisons = 0;
                Timer t;
                t.start();
                int matches = binarySearchAgeRange_Array(sortedCopy, minAge, maxAge, comparisons, true);
                double us = t.elapsedUs();
                cout << "\n[Performance] Time: " << fixed << setprecision(2) << us
                     << " us | Comparisons: " << comparisons << " | Matches: " << matches << "\n";
            } else if (choice == 3) {
                cout << "Select Care Type:\n";
                for (int c = 0; c < NUM_CARE_TYPES; c++)
                    cout << "  " << (c + 1) << ". " << CARE_TYPES[c] << "\n";
                cout << "Choice: ";
                int c = readInt();
                if (c < 1 || c > NUM_CARE_TYPES) {
                    cout << "Invalid care type.\n";
                    continue;
                }
                string selectedCare = CARE_TYPES[c - 1];
                int comparisons = 0;
                Timer t;
                t.start();
                int matches = linearSearchCareType_Array(data, selectedCare, comparisons, true);
                double us = t.elapsedUs();
                cout << "\n[Performance] Time: " << fixed << setprecision(2) << us
                     << " us | Comparisons: " << comparisons << " | Matches: " << matches << "\n";
            } else if (choice == 4) {
                cout << "Enter Visit Duration threshold in hours (e.g. 10): ";
                int minStay = readInt();
                if (minStay < 0) {
                    cout << "Invalid duration.\n";
                    continue;
                }
                int comparisons = 0;
                Timer t;
                t.start();
                int matches = linearSearchDuration_Array(data, minStay, comparisons, true);
                double us = t.elapsedUs();
                cout << "\n[Performance] Time: " << fixed << setprecision(2) << us
                     << " us | Comparisons: " << comparisons << " | Matches: " << matches << "\n";
            }
        } else if (choice == 5) {
            cout << "\nDataset (1-3): ";
            int d = readInt();
            if (d < 1 || d > 3) {
                cout << "Invalid dataset.\n";
                continue;
            }
            PatientArray& data = datasets[d - 1];

            // Benchmark queries
            int testRanges[3][2] = { {18, 25}, {46, 60}, {65, 80} };
            string rangeLabels[3] = { "Age 18-25 (Young Adult)", "Age 46-60 (Late Career)", "Age 65-80 (Geriatric)" };

            // Sorted copy for binary search
            PatientArray sortedData;
            for (int i = 0; i < data.size(); i++) sortedData.insertAtEnd(data[i]);
            long dummyC = 0, dummyM = 0;
            arrayMergeSort(sortedData, 1, true, dummyC, dummyM);

            SearchPerformanceResult results[6];
            int rIdx = 0;

            const int REPEATS = 500;
            for (int q = 0; q < 3; q++) {
                int minA = testRanges[q][0];
                int maxA = testRanges[q][1];

                // 1. Linear Search
                int linComp = 0;
                int linMatches = 0;
                Timer tLin;
                tLin.start();
                for (int rep = 0; rep < REPEATS; rep++) {
                    linMatches = linearSearchAgeRange_Array(data, minA, maxA, linComp, false);
                }
                double linUs = tLin.elapsedUs() / REPEATS;

                results[rIdx].searchType   = rangeLabels[q];
                results[rIdx].algorithm    = "Linear (Array)";
                results[rIdx].timeUs       = linUs;
                results[rIdx].comparisons  = linComp;
                results[rIdx].matchesFound = linMatches;
                results[rIdx].memoryBytes  = data.memoryBytes();
                rIdx++;

                // 2. Binary Search
                int binComp = 0;
                int binMatches = 0;
                Timer tBin;
                tBin.start();
                for (int rep = 0; rep < REPEATS; rep++) {
                    binMatches = binarySearchAgeRange_Array(sortedData, minA, maxA, binComp, false);
                }
                double binUs = tBin.elapsedUs() / REPEATS;

                results[rIdx].searchType   = rangeLabels[q];
                results[rIdx].algorithm    = "Binary (Array)";
                results[rIdx].timeUs       = binUs;
                results[rIdx].comparisons  = binComp;
                results[rIdx].matchesFound = binMatches;
                results[rIdx].memoryBytes  = sortedData.memoryBytes();
                rIdx++;
            }

            cout << "\nSEARCH PERFORMANCE BENCHMARK on " << DATASET_NAMES[d - 1]
                 << " (average of " << REPEATS << " runs)\n";
            printSearchPerformanceTable(results, 6);
            cout << ">> Binary search achieves O(log n) comparisons vs Linear search O(n).\n";
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

#endif