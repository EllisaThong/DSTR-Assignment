#ifndef EXPENDITURE_LIST_HPP
#define EXPENDITURE_LIST_HPP

// ============================================================================
// expenditure_list.hpp
// Task 5 (Healthcare Expenditure & Service Analysis) and
// Task 9 (Clinical Insights & Recommendations) - SINGLY LINKED LIST version.
//
// Same reports and menus as expenditure_array.hpp, but every dataset is a
// PatientList and is processed by node traversal (cur = cur->next) instead
// of index access. Per-care-type statistics are also kept in a self-made
// singly linked list (LstCareTypeList), so no STL containers are used.
//
// Entry point for the main menu:  runExpenditureMenuList(datasets);
// ============================================================================

#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include "common.hpp"
#include "PatientList.hpp"

using namespace std;

// ----------------------------------------------------------------------------
// Local constants (LST_ prefix avoids clashing with names in common.hpp)
// ----------------------------------------------------------------------------
const int LST_MENU_WIDTH = 67;   // width of the menu frames

// Short facility labels used inside table columns.
const char* const LST_FACILITY_SHORT[NUM_DATASETS] = {
    "Facility A", "Facility B", "Facility C"
};

// ----------------------------------------------------------------------------
// Small helpers
// ----------------------------------------------------------------------------

// Division that returns 0 instead of dividing by zero.
inline double lstSafeDivide(double numerator, double denominator) {
    return (denominator == 0.0) ? 0.0 : numerator / denominator;
}

// Saves cout formatting on construction and restores it on destruction,
// so these reports do not leak fixed/setprecision into other menu options.
class LstStreamGuard {
private:
    ostream& os;
    ios_base::fmtflags savedFlags;
    streamsize         savedPrecision;

    LstStreamGuard(const LstStreamGuard&);            // non-copyable
    LstStreamGuard& operator=(const LstStreamGuard&);

public:
    explicit LstStreamGuard(ostream& stream)
        : os(stream), savedFlags(stream.flags()), savedPrecision(stream.precision()) {
    }
    ~LstStreamGuard() {
        os.flags(savedFlags);
        os.precision(savedPrecision);
    }
};

// ----------------------------------------------------------------------------
// LstCareTypeNode / LstCareTypeList
// Self-made singly linked list that accumulates statistics per care type.
// A new node is appended the first time a care type is seen, so the list
// grows only as large as needed (no fixed capacity, unlike the array version).
// ----------------------------------------------------------------------------
struct LstCareTypeNode {
    char             name[CARE_LEN];
    int              count;
    double           totalCost;
    double           totalHours;
    LstCareTypeNode* next;

    explicit LstCareTypeNode(const char* careName)
        : count(0), totalCost(0.0), totalHours(0.0), next(NULL) {
        strncpy(name, careName, CARE_LEN - 1);
        name[CARE_LEN - 1] = '\0';
    }
};

class LstCareTypeList {
private:
    LstCareTypeNode* head;
    LstCareTypeNode* tail;
    int              used;

    LstCareTypeList(const LstCareTypeList&);            // non-copyable
    LstCareTypeList& operator=(const LstCareTypeList&);

public:
    LstCareTypeList() : head(NULL), tail(NULL), used(0) {}

