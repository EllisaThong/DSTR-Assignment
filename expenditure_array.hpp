#ifndef EXPENDITURE_ARRAY_HPP
#define EXPENDITURE_ARRAY_HPP

// ============================================================================
// expenditure_array.hpp
// Task 5 (Healthcare Expenditure & Service Analysis) and
// Task 9 (Clinical Insights & Recommendations) - ARRAY implementation.
//
// All figures are calculated at runtime from the PatientArray datasets.
// Shared definitions (Patient, totalCost, getAgeGroup, NUM_AGE_GROUPS,
// AGE_GROUP_RANGES, printLine, readInt, waitForEnter, ...) come from
// common.hpp.
// No STL containers are used: per-care-type and per-age-group statistics
// are kept in small self-made fixed-capacity tables defined below.
//
// Entry point for the main menu:  runExpenditureMenuArray(datasets);
// ============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "common.hpp"
#include "PatientArray.hpp"

using namespace std;

// ----------------------------------------------------------------------------
// Local constants (EXP_ prefix avoids clashing with names in common.hpp)
// ----------------------------------------------------------------------------
const int EXP_MAX_CARE_TYPES = 16;   // capacity of one ExpCareTypeTable
const int EXP_MENU_WIDTH = 67;   // width of the menu frames

// Short facility labels used inside table columns.
const char* const EXP_FACILITY_SHORT[NUM_DATASETS] = {
    "Facility A", "Facility B", "Facility C"
};

// ----------------------------------------------------------------------------
// Small helpers
// ----------------------------------------------------------------------------

// Division that returns 0 instead of dividing by zero.
inline double expSafeDivide(double numerator, double denominator) {
    return (denominator == 0.0) ? 0.0 : numerator / denominator;
}

// Saves cout formatting on construction and restores it on destruction,
// so these reports do not leak fixed/setprecision into other menu options.
class ExpStreamGuard {
private:
    ostream& os;
    ios_base::fmtflags savedFlags;
    streamsize         savedPrecision;

    ExpStreamGuard(const ExpStreamGuard&);            // non-copyable
    ExpStreamGuard& operator=(const ExpStreamGuard&);

public:
    explicit ExpStreamGuard(ostream& stream)
        : os(stream), savedFlags(stream.flags()), savedPrecision(stream.precision()) {
    }
    ~ExpStreamGuard() {
        os.flags(savedFlags);
        os.precision(savedPrecision);
    }
};

// ----------------------------------------------------------------------------
// ExpCareTypeStat / ExpCareTypeTable
// Self-made fixed-capacity array that accumulates statistics per care type.
// Care types are discovered from the data, so nothing is hard-coded.
// ----------------------------------------------------------------------------
struct ExpCareTypeStat {
    char   name[CARE_LEN];
    int    count;
    double totalCost;
    double totalHours;
};

class ExpCareTypeTable {
private:
    ExpCareTypeStat items[EXP_MAX_CARE_TYPES];
    int             used;

public:
    ExpCareTypeTable() : used(0) {}

    // Linear search by care-type name; returns -1 if not found.
    int indexOf(const char* name) const {
        for (int i = 0; i < used; i++) {
            if (strcmp(items[i].name, name) == 0) return i;
        }
        return -1;
    }

    // Adds one patient's cost and hours; creates a new entry for a new care type.
    void add(const char* name, double cost, double hours) {
        int idx = indexOf(name);
        if (idx == -1) {
            if (used >= EXP_MAX_CARE_TYPES) {
                cerr << "[Warning] Care type table full, ignored: " << name << "\n";
                return;
            }
            idx = used++;
            strncpy(items[idx].name, name, CARE_LEN - 1);
            items[idx].name[CARE_LEN - 1] = '\0';
            items[idx].count = 0;
            items[idx].totalCost = 0.0;
            items[idx].totalHours = 0.0;
        }
        items[idx].count++;
        items[idx].totalCost += cost;
        items[idx].totalHours += hours;
    }

    // Index of the care type with the most patients; -1 if empty.
    int mostFrequentIndex() const {
        if (used == 0) return -1;
        int best = 0;
        for (int i = 1; i < used; i++) {
            if (items[i].count > items[best].count) best = i;
        }
        return best;
    }

