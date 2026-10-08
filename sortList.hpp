#ifndef SORT_LIST_HPP
#define SORT_LIST_HPP

// Algorithm 1: Bubble Sort  O(n^2)
// Algorithm 2: Merge Sort   O(n log n)
// Sort keys: 1 = Age, 2 = Length of Stay, 3 = Total Medical Cost

#include "PatientList.hpp"

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
inline void listBubbleSort(PatientList& list, int key, bool ascending,
                           long& comparisons, long& swaps) {
    int n = list.size();
    for (int pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        PatientNode* current = list.getHead();
        for (int i = 0; i < n - 1 - pass; i++) {
            comparisons++;
            if (isWrongOrder(current->data, current->next->data, key, ascending)) {
                Patient temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swaps++;
                swapped = true;
            }
            current = current->next;
        }
        if (!swapped) break;
    }
}

// Merge Sort

inline PatientNode* splitList(PatientNode* head) {
    PatientNode* slow = head;
    PatientNode* fast = head->next;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    PatientNode* secondHalf = slow->next;
    slow->next = nullptr;
    return secondHalf;
}

inline PatientNode* mergeLists(PatientNode* a, PatientNode* b, int key, bool ascending,
                               long& comparisons, long& relinks) {
    PatientNode* newHead = nullptr;
    PatientNode* newTail = nullptr;

    while (a != nullptr && b != nullptr) {
        comparisons++;
        PatientNode* smaller;
        if (!isWrongOrder(a->data, b->data, key, ascending)) {
            smaller = a;
            a = a->next;
        } else {
            smaller = b;
            b = b->next;
        }
        if (newHead == nullptr) newHead = smaller;
        else newTail->next = smaller;
        newTail = smaller;
        relinks++;
    }
    if (a != nullptr) newTail->next = a;
    if (b != nullptr) newTail->next = b;
    return newHead;
}

inline PatientNode* listMergeSortRec(PatientNode* head, int key, bool ascending,
                                     long& comparisons, long& relinks) {
    if (head == nullptr || head->next == nullptr) return head;
    PatientNode* second = splitList(head);
    head   = listMergeSortRec(head,   key, ascending, comparisons, relinks);
    second = listMergeSortRec(second, key, ascending, comparisons, relinks);
    return mergeLists(head, second, key, ascending, comparisons, relinks);
}

inline void listMergeSort(PatientList& list, int key, bool ascending,
                          long& comparisons, long& relinks) {
    if (list.size() < 2) return;
    PatientNode* newHead = listMergeSortRec(list.getHead(), key, ascending,
                                            comparisons, relinks);
    list.setHead(newHead);
}

// Checking and displaying
inline bool listIsSorted(const PatientList& list, int key, bool ascending) {
    for (PatientNode* cur = list.getHead(); cur != nullptr && cur->next != nullptr;
         cur = cur->next)
        if (isWrongOrder(cur->data, cur->next->data, key, ascending)) return false;
    return true;
}

inline void listShowFirst(const PatientList& list, int rows) {
    printPatientTableHeader();
    int i = 0;
    for (PatientNode* cur = list.getHead(); cur != nullptr && i < rows; cur = cur->next) {
        i++;
        printPatientRow(i, cur->data);
    }
    printLine(PATIENT_TABLE_WIDTH);
    cout << "(showing first " << rows << " of " << list.size() << ")\n";
}

struct ListSortResult {
    double timeUs;
    long   comparisons;
    long   swapsOrLinks;
    size_t extraMemory;
    bool   sorted;
};

// algorithm: 1 = Bubble, 2 = Merge
inline ListSortResult runListSort(PatientList& source, int algorithm, int key,
                                  bool ascending, bool showRecords) {
    ListSortResult r;
    r.comparisons = 0;
    r.swapsOrLinks = 0;
    r.sorted = true;

    // Copy before timing.
    PatientList* copies[SORT_REPEAT];
    for (int i = 0; i < SORT_REPEAT; i++) copies[i] = source.clone();

    if (showRecords) {
        cout << "\nBEFORE sorting:\n";
        listShowFirst(*copies[0], 10);
    }

    Timer t;
    t.start();
    for (int i = 0; i < SORT_REPEAT; i++) {
        long c = 0, s = 0;
        if (algorithm == 1) listBubbleSort(*copies[i], key, ascending, c, s);
        else                listMergeSort(*copies[i], key, ascending, c, s);
        if (i == 0) { r.comparisons = c; r.swapsOrLinks = s; }
    }
    r.timeUs = t.elapsedUs() / SORT_REPEAT;

    for (int i = 0; i < SORT_REPEAT; i++)
        if (!listIsSorted(*copies[i], key, ascending)) r.sorted = false;

    if (showRecords) {
        cout << "\nAFTER sorting by " << getKeyName(key)
             << (ascending ? " (ascending):\n" : " (descending):\n");
        listShowFirst(*copies[0], 10);
    }

    if (algorithm == 1) r.extraMemory = sizeof(Patient);
    else {
        int depth = 0;
        for (int n = source.size(); n > 1; n = (n + 1) / 2) depth++;
        r.extraMemory = depth * 4 * sizeof(PatientNode*);
    }

    for (int i = 0; i < SORT_REPEAT; i++) delete copies[i];
    return r;
}

