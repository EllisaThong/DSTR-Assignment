// =====================================================================
// AgeGroup_Array.hpp                                      (MEMBER 2)
// TASK 4: Patient Demographic & Billing Categorization -- ARRAY VERSION
//
//   PART 1  Age categorisation       (Task 4a: each age group gets its
//                                     OWN dynamic array of patients)
//   PART 2  Most requested care type (Task 4b)
//   PART 3  Total cost               (Task 4b)
//   PART 4  Average cost             (Task 4c)
//   PART 5  Display results (one table per menu option)
//   PART 6  runTask4_Array()        <-- the team's main menu calls this
//
// How to use in main.cpp:
//   #include "PatientArray.hpp"
//   #include "AgeGroup_Array.hpp"
//   runTask4_Array(facilityA, "Facility A");
// =====================================================================
#ifndef AGEGROUP_ARRAY_HPP
#define AGEGROUP_ARRAY_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include "PatientArray.hpp"

using namespace std;

const int NUM_GROUPS = 5;
const int NUM_CARE   = 7;   // 6 care types from the brief + "Other"

const string GROUP_NAMES[NUM_GROUPS] = {
    "0-17   Pediatrics & Adolescents",
    "18-25  Young Adults / University Students",
    "26-45  Working Adults (Early Career)",
    "46-60  Working Adults (Late Career)",
    "61-100 Senior Citizens / Geriatric Care"
};

const string CARE_NAMES[NUM_CARE] = {
    "Emergency", "Outpatient", "Inpatient", "Vaccination",
    "Rehabilitation", "Routine Checkup", "Other"
};

// =====================================================================
// PART 3: TOTAL COST (Task 4b)
// Cost = Length of Stay x Base Cost Per Hour x Days Visits Per Year
// =====================================================================
inline double calculatePatientCost(const Patient& p) {
    return totalCost(p);
}

// =====================================================================
// PART 1: AGE CATEGORISATION (Task 4a)
// =====================================================================