    // Insertion sort, highest total cost first (table is small, O(n^2) is fine).
    void sortByCostDescending() {
        for (int i = 1; i < used; i++) {
            ExpCareTypeStat key = items[i];
            int j = i - 1;
            while (j >= 0 && items[j].totalCost < key.totalCost) {
                items[j + 1] = items[j];
                j--;
            }
            items[j + 1] = key;
        }
    }

    int size() const { return used; }
    const ExpCareTypeStat& at(int i) const { return items[i]; }
};

// ----------------------------------------------------------------------------
// ExpAgeGroupStat - accumulated statistics for one age group.
// ----------------------------------------------------------------------------
struct ExpAgeGroupStat {
    int              count;
    double           totalCost;
    double           totalHours;
    ExpCareTypeTable careTypes;   // used to find the preferred care type

    ExpAgeGroupStat() : count(0), totalCost(0.0), totalHours(0.0) {}
};

// Adds every patient of a dataset into an age-group array (accumulates,
// so it can be called for several datasets). Invalid ages are skipped.
inline void expAccumulateAgeGroups(const PatientArray& data,
    ExpAgeGroupStat groups[NUM_AGE_GROUPS]) {
    for (int i = 0; i < data.size(); i++) {
        const Patient& p = data[i];
        const int g = getAgeGroup(p.age);
        if (g < 0) continue;

        const double cost = totalCost(p);
        groups[g].count++;
        groups[g].totalCost += cost;
        groups[g].totalHours += p.lengthOfStay;
        groups[g].careTypes.add(p.careType, cost, p.lengthOfStay);
    }
}

// Adds every patient of a dataset into a care-type table.
inline void expAccumulateCareTypes(const PatientArray& data, ExpCareTypeTable& table) {
    for (int i = 0; i < data.size(); i++) {
        table.add(data[i].careType, totalCost(data[i]), data[i].lengthOfStay);
    }
}

// Total visit hours of a dataset.
inline double expTotalHours(const PatientArray& data) {
    double hours = 0.0;
    for (int i = 0; i < data.size(); i++) hours += data[i].lengthOfStay;
    return hours;
}

// ============================================================================
// Task 5: Healthcare Expenditure & Service Analysis
// Multi-dataset functions take an array of pointers because PatientArray
// cannot be copied. Overloads further below accept other call forms.
// ============================================================================

// Total medical billing cost of one dataset.
inline double calculateTotalBillingArray(const PatientArray& data) {
    double total = 0.0;
    for (int i = 0; i < data.size(); i++) {
        total += totalCost(data[i]);
    }
    return total;
}

// Task 5a: Total medical billing cost of every dataset and its share of the total.
inline void displayTotalBillingPerDatasetArray(const PatientArray* const datasets[NUM_DATASETS]) {
    ExpStreamGuard guard(cout);
    const int W = 98;

    double cost[NUM_DATASETS];
    double grandTotal = 0.0;
    int    allPatients = 0;
    for (int d = 0; d < NUM_DATASETS; d++) {
        cost[d] = calculateTotalBillingArray(*datasets[d]);
        grandTotal += cost[d];
        allPatients += datasets[d]->size();
    }

    cout << "\n";
    printLine(W, '=');
    cout << "  TOTAL MEDICAL BILLING COST PER DATASET\n";
    printLine(W, '=');
    cout << left << setw(40) << "Dataset"
        << right << setw(10) << "Patients"
        << setw(20) << "Total Cost (RM)"
        << setw(16) << "Avg Cost (RM)"
        << setw(12) << "Share (%)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (int d = 0; d < NUM_DATASETS; d++) {
        const int n = datasets[d]->size();
        cout << left << setw(40) << DATASET_NAMES[d]
            << right << setw(10) << n
            << setw(20) << cost[d]
            << setw(16) << expSafeDivide(cost[d], n)
            << setw(12) << expSafeDivide(cost[d], grandTotal) * 100.0 << "\n";
    }

    printLine(W);
    cout << left << setw(40) << "ALL DATASETS"
        << right << setw(10) << allPatients
        << setw(20) << grandTotal
        << setw(16) << expSafeDivide(grandTotal, allPatients)
        << setw(12) << 100.0 << "\n";
    printLine(W, '=');
}