inline void printListSortHeader() {
    printLine(112);
    cout << left << setw(12) << "Structure"
         << setw(10) << "Algorithm"
         << setw(16) << "Sort Key"
         << right << setw(14) << "Time (us)"
         << setw(14) << "Comparisons"
         << setw(14) << "Swaps/Links"
         << setw(12) << "Data Mem"
         << setw(12) << "Extra Mem"
         << setw(8)  << "Sorted?" << "\n";
    printLine(112);
}

inline void printListSortRow(const char* algoName, int key, const ListSortResult& r,
                             size_t dataMemory) {
    cout << left << setw(12) << "Linked List"
         << setw(10) << algoName
         << setw(16) << getKeyName(key)
         << right << fixed << setprecision(2)
         << setw(14) << r.timeUs
         << setw(14) << r.comparisons
         << setw(14) << r.swapsOrLinks
         << setw(12) << formatBytes(dataMemory)
         << setw(12) << formatBytes(r.extraMemory)
         << setw(8)  << (r.sorted ? "YES" : "NO") << "\n";
}

inline void printSpeedup(const ListSortResult& bubble, const ListSortResult& merge) {
    if (merge.timeUs > 0)
        cout << ">> Merge Sort was " << fixed << setprecision(1)
             << bubble.timeUs / merge.timeUs << "x faster and used "
             << bubble.comparisons - merge.comparisons
             << " fewer comparisons than Bubble Sort.\n";
}

// Menu
inline int listReadSortingChoice(int maxChoice) {
    while (true) {
        cout << "Enter choice: ";
        int choice = readInt();
        if (choice >= 0 && choice <= maxChoice) return choice;
        cout << "\nInvalid choice, please enter 0-" << maxChoice << ".\n";
    }
}

inline void listViewAllSortedRecords(const PatientList& source, int algorithm,
                                    int key, bool ascending) {
    cout << "\n  1. View all sorted records (20 per page)\n"
         << "  0. Back\n";
    int choice = listReadSortingChoice(1);
    switch (choice) {
    case 1: {
        PatientList* records = source.clone();
        long comparisons = 0, links = 0;
        if (algorithm == 1)
            listBubbleSort(*records, key, ascending, comparisons, links);
        else
            listMergeSort(*records, key, ascending, comparisons, links);

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

inline void listSortingMenu(PatientList datasets[]) {
    int choice;
    do {
        cout << "\n";
        printLine(50, '=');
        cout << "   Sorting Experiments & Benchmark [LINKED LIST]\n";
        printLine(50, '=');
        cout << "  1. Bubble Sort (show before/after)\n"
             << "  2. Merge Sort (show before/after)\n"
             << "  3. Compare Bubble Sort vs Merge Sort\n"
             << "  4. Full benchmark (all datasets, all keys)\n"
             << "  0. Back\n";
        printLine(50, '=');
        choice = listReadSortingChoice(4);

        switch (choice) {
        case 1:
        case 2:
        case 3: {
            cout << "\nSelect dataset:\n";
            for (int d = 0; d < NUM_DATASETS; d++)
                cout << "  " << (d + 1) << ". " << DATASET_NAMES[d] << "\n";
            cout << "  0. Back\n";
            int d = listReadSortingChoice(NUM_DATASETS);
            if (d == 0) continue;

            cout << "\nSelect sorting criterion:\n"
                 << "  1. Age\n"
                 << "  2. Visit Duration (Length of Stay)\n"
                 << "  3. Total Medical Cost\n"
                 << "  0. Back\n";
            int key = listReadSortingChoice(3);
            if (key == 0) continue;

            cout << "\nSelect sorting order:\n"
                 << "  1. Ascending\n"
                 << "  2. Descending\n"
                 << "  0. Back\n";
            int order = listReadSortingChoice(2);
            if (order == 0) continue;
            bool asc = (order == 1);
            PatientList& data = datasets[d - 1];

            if (choice == 1 || choice == 2) {
                ListSortResult r = runListSort(data, choice, key, asc, true);
                cout << "\nPERFORMANCE (average of " << SORT_REPEAT << " runs)\n";
                printListSortHeader();
                printListSortRow(choice == 1 ? "Bubble" : "Merge", key, r,
                                 data.memoryBytes());
                printLine(112);
                listViewAllSortedRecords(data, choice, key, asc);
                continue;
            } else {
                ListSortResult b = runListSort(data, 1, key, asc, false);
                ListSortResult m = runListSort(data, 2, key, asc, false);
                cout << "\nBUBBLE vs MERGE on " << DATASET_NAMES[d - 1]
                     << " (average of " << SORT_REPEAT << " runs)\n";
                printListSortHeader();
                printListSortRow("Bubble", key, b, data.memoryBytes());
                printListSortRow("Merge",  key, m, data.memoryBytes());
                printLine(112);
                printSpeedup(b, m);
            }
            break;
        }
        case 4: {
            cout << "\nFULL BENCHMARK [LINKED LIST] (ascending, average of "
                 << SORT_REPEAT << " runs)\n";
            printListSortHeader();
            for (int d = 0; d < NUM_DATASETS; d++) {
                cout << DATASET_NAMES[d] << "\n";
                for (int key = 1; key <= 3; key++) {
                    ListSortResult b = runListSort(datasets[d], 1, key, true, false);
                    ListSortResult m = runListSort(datasets[d], 2, key, true, false);
                    printListSortRow("Bubble", key, b, datasets[d].memoryBytes());
                    printListSortRow("Merge",  key, m, datasets[d].memoryBytes());
                }
            }
            printLine(112);
            break;
        }
        case 0:
            break;
        }
        if (choice != 0) waitForEnter();
    } while (choice != 0);
}

#endif