// Returns the age group (0-4) of an age, or -1 if the age is invalid
inline int categoriseAge(int age) {
    if (age >= 0  && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

// Go through ALL patients once (array traversal with an index) and copy
// each patient into the array of its age group: groups[0] .. groups[4].
// Returns the number of patients skipped because of an invalid age.
inline int recategorisePatients_Array(PatientArray& all, PatientArray groups[]) {
    int skipped = 0;
    for (int i = 0; i < all.size(); i++) {
        const Patient& p = all[i];           // direct access by index
        int g = categoriseAge(p.age);
        if (g == -1)
            skipped++;                       // invalid age: not placed in a group
        else
            groups[g].insertAtEnd(p);        // add to the end of that group's array
    }
    return skipped;
}

// =====================================================================
// PART 2: MOST REQUESTED CARE TYPE (Task 4b)
// =====================================================================

// Returns the position of a care type in CARE_NAMES (unknown = "Other")
inline int careTypeIndex(const string& careType) {
    for (int c = 0; c < NUM_CARE - 1; c++) {
        if (CARE_NAMES[c] == careType)
            return c;
    }
    return NUM_CARE - 1;
}

// Traverse one age group and count the patients and billing of each care type
inline void countCareTypes_Array(PatientArray& group, int count[], double cost[]) {
    for (int c = 0; c < NUM_CARE; c++) {
        count[c] = 0;
        cost[c]  = 0;
    }
    for (int i = 0; i < group.size(); i++) {
        int c = careTypeIndex(group[i].careType);
        count[c]++;
        cost[c] += calculatePatientCost(group[i]);
    }
}

// Returns the index (in CARE_NAMES) of the most requested care type,
// or -1 if the group has no patients. A tie goes to the type listed first.
inline int mostRequestedCare_Array(PatientArray& group) {
    if (group.size() == 0)
        return -1;

    int count[NUM_CARE];
    double cost[NUM_CARE];
    countCareTypes_Array(group, count, cost);

    int best = 0;
    for (int c = 1; c < NUM_CARE; c++) {
        if (count[c] > count[best])
            best = c;
    }
    return best;
}

// Traverse one age group and add up the cost of every patient in it
inline double totalCost_Array(PatientArray& group) {
    double total = 0;
    for (int i = 0; i < group.size(); i++)
        total += calculatePatientCost(group[i]);
    return total;
}

// =====================================================================
// PART 4: AVERAGE COST (Task 4c)
// Average Cost = Total Medical Cost in Age Group / Number of Patients
// =====================================================================
inline double averageCost_Array(PatientArray& group) {
    if (group.size() == 0)
        return 0;                            // avoid dividing by zero
    return totalCost_Array(group) / group.size();
}

// =====================================================================
// PART 5: DISPLAY RESULTS (one table for each menu option)
// =====================================================================
inline void printTask4Line(int width) {
    cout << string(width, '-') << endl;
}

// Option 1 (Age categorisation): number and share of patients in each group
inline void printAgeCategorisation_Array(PatientArray groups[], const string& datasetName) {
    int all = 0;
    for (int g = 0; g < NUM_GROUPS; g++)
        all += groups[g].size();

    cout << endl << "AGE CATEGORISATION - " << datasetName << " (ARRAY)" << endl;
    printTask4Line(66);
    cout << left << setw(44) << "Age Group" << right << setw(10) << "Patients"
         << setw(12) << "Share (%)" << endl;
    printTask4Line(66);
    cout << fixed << setprecision(1);
    for (int g = 0; g < NUM_GROUPS; g++) {
        double share = (all > 0) ? 100.0 * groups[g].size() / all : 0;
        cout << left << setw(44) << GROUP_NAMES[g] << right << setw(10) << groups[g].size()
             << setw(12) << share << endl;
    }
    printTask4Line(66);
    cout << left << setw(44) << "TOTAL" << right << setw(10) << all << endl;
}

// Option 1 (Age categorisation): the patients that were placed in one group
inline void printPatientsInGroup_Array(PatientArray& group, int g) {
    cout << endl << "Age Group: " << GROUP_NAMES[g]
         << "  (" << group.size() << " patients)" << endl;
    printTask4Line(86);
    cout << left << setw(10) << "ID" << right << setw(5) << "Age" << "   "
         << left << setw(17) << "Care Type"
         << right << setw(10) << "Stay(h)" << setw(12) << "RM/hour"
         << setw(10) << "Visits" << setw(19) << "Total Cost (RM)" << endl;
    printTask4Line(86);

    for (int i = 0; i < group.size(); i++) {
        const Patient& p = group[i];
        cout << left << setw(10) << p.patientID << right << setw(5) << p.age << "   "
             << left << setw(17) << p.careType << right << fixed
             << setw(10) << setprecision(0) << p.lengthOfStay
             << setw(12) << setprecision(2) << p.baseCostPerHour
             << setw(10) << setprecision(0) << p.daysVisitsPerYear
             << setw(19) << setprecision(2) << calculatePatientCost(p) << endl;
    }
    if (group.size() == 0)
        cout << "No patients in this age group." << endl;
    printTask4Line(86);
}

// Option 2 (Most requested care type)
inline void printMostRequestedCare_Array(PatientArray groups[], const string& datasetName) {
    cout << endl << "MOST REQUESTED CARE TYPE - " << datasetName << " (ARRAY)" << endl;
    printTask4Line(92);
    cout << left << setw(44) << "Age Group" << setw(20) << "Most Requested"
         << right << setw(14) << "Patients" << setw(14) << "Share (%)" << endl;
    printTask4Line(92);

    cout << fixed << setprecision(1);
    for (int g = 0; g < NUM_GROUPS; g++) {
        int best = mostRequestedCare_Array(groups[g]);
        cout << left << setw(44) << GROUP_NAMES[g];
        if (best == -1) {
            cout << setw(20) << "-" << right << setw(14) << 0 << setw(14) << 0.0 << endl;
        } else {
            int count[NUM_CARE];
            double cost[NUM_CARE];
            countCareTypes_Array(groups[g], count, cost);
            cout << setw(20) << CARE_NAMES[best] << right << setw(14) << count[best]
                 << setw(14) << 100.0 * count[best] / groups[g].size() << endl;
        }
    }
    printTask4Line(92);
    cout << "(Patients = how many patients in the group used that care type)" << endl;
}

// Option 3 (Total cost)
inline void printTotalCost_Array(PatientArray groups[], const string& datasetName) {
    cout << endl << "TOTAL MEDICAL COST - " << datasetName << " (ARRAY)" << endl;
    cout << "Cost = Length of Stay x Base Cost Per Hour x Days Visits Per Year" << endl;
    printTask4Line(74);
    cout << left << setw(44) << "Age Group" << right << setw(10) << "Patients"
         << setw(20) << "Total Cost (RM)" << endl;
    printTask4Line(74);

    double allCost = 0;
    int    all     = 0;
    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS; g++) {
        double total = totalCost_Array(groups[g]);
        cout << left << setw(44) << GROUP_NAMES[g] << right << setw(10) << groups[g].size()
             << setw(20) << total << endl;
        allCost += total;
        all     += groups[g].size();
    }
    printTask4Line(74);
    cout << left << setw(44) << "TOTAL BILLING" << right << setw(10) << all
         << setw(20) << allCost << endl;
}

// Option 4 (Average cost)
inline void printAverageCost_Array(PatientArray groups[], const string& datasetName) {
    cout << endl << "AVERAGE MEDICAL COST PER PATIENT - " << datasetName << " (ARRAY)" << endl;
    cout << "Average Cost = Total Medical Cost in Age Group / Number of Patients" << endl;
    printTask4Line(94);
    cout << left << setw(44) << "Age Group" << right << setw(10) << "Patients"
         << setw(20) << "Total Cost (RM)" << setw(20) << "Average (RM)" << endl;
    printTask4Line(94);

    double allCost = 0;
    int    all     = 0;
    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS; g++) {
        double total = totalCost_Array(groups[g]);
        cout << left << setw(44) << GROUP_NAMES[g] << right << setw(10) << groups[g].size()
             << setw(20) << total << setw(20) << averageCost_Array(groups[g]) << endl;
        allCost += total;
        all     += groups[g].size();
    }
    printTask4Line(94);
    cout << left << setw(44) << "ALL AGE GROUPS" << right << setw(10) << all
         << setw(20) << allCost << setw(20) << (all > 0 ? allCost / all : 0.0) << endl;
}