// Task 5b: Total medical cost grouped by care type for one dataset,
// sorted highest to lowest, with each care type's share of the total.
inline void displayCostByCareTypeArray(const PatientArray& data, const char* datasetName) {
    ExpStreamGuard guard(cout);
    const int W = 80;

    ExpCareTypeTable table;
    expAccumulateCareTypes(data, table);
    table.sortByCostDescending();
    const double grandTotal = calculateTotalBillingArray(data);

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
        << setw(20) << "Total Cost (RM)"
        << setw(16) << "Avg Cost (RM)"
        << setw(12) << "Share (%)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (int i = 0; i < table.size(); i++) {
        const ExpCareTypeStat& s = table.at(i);
        cout << left << setw(22) << s.name
            << right << setw(10) << s.count
            << setw(20) << s.totalCost
            << setw(16) << expSafeDivide(s.totalCost, s.count)
            << setw(12) << expSafeDivide(s.totalCost, grandTotal) * 100.0 << "\n";
    }

    printLine(W);
    cout << left << setw(22) << "TOTAL"
        << right << setw(10) << data.size()
        << setw(20) << grandTotal
        << setw(16) << expSafeDivide(grandTotal, data.size())
        << setw(12) << 100.0 << "\n";
    printLine(W, '=');
}

// Task 5c (part 1): Billing and visit duration of every dataset side by side.
inline void displayDatasetComparisonArray(const PatientArray* const datasets[NUM_DATASETS]) {
    ExpStreamGuard guard(cout);
    const int W = 88;

    cout << "\n";
    printLine(W, '=');
    cout << "  EXPENDITURE & VISIT DURATION ACROSS DATASETS\n";
    printLine(W, '=');
    cout << left << setw(14) << "Facility"
        << right << setw(10) << "Patients"
        << setw(20) << "Total Cost (RM)"
        << setw(16) << "Avg Cost (RM)"
        << setw(14) << "Total Hours"
        << setw(14) << "Avg Stay (h)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    int    allPatients = 0;
    double allCost = 0.0, allHours = 0.0;

    for (int d = 0; d < NUM_DATASETS; d++) {
        const int    n = datasets[d]->size();
        const double cost = calculateTotalBillingArray(*datasets[d]);
        const double hours = expTotalHours(*datasets[d]);
        allPatients += n;
        allCost += cost;
        allHours += hours;

        cout << left << setw(14) << EXP_FACILITY_SHORT[d]
            << right << setw(10) << n
            << setw(20) << cost
            << setw(16) << expSafeDivide(cost, n)
            << setw(14) << hours
            << setw(14) << expSafeDivide(hours, n) << "\n";
    }

    printLine(W);
    cout << left << setw(14) << "ALL"
        << right << setw(10) << allPatients
        << setw(20) << allCost
        << setw(16) << expSafeDivide(allCost, allPatients)
        << setw(14) << allHours
        << setw(14) << expSafeDivide(allHours, allPatients) << "\n";
    printLine(W, '=');
}

// Task 5c (part 2): Expenditure and visit duration by age group and dataset.
inline void displayAgeGroupComparisonArray(const PatientArray* const datasets[NUM_DATASETS]) {
    ExpStreamGuard guard(cout);
    const int W = 80;

    ExpAgeGroupStat stats[NUM_DATASETS][NUM_AGE_GROUPS];
    for (int d = 0; d < NUM_DATASETS; d++) {
        expAccumulateAgeGroups(*datasets[d], stats[d]);
    }

    cout << "\n";
    printLine(W, '=');
    cout << "  EXPENDITURE & VISIT DURATION BY AGE GROUP AND DATASET\n";
    printLine(W, '=');
    cout << left << setw(12) << "Age Group"
        << setw(14) << "Facility"
        << right << setw(10) << "Patients"
        << setw(18) << "Total Cost (RM)"
        << setw(14) << "Avg Cost"
        << setw(12) << "Avg Stay(h)" << "\n";
    printLine(W);

    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        for (int d = 0; d < NUM_DATASETS; d++) {
            const ExpAgeGroupStat& s = stats[d][g];
            // Show the age-group range only on its first row for readability.
            cout << left << setw(12) << (d == 0 ? AGE_GROUP_RANGES[g] : "")
                << setw(14) << EXP_FACILITY_SHORT[d]
                << right << setw(10) << s.count
                << setw(18) << s.totalCost
                << setw(14) << expSafeDivide(s.totalCost, s.count)
                << setw(12) << expSafeDivide(s.totalHours, s.count) << "\n";
        }
        printLine(W);
    }
}