    ~LstCareTypeList() {
        LstCareTypeNode* cur = head;
        while (cur != NULL) {
            LstCareTypeNode* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
    }

    // Linear search by care-type name; returns NULL if not found.
    LstCareTypeNode* find(const char* name) const {
        for (LstCareTypeNode* cur = head; cur != NULL; cur = cur->next) {
            if (strcmp(cur->name, name) == 0) return cur;
        }
        return NULL;
    }

    // Adds one patient's cost and hours; appends a new node for a new care type.
    void add(const char* name, double cost, double hours) {
        LstCareTypeNode* node = find(name);
        if (node == NULL) {
            node = new LstCareTypeNode(name);
            if (head == NULL) {
                head = tail = node;
            }
            else {
                tail->next = node;
                tail = node;
            }
            used++;
        }
        node->count++;
        node->totalCost += cost;
        node->totalHours += hours;
    }

    // Node with the most patients; NULL if the list is empty.
    const LstCareTypeNode* mostFrequent() const {
        const LstCareTypeNode* best = head;
        for (const LstCareTypeNode* cur = head; cur != NULL; cur = cur->next) {
            if (cur->count > best->count) best = cur;
        }
        return best;
    }

    // Insertion sort on the linked list, highest total cost first.
    // Nodes are re-linked into a new sorted chain - no data is copied.
    void sortByCostDescending() {
        LstCareTypeNode* sorted = NULL;
        LstCareTypeNode* cur = head;

        while (cur != NULL) {
            LstCareTypeNode* nextNode = cur->next;

            if (sorted == NULL || cur->totalCost > sorted->totalCost) {
                cur->next = sorted;                       // new front of sorted chain
                sorted = cur;
            }
            else {
                LstCareTypeNode* pos = sorted;            // find insert position
                while (pos->next != NULL && pos->next->totalCost >= cur->totalCost) {
                    pos = pos->next;
                }
                cur->next = pos->next;
                pos->next = cur;
            }
            cur = nextNode;
        }

        head = sorted;
        tail = sorted;                                    // re-locate the tail
        while (tail != NULL && tail->next != NULL) tail = tail->next;
    }

    const LstCareTypeNode* getHead() const { return head; }
    int size() const { return used; }
};

// ----------------------------------------------------------------------------
// LstAgeGroupStat - accumulated statistics for one age group.
// The 5 age groups are a fixed category set, so they are indexed directly
// (getAgeGroup() returns 0..4); the patient data itself stays in the lists.
// ----------------------------------------------------------------------------
struct LstAgeGroupStat {
    int             count;
    double          totalCost;
    double          totalHours;
    LstCareTypeList careTypes;   // used to find the preferred care type

    LstAgeGroupStat() : count(0), totalCost(0.0), totalHours(0.0) {}
};

// Traverses a dataset and adds every patient into the age-group statistics
// (accumulates, so it can be called for several datasets). Invalid ages are skipped.
inline void lstAccumulateAgeGroups(const PatientList& data,
    LstAgeGroupStat groups[NUM_AGE_GROUPS]) {
    for (const PatientNode* cur = data.getHead(); cur != NULL; cur = cur->next) {
        const Patient& p = cur->data;
        const int g = getAgeGroup(p.age);
        if (g < 0) continue;

        const double cost = totalCost(p);
        groups[g].count++;
        groups[g].totalCost += cost;
        groups[g].totalHours += p.lengthOfStay;
        groups[g].careTypes.add(p.careType, cost, p.lengthOfStay);
    }
}

// Traverses a dataset and adds every patient into a care-type list.
inline void lstAccumulateCareTypes(const PatientList& data, LstCareTypeList& table) {
    for (const PatientNode* cur = data.getHead(); cur != NULL; cur = cur->next) {
        table.add(cur->data.careType, totalCost(cur->data), cur->data.lengthOfStay);
    }
}

// Total visit hours of a dataset.
inline double lstTotalHours(const PatientList& data) {
    double hours = 0.0;
    for (const PatientNode* cur = data.getHead(); cur != NULL; cur = cur->next) {
        hours += cur->data.lengthOfStay;
    }
    return hours;
}

// ============================================================================
// Task 5: Healthcare Expenditure & Service Analysis
// Multi-dataset functions take an array of pointers because PatientList
// cannot be copied. Overloads further below accept other call forms.
// ============================================================================

// Total medical billing cost of one dataset (one full traversal, O(n)).
inline double calculateTotalBillingList(const PatientList& data) {
    double total = 0.0;
    for (const PatientNode* cur = data.getHead(); cur != NULL; cur = cur->next) {
        total += totalCost(cur->data);
    }
    return total;
}

// Task 5a: Total medical billing cost of every dataset and its share of the total.
inline void displayTotalBillingPerDatasetList(const PatientList* const datasets[NUM_DATASETS]) {
    LstStreamGuard guard(cout);
    const int W = 98;

    double cost[NUM_DATASETS];
    double grandTotal = 0.0;
    int    allPatients = 0;
    for (int d = 0; d < NUM_DATASETS; d++) {
        cost[d] = calculateTotalBillingList(*datasets[d]);
        grandTotal += cost[d];
        allPatients += datasets[d]->size();
    }

    cout << "\n";
    printLine(W, '=');
    cout << "  TOTAL MEDICAL BILLING COST PER DATASET\n";
    printLine(W, '=');
    cout << left << setw(40) << "Dataset"
        << right << setw(10) << "Patients"
        << setw(20) << "Total Cost (MYR)"
        << setw(16) << "Avg Cost (MYR)"
        << setw(12) << "Share (%)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (int d = 0; d < NUM_DATASETS; d++) {
        const int n = datasets[d]->size();
        cout << left << setw(40) << DATASET_NAMES[d]
            << right << setw(10) << n
            << setw(20) << cost[d]
            << setw(16) << lstSafeDivide(cost[d], n)
            << setw(12) << lstSafeDivide(cost[d], grandTotal) * 100.0 << "\n";
    }