// Option 5: billing breakdown by care type (same layout as the sample in the brief)
inline void printBillingBreakdown_Array(PatientArray groups[], const string& datasetName) {
    cout << endl << "BILLING BREAKDOWN BY CARE TYPE - " << datasetName << " (ARRAY)" << endl;

    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS; g++) {
        cout << endl << "Age Group: " << GROUP_NAMES[g] << endl;
        printTask4Line(80);
        if (groups[g].size() == 0) {
            cout << "No patients in this age group." << endl;
            printTask4Line(80);
            continue;
        }

        int count[NUM_CARE];
        double cost[NUM_CARE];
        countCareTypes_Array(groups[g], count, cost);

        cout << left << setw(20) << "Care Type" << right << setw(15) << "Patient Count"
             << setw(20) << "Total Cost (RM)" << setw(25) << "Avg Cost/Patient (RM)" << endl;
        printTask4Line(80);
        for (int c = 0; c < NUM_CARE; c++) {
            if (count[c] == 0)
                continue;                    // only show care types that were used
            cout << left << setw(20) << CARE_NAMES[c]
                 << right << setw(15) << count[c]
                 << setw(20) << cost[c]
                 << setw(25) << cost[c] / count[c] << endl;
        }
        printTask4Line(80);
        cout << "Total Billing for Age Group: RM " << totalCost_Array(groups[g]) << endl;
    }
}