// ============================================================================
// Task 9: Clinical Insights and Recommendations Report (Executive View)
// Every value is calculated from the datasets - nothing is hard-coded.
// ============================================================================
inline void displayClinicalInsightsReportArray(const PatientArray* const datasets[NUM_DATASETS]) {
    ExpStreamGuard guard(cout);
    const int W = 92;

    // ---- Gather statistics -------------------------------------------------
    ExpAgeGroupStat  facilityGroups[NUM_DATASETS][NUM_AGE_GROUPS];
    ExpAgeGroupStat  combinedGroups[NUM_AGE_GROUPS];
    ExpCareTypeTable facilityCare[NUM_DATASETS];
    ExpCareTypeTable combinedCare;
    double           facilityCost[NUM_DATASETS];
    double           grandTotal = 0.0;
    int              totalPatients = 0;

    for (int d = 0; d < NUM_DATASETS; d++) {
        expAccumulateAgeGroups(*datasets[d], facilityGroups[d]);
        expAccumulateAgeGroups(*datasets[d], combinedGroups);
        expAccumulateCareTypes(*datasets[d], facilityCare[d]);
        expAccumulateCareTypes(*datasets[d], combinedCare);
        facilityCost[d] = calculateTotalBillingArray(*datasets[d]);
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
        << setw(16) << "Total (RM)"
        << setw(13) << "Avg (RM)" << "\n";
    printLine(W);

    for (int d = 0; d < NUM_DATASETS; d++) {
        bool printedAny = false;
        for (int g = 0; g < NUM_AGE_GROUPS; g++) {
            const ExpAgeGroupStat& s = facilityGroups[d][g];
            if (s.count == 0) continue;   // skip empty age groups

            const ExpCareTypeStat& pref = s.careTypes.at(s.careTypes.mostFrequentIndex());
            cout << left << setw(13) << (printedAny ? "" : EXP_FACILITY_SHORT[d])
                << setw(10) << AGE_GROUP_RANGES[g]
                << setw(20) << pref.name
                << right << setw(10) << s.count
                << setw(10) << expSafeDivide(pref.count, s.count) * 100.0
                << setw(16) << s.totalCost
                << setw(13) << expSafeDivide(s.totalCost, s.count) << "\n";
            printedAny = true;
        }
        if (!printedAny) {
            cout << left << setw(13) << EXP_FACILITY_SHORT[d] << "(no records)\n";
        }
        printLine(W);
    }
    cout << "  Share(%) = patients in the age group who chose the preferred care type.\n";

    // ---- Part 9b: highest billing group & highest traffic care types -------
    int topCostGroup = -1, topAvgGroup = -1, longestStayGroup = -1;
    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        const ExpAgeGroupStat& s = combinedGroups[g];
        if (s.count == 0) continue;

        if (topCostGroup == -1 || s.totalCost > combinedGroups[topCostGroup].totalCost)
            topCostGroup = g;

        const double avg = expSafeDivide(s.totalCost, s.count);
        if (topAvgGroup == -1 ||
            avg > expSafeDivide(combinedGroups[topAvgGroup].totalCost,
                combinedGroups[topAvgGroup].count))
            topAvgGroup = g;

        const double stay = expSafeDivide(s.totalHours, s.count);
        if (longestStayGroup == -1 ||
            stay > expSafeDivide(combinedGroups[longestStayGroup].totalHours,
                combinedGroups[longestStayGroup].count))
            longestStayGroup = g;
    }

    if (topCostGroup < 0) {   // every age was invalid
        cout << "  No patients with a valid age - insights cannot be generated.\n";
        printLine(W, '=');
        return;
    }

    const int topCareOverall = combinedCare.mostFrequentIndex();

    cout << "\n[ High-Billing Demographics & Peak Traffic Analysis ]\n";
    printLine(W);

    const ExpAgeGroupStat& tc = combinedGroups[topCostGroup];
    cout << "1. Highest total billing age group  : " << AGE_GROUP_RANGES[topCostGroup]
        << " (" << AGE_GROUP_NAMES[topCostGroup] << ")\n"
        << "   -> RM " << tc.totalCost << "  ("
        << expSafeDivide(tc.totalCost, grandTotal) * 100.0 << "% of all billing, "
        << tc.count << " patients)\n\n";

    const ExpAgeGroupStat& ta = combinedGroups[topAvgGroup];
    cout << "2. Highest average cost per patient : " << AGE_GROUP_RANGES[topAvgGroup]
        << " (" << AGE_GROUP_NAMES[topAvgGroup] << ")\n"
        << "   -> RM " << expSafeDivide(ta.totalCost, ta.count) << " per patient\n\n";

    cout << "3. Highest patient traffic care type:\n";
    for (int d = 0; d < NUM_DATASETS; d++) {
        const int top = facilityCare[d].mostFrequentIndex();
        cout << "   -> " << left << setw(11) << EXP_FACILITY_SHORT[d] << ": ";
        if (top >= 0) {
            const ExpCareTypeStat& s = facilityCare[d].at(top);
            cout << s.name << " (" << s.count << " of " << datasets[d]->size()
                << " patients, " << expSafeDivide(s.count, datasets[d]->size()) * 100.0 << "%)\n";
        }
        else {
            cout << "no records\n";
        }
    }
    if (topCareOverall >= 0) {
        const ExpCareTypeStat& s = combinedCare.at(topCareOverall);
        cout << "   -> " << left << setw(11) << "Overall" << ": "
            << s.name << " (" << s.count << " of " << totalPatients << " patients)\n";
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
        const ExpAgeGroupStat& s = facilityGroups[d][topCostGroup];
        if (s.count == 0) continue;
        if (target == -1 ||
            expSafeDivide(s.totalCost, s.count) <
            expSafeDivide(facilityGroups[target][topCostGroup].totalCost,
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

    if (topCareOverall >= 0) {
        cout << rec++ << ". Reduce bottlenecks in " << combinedCare.at(topCareOverall).name
            << " services:\n"
            << "   This care type has the highest patient traffic. Use appointment slots,\n"
            << "   fast-track triage and extra counters during peak hours.\n\n";
    }

    const ExpAgeGroupStat& ls = combinedGroups[longestStayGroup];
    cout << rec++ << ". Improve bed turnover for the " << AGE_GROUP_RANGES[longestStayGroup]
        << " age group:\n"
        << "   Its average stay is " << expSafeDivide(ls.totalHours, ls.count)
        << " hours, the longest of all groups. Early discharge planning\n"
        << "   and step-down / home-care options would free beds for new admissions.\n\n";

    if (target >= 0) {
        const ExpAgeGroupStat& hi = facilityGroups[highBill][topCostGroup];
        const ExpAgeGroupStat& lo = facilityGroups[target][topCostGroup];
        cout << rec++ << ". Balance workload between facilities:\n"
            << "   " << EXP_FACILITY_SHORT[highBill] << " carries "
            << expSafeDivide(facilityCost[highBill], grandTotal) * 100.0
            << "% of all billing. " << EXP_FACILITY_SHORT[target] << " also serves the "
            << AGE_GROUP_RANGES[topCostGroup] << " group\n"
            << "   at RM " << expSafeDivide(lo.totalCost, lo.count) << " per patient vs RM "
            << expSafeDivide(hi.totalCost, hi.count) << " at "
            << EXP_FACILITY_SHORT[highBill] << ".\n"
            << "   Redirect stable follow-ups and routine checkups to "
            << EXP_FACILITY_SHORT[target] << " so "
            << EXP_FACILITY_SHORT[highBill] << " can focus\n"
            << "   on emergency and inpatient admissions.\n";
    }
    printLine(W, '=');
}

// ============================================================================
// Convenience overloads - all of these calls work:
//     displayClinicalInsightsReportArray(datasets);                 // array of 3
//     displayClinicalInsightsReportArray(datasets[0], datasets[1], datasets[2]);
// Both forms build a small array of pointers and forward to the versions above.
// ============================================================================
inline void displayDatasetComparisonArray(const PatientArray datasets[NUM_DATASETS]) {
    const PatientArray* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayDatasetComparisonArray(list);
}
inline void displayDatasetComparisonArray(const PatientArray& a, const PatientArray& b,
    const PatientArray& c) {
    const PatientArray* list[NUM_DATASETS] = { &a, &b, &c };
    displayDatasetComparisonArray(list);
}

inline void displayAgeGroupComparisonArray(const PatientArray datasets[NUM_DATASETS]) {
    const PatientArray* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayAgeGroupComparisonArray(list);
}
inline void displayAgeGroupComparisonArray(const PatientArray& a, const PatientArray& b,
    const PatientArray& c) {
    const PatientArray* list[NUM_DATASETS] = { &a, &b, &c };
    displayAgeGroupComparisonArray(list);
}

inline void displayClinicalInsightsReportArray(const PatientArray datasets[NUM_DATASETS]) {
    const PatientArray* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    displayClinicalInsightsReportArray(list);
}
inline void displayClinicalInsightsReportArray(const PatientArray& a, const PatientArray& b,
    const PatientArray& c) {
    const PatientArray* list[NUM_DATASETS] = { &a, &b, &c };
    displayClinicalInsightsReportArray(list);
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
inline char expReadLetter() {
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
inline void expRunCostByCareType(const PatientArray* const datasets[NUM_DATASETS]) {
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
        displayCostByCareTypeArray(*datasets[c - 1], DATASET_NAMES[c - 1]);
    }
    else if (c == NUM_DATASETS + 1) {
        for (int d = 0; d < NUM_DATASETS; d++) {
            displayCostByCareTypeArray(*datasets[d], DATASET_NAMES[d]);
        }
    }
    else {
        cout << "\nInvalid dataset choice.\n";
    }
}

// Sub-menu [1]: Healthcare Expenditure & Service Analysis (Task 5).
inline void expRunServiceAnalysisMenu(const PatientArray* const datasets[NUM_DATASETS]) {
    char choice;
    do {
        cout << "\n";
        printLine(EXP_MENU_WIDTH, '=');
        cout << "   HEALTHCARE EXPENDITURE & SERVICE ANALYSIS  [ARRAY]\n";
        printLine(EXP_MENU_WIDTH, '=');
        cout << "[1] Calculate total medical billing costs per dataset\n"
            << "[2] Determine total medical costs grouped by Care Type\n"
            << "[3] Compare expenditure and visit durations across\n"
            << "    datasets and age groups\n"
            << "[0] Back\n";
        printLine(EXP_MENU_WIDTH, '=');
        cout << "Enter choice: ";

        choice = expReadLetter();
        switch (choice) {
        case '1':
            displayTotalBillingPerDatasetArray(datasets);
            waitForEnter();
            break;
        case '2':
            expRunCostByCareType(datasets);
            waitForEnter();
            break;
        case '3':
            displayDatasetComparisonArray(datasets);
            displayAgeGroupComparisonArray(datasets);
            waitForEnter();
            break;
        case '0':
            break;
        default:
            cout << "\nInvalid choice, please enter 1~3 or 0.\n";
        }
    } while (choice != '0');
}

inline void expRunServiceAnalysisMenu(const PatientArray datasets[NUM_DATASETS]) {
    const PatientArray* list[NUM_DATASETS] = { &datasets[0], &datasets[1], &datasets[2] };
    expRunServiceAnalysisMenu(list);
}

// Entry point for main menu option [3]: Healthcare Expenditure & Insights.
inline void runExpenditureMenuArray(const PatientArray datasets[NUM_DATASETS]) {
    // Build an array of pointers once (PatientArray cannot be copied).
    const PatientArray* list[NUM_DATASETS];
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
        printLine(EXP_MENU_WIDTH, '=');
        cout << "   HEALTHCARE EXPENDITURE & INSIGHTS  [ARRAY]\n";
        printLine(EXP_MENU_WIDTH, '=');
        cout << "[1] Healthcare Expenditure & Service Analysis\n"
            << "[2] Clinical Insights and Recommendations\n"
            << "[0] Back to Main Menu\n";
        printLine(EXP_MENU_WIDTH, '=');
        cout << "Enter choice: ";

        choice = readInt();
        switch (choice) {
        case 1:
            expRunServiceAnalysisMenu(list);
            break;
        case 2:
            displayClinicalInsightsReportArray(list);
            waitForEnter();
            break;
        case 0:
            break;
        default:
            cout << "\nInvalid choice, please enter 1, 2 or 0.\n";
        }
    } while (choice != 0);
}

#endif // EXPENDITURE_ARRAY_HPP