    printLine(W);
    cout << left << setw(40) << "ALL DATASETS"
        << right << setw(10) << allPatients
        << setw(20) << grandTotal
        << setw(16) << lstSafeDivide(grandTotal, allPatients)
        << setw(12) << 100.0 << "\n";
    printLine(W, '=');
}

// Task 5b: Total medical cost grouped by care type for one dataset,
// sorted highest to lowest, with each care type's share of the total.
inline void displayCostByCareTypeList(const PatientList& data, const char* datasetName) {
    LstStreamGuard guard(cout);
    const int W = 80;

    LstCareTypeList table;
    lstAccumulateCareTypes(data, table);
    table.sortByCostDescending();
    const double grandTotal = calculateTotalBillingList(data);

    cout << "\n";
    printLine(W, '=');
    cout << "  TOTAL MEDICAL COST BY CARE TYPE - " << datasetName << "\n";
    printLine(W, '=');

    if (data.isEmpty()) {
        cout << "  No patient records.\n";
        printLine(W, '=');
        return;
    }

    cout << left << setw(22) << "Care Type"
        << right << setw(10) << "Patients"
        << setw(20) << "Total Cost (MYR)"
        << setw(16) << "Avg Cost (MYR)"
        << setw(12) << "Share (%)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (const LstCareTypeNode* s = table.getHead(); s != NULL; s = s->next) {
        cout << left << setw(22) << s->name
            << right << setw(10) << s->count
            << setw(20) << s->totalCost
            << setw(16) << lstSafeDivide(s->totalCost, s->count)
            << setw(12) << lstSafeDivide(s->totalCost, grandTotal) * 100.0 << "\n";
    }

    printLine(W);
    cout << left << setw(22) << "TOTAL"
        << right << setw(10) << data.size()
        << setw(20) << grandTotal
        << setw(16) << lstSafeDivide(grandTotal, data.size())
        << setw(12) << 100.0 << "\n";
    printLine(W, '=');
}

// Task 5c (part 1): Billing and visit duration of every dataset side by side.
inline void displayDatasetComparisonList(const PatientList* const datasets[NUM_DATASETS]) {
    LstStreamGuard guard(cout);
    const int W = 88;

    cout << "\n";
    printLine(W, '=');
    cout << "  EXPENDITURE & VISIT DURATION ACROSS DATASETS\n";
    printLine(W, '=');
    cout << left << setw(14) << "Facility"
        << right << setw(10) << "Patients"
        << setw(20) << "Total Cost (MYR)"
        << setw(16) << "Avg Cost (MYR)"
        << setw(14) << "Total Hours"
        << setw(14) << "Avg Stay (h)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    int    allPatients = 0;
    double allCost = 0.0, allHours = 0.0;

    for (int d = 0; d < NUM_DATASETS; d++) {
        const int    n = datasets[d]->size();
        const double cost = calculateTotalBillingList(*datasets[d]);
        const double hours = lstTotalHours(*datasets[d]);
        allPatients += n;
        allCost += cost;
        allHours += hours;

        cout << left << setw(14) << LST_FACILITY_SHORT[d]
            << right << setw(10) << n
            << setw(20) << cost
            << setw(16) << lstSafeDivide(cost, n)
            << setw(14) << hours
            << setw(14) << lstSafeDivide(hours, n) << "\n";
    }

    printLine(W);
    cout << left << setw(14) << "ALL"
        << right << setw(10) << allPatients
        << setw(20) << allCost
        << setw(16) << lstSafeDivide(allCost, allPatients)
        << setw(14) << allHours
        << setw(14) << lstSafeDivide(allHours, allPatients) << "\n";
    printLine(W, '=');
}