// Option 6: everything for Task 4 in one table
inline void printAgeGroupSummary_Array(PatientArray groups[], const string& datasetName) {
    cout << endl << "DEMOGRAPHIC & BILLING SUMMARY - " << datasetName << " (ARRAY)" << endl;
    printTask4Line(110);
    cout << left << setw(44) << "Age Group" << right << setw(9) << "Patients" << "   "
         << left << setw(18) << "Most Requested"
         << right << setw(18) << "Total Cost (RM)" << setw(18) << "Avg/Patient (RM)" << endl;
    printTask4Line(110);

    int    all     = 0;
    double allCost = 0;
    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS; g++) {
        int    best  = mostRequestedCare_Array(groups[g]);
        double total = totalCost_Array(groups[g]);
        cout << left << setw(44) << GROUP_NAMES[g]
             << right << setw(9) << groups[g].size() << "   "
             << left << setw(18) << (best == -1 ? "-" : CARE_NAMES[best])
             << right << setw(18) << total
             << setw(18) << averageCost_Array(groups[g]) << endl;
        all     += groups[g].size();
        allCost += total;
    }
    printTask4Line(110);
    cout << left << setw(44) << "ALL AGE GROUPS" << right << setw(9) << all << "   "
         << left << setw(18) << ""
         << right << setw(18) << allCost
         << setw(18) << (all > 0 ? allCost / all : 0.0) << endl;
}

// =====================================================================
// PART 6: RUN DEMOGRAPHIC & BILLING
// =====================================================================

// Read a menu choice between low and high; ask again if the input is invalid
inline int readTask4Choice(int low, int high) {
    int choice;
    while (!(cin >> choice) || choice < low || choice > high) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice. Enter " << low << "-" << high << ": ";
    }
    return choice;
}

inline void runTask4_Array(PatientArray& patients, string datasetName) {
    // Recategorise every patient into the array of its age group
    PatientArray groups[NUM_GROUPS];
    int skipped = recategorisePatients_Array(patients, groups);

    int choice;
    do {
        cout << endl << "===== PATIENT DEMOGRAPHIC & BILLING (ARRAY) - " << datasetName
             << " (" << patients.size() << " patients) =====" << endl;
        cout << "1. Age categorisation" << endl;
        cout << "2. Most requested care type" << endl;
        cout << "3. Total cost" << endl;
        cout << "4. Average cost" << endl;
        cout << "5. Billing breakdown by care type" << endl;
        cout << "6. Demographic & billing summary (all in one table)" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        choice = readTask4Choice(0, 6);

        if (choice == 1) {
            printAgeCategorisation_Array(groups, datasetName);
            cout << endl << "View the patients in an age group?" << endl;
            for (int g = 0; g < NUM_GROUPS; g++)
                cout << (g + 1) << ". " << GROUP_NAMES[g] << endl;
            cout << "6. All age groups" << endl << "0. Back" << endl << "Choice: ";
            int g = readTask4Choice(0, 6);
            if (g == 6) {
                for (int k = 0; k < NUM_GROUPS; k++)
                    printPatientsInGroup_Array(groups[k], k);
            } else if (g != 0) {
                printPatientsInGroup_Array(groups[g - 1], g - 1);
            }
        }
        else if (choice == 2) printMostRequestedCare_Array(groups, datasetName);
        else if (choice == 3) printTotalCost_Array(groups, datasetName);
        else if (choice == 4) printAverageCost_Array(groups, datasetName);
        else if (choice == 5) printBillingBreakdown_Array(groups, datasetName);
        else if (choice == 6) printAgeGroupSummary_Array(groups, datasetName);

        if (choice != 0 && skipped > 0)
            cout << "Note: " << skipped << " patient(s) skipped (age outside 0-100)." << endl;
    } while (choice != 0);
}

#endif
