#ifndef SEARCH_LIST_HPP
#define SEARCH_LIST_HPP

// ============================================================================
// Search_List.hpp - Member 5, Task 7: Searching experiments (LINKED LIST version)
//
// 1. Linear Search on Unsorted Linked List: O(n)
//    - Search by Age Range / Age Group
//    - Search by Care Type
//    - Search by Visit Duration threshold (> X hours)
//
// 2. Early-Stop Linear Search on Sorted Linked List (by Age):
//    - As list is sorted ascending by Age, as soon as node->age > maxAge,
//      we break out early, avoiding checking the remainder of the list.
//    - Note: Binary search is NOT implemented on linked lists because
//      sequential pointer traversal O(n) to find midpoints yields O(n log n),
//      which is strictly inferior to O(n) linear search.
//
// 3. Performance Benchmark:
//    - Compares execution time (us), comparisons, and memory overhead.
// ============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include "PatientList.hpp"
#include "sortList.hpp"

using namespace std;

struct ListSearchPerformanceResult {
    string searchType;
    string algorithm;
    double timeUs;
    int    comparisons;
    int    matchesFound;
    size_t memoryBytes;
};

// ===================== Helper Display =====================
inline void printListSearchResultsHeader() {
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

// ===================== Linear Search on Linked List =====================
inline int linearSearchAgeRange_List(const PatientList& list, int minAge, int maxAge,
                                     int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printListSearchResultsHeader();

    PatientNode* cur = list.getHead();
    while (cur != NULL) {
        comparisons++;
        if (cur->data.age >= minAge && cur->data.age <= maxAge) {
            matches++;
            if (display) printPatientRow(matches, cur->data);
        }
        cur = cur->next;
    }

    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

inline int earlyStopSearchAgeRange_List(const PatientList& sortedList, int minAge, int maxAge,
                                        int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printListSearchResultsHeader();

    PatientNode* cur = sortedList.getHead();
    while (cur != NULL) {
        comparisons++;
        if (cur->data.age > maxAge) {
            // Early exit: list is sorted ascending by age, no further matches possible!
            break;
        }
        if (cur->data.age >= minAge) {
            matches++;
            if (display) printPatientRow(matches, cur->data);
        }
        cur = cur->next;
    }

    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found (Early Exit applied): " << matches << "\n";
    }
    return matches;
}

inline int linearSearchCareType_List(const PatientList& list, const string& careType,
                                     int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printListSearchResultsHeader();

    PatientNode* cur = list.getHead();
    while (cur != NULL) {
        comparisons++;
        if (careType == cur->data.careType) {
            matches++;
            if (display) printPatientRow(matches, cur->data);
        }
        cur = cur->next;
    }

    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

inline int linearSearchDuration_List(const PatientList& list, double minHours,
                                     int& comparisons, bool display) {
    comparisons = 0;
    int matches = 0;
    if (display) printListSearchResultsHeader();

    PatientNode* cur = list.getHead();
    while (cur != NULL) {
        comparisons++;
        if (cur->data.lengthOfStay >= minHours) {
            matches++;
            if (display) printPatientRow(matches, cur->data);
        }
        cur = cur->next;
    }

    if (display) {
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total matches found: " << matches << "\n";
    }
    return matches;
}

// ===================== Performance Benchmark =====================
inline void printListSearchPerformanceTable(const ListSearchPerformanceResult results[], int count) {
    printLine(94);
    cout << left << setw(28) << "Query Type"
         << setw(20) << "Algorithm"
         << right << setw(12) << "Time (us)"
         << setw(14) << "Comparisons"
         << setw(10) << "Matches"
         << setw(10) << "Memory" << "\n";
    printLine(94);

    for (int i = 0; i < count; i++) {
        cout << left << setw(28) << results[i].searchType
             << setw(20) << results[i].algorithm
             << right << fixed << setprecision(2)
             << setw(12) << results[i].timeUs
             << setw(14) << results[i].comparisons
             << setw(10) << results[i].matchesFound
             << setw(10) << formatBytes(results[i].memoryBytes) << "\n";
    }
    printLine(94);
}

// ===================== Interactive Search Menu =====================
inline void listSearchingMenu(PatientList datasets[]) {
    int choice;
    do {
        cout << "\n------ SEARCHING EXPERIMENTS [LINKED LIST] ------\n"
             << "  1. Search by Age Range (Linear Search - Unsorted)\n"
             << "  2. Search by Age Range (Early-Stop Linear Search - Sorted by Age)\n"
             << "  3. Search by Specific Care Type (Linear Search)\n"
             << "  4. Search by Visit Duration Threshold (Linear Search)\n"
             << "  5. Compare Unsorted vs Sorted Search Performance\n"
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
            PatientList& data = datasets[d - 1];

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
                int matches = linearSearchAgeRange_List(data, minAge, maxAge, comparisons, true);
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
                PatientList* sortedCopy = data.clone();
                long dummyC = 0, dummyR = 0;
                listMergeSort(*sortedCopy, 1, true, dummyC, dummyR);

                int comparisons = 0;
                Timer t;
                t.start();
                int matches = earlyStopSearchAgeRange_List(*sortedCopy, minAge, maxAge, comparisons, true);
                double us = t.elapsedUs();
                cout << "\n[Performance] Time: " << fixed << setprecision(2) << us
                     << " us | Comparisons: " << comparisons << " | Matches: " << matches << "\n";
                delete sortedCopy;
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
                int matches = linearSearchCareType_List(data, selectedCare, comparisons, true);
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
                int matches = linearSearchDuration_List(data, minStay, comparisons, true);
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
            PatientList& data = datasets[d - 1];

            int testRanges[3][2] = { {18, 25}, {46, 60}, {65, 80} };
            string rangeLabels[3] = { "Age 18-25 (Young Adult)", "Age 46-60 (Late Career)", "Age 65-80 (Geriatric)" };

            // Sorted copy for early-stop linear search
            PatientList* sortedData = data.clone();
            long dummyC = 0, dummyR = 0;
            listMergeSort(*sortedData, 1, true, dummyC, dummyR);

            ListSearchPerformanceResult results[6];
            int rIdx = 0;

            const int REPEATS = 500;
            for (int q = 0; q < 3; q++) {
                int minA = testRanges[q][0];
                int maxA = testRanges[q][1];

                // 1. Unsorted Linear Search
                int linComp = 0;
                int linMatches = 0;
                Timer tLin;
                tLin.start();
                for (int rep = 0; rep < REPEATS; rep++) {
                    linMatches = linearSearchAgeRange_List(data, minA, maxA, linComp, false);
                }
                double linUs = tLin.elapsedUs() / REPEATS;

                results[rIdx].searchType   = rangeLabels[q];
                results[rIdx].algorithm    = "Linear (Unsorted)";
                results[rIdx].timeUs       = linUs;
                results[rIdx].comparisons  = linComp;
                results[rIdx].matchesFound = linMatches;
                results[rIdx].memoryBytes  = data.memoryBytes();
                rIdx++;

                // 2. Sorted Early-Stop Linear Search
                int earlyComp = 0;
                int earlyMatches = 0;
                Timer tEarly;
                tEarly.start();
                for (int rep = 0; rep < REPEATS; rep++) {
                    earlyMatches = earlyStopSearchAgeRange_List(*sortedData, minA, maxA, earlyComp, false);
                }
                double earlyUs = tEarly.elapsedUs() / REPEATS;

                results[rIdx].searchType   = rangeLabels[q];
                results[rIdx].algorithm    = "Early-Stop (Sorted)";
                results[rIdx].timeUs       = earlyUs;
                results[rIdx].comparisons  = earlyComp;
                results[rIdx].matchesFound = earlyMatches;
                results[rIdx].memoryBytes  = sortedData->memoryBytes();
                rIdx++;
            }

            cout << "\nSEARCH PERFORMANCE BENCHMARK on " << DATASET_NAMES[d - 1]
                 << " (average of " << REPEATS << " runs)\n";
            printListSearchPerformanceTable(results, 6);
            cout << ">> Note: Binary search is avoided on linked lists due to sequential access O(n log n).\n"
                 << "   Early-stop linear search on sorted list terminates as soon as age > maxAge.\n";

            delete sortedData;
        } else if (choice != 0) {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

#endif