// Task 5c (part 2): Expenditure and visit duration by age group and dataset.
inline void displayAgeGroupComparisonList(const PatientList* const datasets[NUM_DATASETS]) {
    LstStreamGuard guard(cout);
    const int W = 80;

    LstAgeGroupStat stats[NUM_DATASETS][NUM_AGE_GROUPS];
    for (int d = 0; d < NUM_DATASETS; d++) {
        lstAccumulateAgeGroups(*datasets[d], stats[d]);
    }

    cout << "\n";
    printLine(W, '=');
    cout << "  EXPENDITURE & VISIT DURATION BY AGE GROUP AND DATASET\n";
    printLine(W, '=');
    cout << left << setw(12) << "Age Group"
        << setw(14) << "Facility"
        << right << setw(10) << "Patients"
        << setw(18) << "Total Cost (MYR)"
        << setw(14) << "Avg Cost"
        << setw(12) << "Avg Stay(h)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        for (int d = 0; d < NUM_DATASETS; d++) {
            const LstAgeGroupStat& s = stats[d][g];
            // Show the age-group range only on its first row for readability.
            cout << left << setw(12) << (d == 0 ? AGE_GROUP_RANGES[g] : "")
                << setw(14) << LST_FACILITY_SHORT[d]
                << right << setw(10) << s.count
                << setw(18) << s.totalCost
                << setw(14) << lstSafeDivide(s.totalCost, s.count)
                << setw(12) << lstSafeDivide(s.totalHours, s.count) << "\n";
        }
        printLine(W);
    }
}

// ============================================================================
// Task 9: Clinical Insights and Recommendations Report (Executive View)
// Every value is calculated from the datasets - nothing is hard-coded.
// ============================================================================
inline void displayClinicalInsightsReportList(const PatientList* const datasets[NUM_DATASETS]) {
    LstStreamGuard guard(cout);
    const int W = 92;

    // ---- Gather statistics -------------------------------------------------
    LstAgeGroupStat facilityGroups[NUM_DATASETS][NUM_AGE_GROUPS];
    LstAgeGroupStat combinedGroups[NUM_AGE_GROUPS];
    LstCareTypeList facilityCare[NUM_DATASETS];
    LstCareTypeList combinedCare;
    double          facilityCost[NUM_DATASETS];
    double          grandTotal = 0.0;
    int             totalPatients = 0;

    for (int d = 0; d < NUM_DATASETS; d++) {
        lstAccumulateAgeGroups(*datasets[d], facilityGroups[d]);
        lstAccumulateAgeGroups(*datasets[d], combinedGroups);
        lstAccumulateCareTypes(*datasets[d], facilityCare[d]);
        lstAccumulateCareTypes(*datasets[d], combinedCare);
        facilityCost[d] = calculateTotalBillingList(*datasets[d]);
        grandTotal += facilityCost[d];
        totalPatients += datasets[d]->size();
    }

    cout << fixed << setprecision(2) << "\n";
    printLine(W, '=');
    cout << "             EXECUTIVE CLINICAL INSIGHTS & RECOMMENDATIONS REPORT\n";
    printLine(W, '=');

    if (totalPatients == 0) {
        cout << "  No patient records available - report cannot be generated.\n";
        printLine(W, '=');
        return;
    }

    // ---- Part 9a: service preference & cost matrix -------------------------
    cout << "\n[ Service Preference & Cost by Facility and Age Group ]\n";
    printLine(W);
    cout << left << setw(13) << "Facility"
        << setw(10) << "Age Group"
        << setw(20) << "Preferred Care"
        << right << setw(10) << "Patients"
        << setw(10) << "Share(%)"
        << setw(16) << "Total (MYR)"
        << setw(13) << "Avg (MYR)" << "\n";
    printLine(W);

    for (int d = 0; d < NUM_DATASETS; d++) {
        bool printedAny = false;
        for (int g = 0; g < NUM_AGE_GROUPS; g++) {
            const LstAgeGroupStat& s = facilityGroups[d][g];
            if (s.count == 0) continue;   // skip empty age groups

            const LstCareTypeNode* pref = s.careTypes.mostFrequent();
            cout << left << setw(13) << (printedAny ? "" : LST_FACILITY_SHORT[d])
                << setw(10) << AGE_GROUP_RANGES[g]
                << setw(20) << pref->name
                << right << setw(10) << s.count
                << setw(10) << lstSafeDivide(pref->count, s.count) * 100.0
                << setw(16) << s.totalCost
                << setw(13) << lstSafeDivide(s.totalCost, s.count) << "\n";
            printedAny = true;
        }
        if (!printedAny) {
            cout << left << setw(13) << LST_FACILITY_SHORT[d] << "(no records)\n";
        }
        printLine(W);
    }
    cout << "  Share(%) = patients in the age group who chose the preferred care type.\n";

    // ---- Part 9b: highest billing group & highest traffic care types -------
    int topCostGroup = -1, topAvgGroup = -1, longestStayGroup = -1;
    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        const LstAgeGroupStat& s = combinedGroups[g];
        if (s.count == 0) continue;

        if (topCostGroup == -1 || s.totalCost > combinedGroups[topCostGroup].totalCost)
            topCostGroup = g;

        const double avg = lstSafeDivide(s.totalCost, s.count);
        if (topAvgGroup == -1 ||
            avg > lstSafeDivide(combinedGroups[topAvgGroup].totalCost,
                combinedGroups[topAvgGroup].count))
            topAvgGroup = g;

        const double stay = lstSafeDivide(s.totalHours, s.count);
        if (longestStayGroup == -1 ||
            stay > lstSafeDivide(combinedGroups[longestStayGroup].totalHours,
                combinedGroups[longestStayGroup].count))
            longestStayGroup = g;
    }

    if (topCostGroup < 0) {   // every age was invalid
        cout << "  No patients with a valid age - insights cannot be generated.\n";
        printLine(W, '=');
        return;
    }

    const LstCareTypeNode* topCareOverall = combinedCare.mostFrequent();

    cout << "\n[ High-Billing Demographics & Peak Traffic Analysis ]\n";
    printLine(W);

    const LstAgeGroupStat& tc = combinedGroups[topCostGroup];
    cout << "1. Highest total billing age group  : " << AGE_GROUP_RANGES[topCostGroup]
        << " (" << AGE_GROUP_NAMES[topCostGroup] << ")\n"
        << "   -> MYR " << tc.totalCost << "  ("
        << lstSafeDivide(tc.totalCost, grandTotal) * 100.0 << "% of all billing, "
        << tc.count << " patients)\n\n";

    const LstAgeGroupStat& ta = combinedGroups[topAvgGroup];
    cout << "2. Highest average cost per patient : " << AGE_GROUP_RANGES[topAvgGroup]
        << " (" << AGE_GROUP_NAMES[topAvgGroup] << ")\n"
        << "   -> MYR " << lstSafeDivide(ta.totalCost, ta.count) << " per patient\n\n";

    cout << "3. Highest patient traffic care type:\n";
    for (int d = 0; d < NUM_DATASETS; d++) {
        const LstCareTypeNode* top = facilityCare[d].mostFrequent();
        cout << "   -> " << left << setw(11) << LST_FACILITY_SHORT[d] << ": ";
        if (top != NULL) {
            cout << top->name << " (" << top->count << " of " << datasets[d]->size()
                << " patients, " << lstSafeDivide(top->count, datasets[d]->size()) * 100.0 << "%)\n";
        }
        else {
            cout << "no records\n";
        }
    }
    if (topCareOverall != NULL) {
        cout << "   -> " << left << setw(11) << "Overall" << ": "
            << topCareOverall->name << " (" << topCareOverall->count << " of "
            << totalPatients << " patients)\n";
    }
    printLine(W);

    // ---- Part 9c: data-driven recommendations ------------------------------
    // Facility with the highest billing load.
    int highBill = 0;
    for (int d = 1; d < NUM_DATASETS; d++) {
        if (facilityCost[d] > facilityCost[highBill]) highBill = d;
    }

    // Best facility to receive redirected patients: one that already serves the
    // highest-billing age group, at the lowest average cost per patient.
    int target = -1;
    for (int d = 0; d < NUM_DATASETS; d++) {
        if (d == highBill) continue;
        const LstAgeGroupStat& s = facilityGroups[d][topCostGroup];
        if (s.count == 0) continue;
        if (target == -1 ||
            lstSafeDivide(s.totalCost, s.count) <
            lstSafeDivide(facilityGroups[target][topCostGroup].totalCost,
                facilityGroups[target][topCostGroup].count))
            target = d;
    }

    cout << "\n[ Recommendations for Hospital Administration ]\n";
    printLine(W);

    int rec = 1;
    cout << rec++ << ". Prioritise resources for the " << AGE_GROUP_RANGES[topCostGroup]
        << " age group (" << AGE_GROUP_NAMES[topCostGroup] << "):\n"
        << "   It produces the largest share of total billing. Reserve additional beds,\n"
        << "   specialist staff and follow-up programmes for this demographic.\n\n";

    if (topCareOverall != NULL) {
        cout << rec++ << ". Reduce bottlenecks in " << topCareOverall->name << " services:\n"
            << "   This care type has the highest patient traffic. Use appointment slots,\n"
            << "   fast-track triage and extra counters during peak hours.\n\n";
    }

    const LstAgeGroupStat& ls = combinedGroups[longestStayGroup];
    cout << rec++ << ". Improve bed turnover for the " << AGE_GROUP_RANGES[longestStayGroup]
        << " age group:\n"
        << "   Its average stay is " << lstSafeDivide(ls.totalHours, ls.count)
        << " hours, the longest of all groups. Early discharge planning\n"
        << "   and step-down / home-care options would free beds for new admissions.\n\n";

    if (target >= 0) {
        const LstAgeGroupStat& hi = facilityGroups[highBill][topCostGroup];
        const LstAgeGroupStat& lo = facilityGroups[target][topCostGroup];
        cout << rec++ << ". Balance workload between facilities:\n"
            << "   " << LST_FACILITY_SHORT[highBill] << " carries "
            << lstSafeDivide(facilityCost[highBill], grandTotal) * 100.0
            << "% of all billing. " << LST_FACILITY_SHORT[target] << " also serves the "
            << AGE_GROUP_RANGES[topCostGroup] << " group\n"
            << "   at MYR " << lstSafeDivide(lo.totalCost, lo.count) << " per patient vs MYR "
            << lstSafeDivide(hi.totalCost, hi.count) << " at "
            << LST_FACILITY_SHORT[highBill] << ".\n"
            << "   Redirect stable follow-ups and routine checkups to "
            << LST_FACILITY_SHORT[target] << " so "
            << LST_FACILITY_SHORT[highBill] << " can focus\n"
            << "   on emergency and inpatient admissions.\n";
    }
    printLine(W, '=');
}

// ============================================================================
// Convenience overloads - all of these calls work:
//     displayClinicalInsightsReportList(datasets);                 // array of 3
//     displayClinicalInsightsReportList(datasets[0], datasets[1], datasets[2]);
// Both forms build a small array of pointers and forward to the versions above.
// ============================================================================
inline void displayDatasetComparisonList(const PatientList datasets[NUM_DATASETS]) {
    const PatientList* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayDatasetComparisonList(list);
}
inline void displayDatasetComparisonList(const PatientList& a, const PatientList& b,
    const PatientList& c) {
    const PatientList* list[NUM_DATASETS] = { &a, &b, &c };
    displayDatasetComparisonList(list);
}

inline void displayAgeGroupComparisonList(const PatientList datasets[NUM_DATASETS]) {
    const PatientList* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayAgeGroupComparisonList(list);
}
inline void displayAgeGroupComparisonList(const PatientList& a, const PatientList& b,
    const PatientList& c) {
    const PatientList* list[NUM_DATASETS] = { &a, &b, &c };
    displayAgeGroupComparisonList(list);
}

inline void displayClinicalInsightsReportList(const PatientList datasets[NUM_DATASETS]) {
    const PatientList* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayClinicalInsightsReportList(list);
}
inline void displayClinicalInsightsReportList(const PatientList& a, const PatientList& b,
    const PatientList& c) {
    const PatientList* list[NUM_DATASETS] = { &a, &b, &c };
    displayClinicalInsightsReportList(list);
}

// ============================================================================
// Menu for Task 5 & Task 9  (called from main menu option [3])
//
//   [1] Healthcare Expenditure & Service Analysis
//         [1] Total medical billing cost per dataset        (Task 5a)
//         [2] Total medical cost grouped by care type       (Task 5b)
//         [3] Expenditure & visit duration comparison       (Task 5c)
//         [0] Back
//   [2] Clinical Insights and Recommendations               (Task 9)
//   [0] Back to main menu
// ============================================================================

// Returns '?' for invalid input and '0' (back) if input has ended.
inline char lstReadLetter() {
    string line;
    if (!getline(cin, line)) return '0';

    char found = '?';
    int  letters = 0;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r') continue;
        found = line[i];
        letters++;
    }
    return (letters == 1) ? found : '?';
}

// Task 5b: asks which dataset to show (or all three).
inline void lstRunCostByCareType(const PatientList* const datasets[NUM_DATASETS]) {
    cout << "\nSelect dataset:\n";
    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "  [" << (d + 1) << "] " << DATASET_NAMES[d] << "\n";
    }
    cout << "  [" << (NUM_DATASETS + 1) << "] All datasets\n"
        << "  [0] Back\n"
        << "Enter choice: ";

    const int c = readInt();
    if (c == 0) {
        return;
    }
    else if (c >= 1 && c <= NUM_DATASETS) {
        displayCostByCareTypeList(*datasets[c - 1], DATASET_NAMES[c - 1]);
    }
    else if (c == NUM_DATASETS + 1) {
        for (int d = 0; d < NUM_DATASETS; d++) {
            displayCostByCareTypeList(*datasets[d], DATASET_NAMES[d]);
        }
    }
    else {
        cout << "\nInvalid dataset choice.\n";
    }
}

// Sub-menu [1]: Healthcare Expenditure & Service Analysis (Task 5).
inline void lstRunServiceAnalysisMenu(const PatientList* const datasets[NUM_DATASETS]) {
    char choice;
    do {
        cout << "\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "   HEALTHCARE EXPENDITURE & SERVICE ANALYSIS  [LINKED LIST]\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "[1] Calculate total medical billing costs per dataset\n"
            << "[2] Determine total medical costs grouped by Care Type\n"
            << "[3] Compare expenditure and visit durations across\n"
            << "    datasets and age groups\n"
            << "[0] Back\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "Enter choice: ";

        choice = lstReadLetter();
        switch (choice) {
        case '1':
            displayTotalBillingPerDatasetList(datasets);
            waitForEnter();
            break;
        case '2':
            lstRunCostByCareType(datasets);
            waitForEnter();
            break;
        case '3':
            displayDatasetComparisonList(datasets);
            displayAgeGroupComparisonList(datasets);
            waitForEnter();
            break;
        case '0':
            break;
        default:
            cout << "\nInvalid choice, please enter 1~3 or 0.\n";
        }
    } while (choice != '0');
}

inline void lstRunServiceAnalysisMenu(const PatientList datasets[NUM_DATASETS]) {
    const PatientList* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    lstRunServiceAnalysisMenu(list);
}

// Entry point for main menu option [3]: Healthcare Expenditure & Insights.
inline void runExpenditureMenuList(const PatientList datasets[NUM_DATASETS]) {
    // Build an array of pointers once (PatientList cannot be copied).
    const PatientList* list[NUM_DATASETS];
    bool anyData = false;
    for (int d = 0; d < NUM_DATASETS; d++) {
        list[d] = &datasets[d];
        if (!datasets[d].isEmpty()) anyData = true;
    }
    if (!anyData) {
        cout << "\nNo patient data loaded. Please load the datasets first.\n";
        return;
    }

    int choice;
    do {
        cout << "\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "   HEALTHCARE EXPENDITURE & INSIGHTS  [LINKED LIST]\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "[1] Healthcare Expenditure & Service Analysis\n"
            << "[2] Clinical Insights and Recommendations\n"
            << "[0] Back to Main Menu\n";
        printLine(LST_MENU_WIDTH, '=');
        cout << "Enter choice: ";

        choice = readInt();
        switch (choice) {
        case 1:
            lstRunServiceAnalysisMenu(list);
            break;
        case 2:
            displayClinicalInsightsReportList(list);
            waitForEnter();
            break;
        case 0:
            break;
        default:
            cout << "\nInvalid choice, please enter 1, 2 or 0.\n";
        }
    } while (choice != 0);
}

#endif // EXPENDITURE_LIST_HPP