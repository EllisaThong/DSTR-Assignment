// =====================================================================
// AgeGroup_List.hpp                                      (MEMBER 2)
// TASK 4: Patient Demographic & Billing Categorization -- LINKED LIST VERSION
//
//   PART 1  Age categorisation       (Task 4a: each age group gets its
//                                     OWN linked list of patients)
//   PART 2  Most requested care type (Task 4b)
//   PART 3  Total cost               (Task 4b)
//   PART 4  Average cost             (Task 4c)
//   PART 5  Display results (summary, billing breakdown, patient list)
//   PART 6  runTask4_List()        <-- the team's main menu calls this
//
// How to use in main.cpp:
//   #include "PatientList.hpp"
//   #include "AgeGroup_List.hpp"
//   runTask4_List(facilityA, "Facility A");
// =====================================================================
#ifndef AGEGROUP_LIST_HPP
#define AGEGROUP_LIST_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include "PatientList.hpp"

using namespace std;

const int NUM_GROUPS_LIST = 5;
const int NUM_CARE_LIST   = 7;   // 6 care types from the brief + "Other"

const string GROUP_NAMES_LIST[NUM_GROUPS_LIST] = {
    "0-17   Pediatrics & Adolescents",
    "18-25  Young Adults / University Students",
    "26-45  Working Adults (Early Career)",
    "46-60  Working Adults (Late Career)",
    "61-100 Senior Citizens / Geriatric Care"
};

const string CARE_NAMES_LIST[NUM_CARE_LIST] = {
    "Emergency", "Outpatient", "Inpatient", "Vaccination",
    "Rehabilitation", "Routine Checkup", "Other"
};

// =====================================================================
// PART 3: TOTAL COST (Task 4b)
// Cost = Length of Stay x Base Cost Per Hour x Days Visits Per Year
// =====================================================================
inline double calculatePatientCost_List(const Patient& p) {
    return totalCost(p);
}

// =====================================================================
// PART 1: AGE CATEGORISATION (Task 4a)
// =====================================================================

// Returns the age group (0-4) of an age, or -1 if the age is invalid
inline int categoriseAge_List(int age) {
    if (age >= 0  && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

// Go through ALL patients once (linked list traversal: start at head and
// follow each link until NULL) and copy each patient into the linked list
// of its age group: groups[0] .. groups[4].
// Returns the number of patients skipped because of an invalid age.
inline int recategorisePatients_List(PatientList& all, PatientList groups[]) {
    int skipped = 0;
    PatientNode* current = all.getHead();        // start at the first node
    while (current != NULL) {
        const Patient& p = current->data;        // record inside this node
        int g = categoriseAge_List(p.age);
        if (g == -1)
            skipped++;                           // invalid age: not placed in a group
        else
            groups[g].insertAtEnd(p);            // new node at the end of that group's list
        current = current->next;                 // move to the next node
    }
    return skipped;
}

// =====================================================================
// PART 2: MOST REQUESTED CARE TYPE (Task 4b)
// =====================================================================

// Returns the position of a care type in CARE_NAMES_LIST (unknown = "Other")
inline int careTypeIndex_List(const string& careType) {
    for (int c = 0; c < NUM_CARE_LIST - 1; c++) {
        if (CARE_NAMES_LIST[c] == careType)
            return c;
    }
    return NUM_CARE_LIST - 1;
}

// Traverse one age group and count the patients and billing of each care type
inline void countCareTypes_List(PatientList& group, int count[], double cost[]) {
    for (int c = 0; c < NUM_CARE_LIST; c++) {
        count[c] = 0;
        cost[c]  = 0;
    }
    PatientNode* current = group.getHead();
    while (current != NULL) {
        int c = careTypeIndex_List(current->data.careType);
        count[c]++;
        cost[c] += calculatePatientCost_List(current->data);
        current = current->next;
    }
}

// Returns the index (in CARE_NAMES_LIST) of the most requested care type,
// or -1 if the group has no patients. A tie goes to the type listed first.
inline int mostRequestedCare_List(PatientList& group) {
    if (group.size() == 0)
        return -1;

    int count[NUM_CARE_LIST];
    double cost[NUM_CARE_LIST];
    countCareTypes_List(group, count, cost);

    int best = 0;
    for (int c = 1; c < NUM_CARE_LIST; c++) {
        if (count[c] > count[best])
            best = c;
    }
    return best;
}

// Traverse one age group and add up the cost of every patient in it
inline double totalCost_List(PatientList& group) {
    double total = 0;
    PatientNode* current = group.getHead();
    while (current != NULL) {
        total += calculatePatientCost_List(current->data);
        current = current->next;
    }
    return total;
}

// =====================================================================
// PART 4: AVERAGE COST (Task 4c)
// Average Cost = Total Medical Cost in Age Group / Number of Patients
// =====================================================================
inline double averageCost_List(PatientList& group) {
    if (group.size() == 0)
        return 0;                            // avoid dividing by zero
    return totalCost_List(group) / group.size();
}

// =====================================================================
// PART 5: DISPLAY RESULTS
// =====================================================================
inline void printTask4Line_List(int width) {
    cout << string(width, '-') << endl;
}


// Option 1 (Age categorisation): the patients that were placed in one group
inline void printPatientsInGroup_List(PatientList& group, int g) {
    cout << endl << "Age Group: " << GROUP_NAMES_LIST[g]
         << "  (" << group.size() << " patients)" << endl;
    printTask4Line_List(86);
    cout << left << setw(10) << "ID" << right << setw(5) << "Age" << "   "
         << left << setw(17) << "Care Type"
         << right << setw(10) << "Stay(h)" << setw(12) << "RM/hour"
         << setw(10) << "Visits" << setw(19) << "Total Cost (RM)" << endl;
    printTask4Line_List(86);

    PatientNode* current = group.getHead();
    while (current != NULL) {
        const Patient& p = current->data;
        cout << left << setw(10) << p.patientID << right << setw(5) << p.age << "   "
             << left << setw(17) << p.careType << right << fixed
             << setw(10) << setprecision(0) << p.lengthOfStay
             << setw(12) << setprecision(2) << p.baseCostPerHour
             << setw(10) << setprecision(0) << p.daysVisitsPerYear
             << setw(19) << setprecision(2) << calculatePatientCost_List(p) << endl;
        current = current->next;
    }
    if (group.size() == 0)
        cout << "No patients in this age group." << endl;
    printTask4Line_List(86);
}




// Option 5: billing breakdown by care type (same layout as the sample in the brief)
inline void printBillingBreakdown_List(PatientList groups[], const string& datasetName) {
    cout << endl << "BILLING BREAKDOWN BY CARE TYPE - " << datasetName << " (LINKED LIST)" << endl;

    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS_LIST; g++) {
        cout << endl << "Age Group: " << GROUP_NAMES_LIST[g] << endl;
        printTask4Line_List(80);
        if (groups[g].size() == 0) {
            cout << "No patients in this age group." << endl;
            printTask4Line_List(80);
            continue;
        }

        int count[NUM_CARE_LIST];
        double cost[NUM_CARE_LIST];
        countCareTypes_List(groups[g], count, cost);

        cout << left << setw(20) << "Care Type" << right << setw(15) << "Patient Count"
             << setw(20) << "Total Cost (RM)" << setw(25) << "Avg Cost/Patient (RM)" << endl;
        printTask4Line_List(80);
        for (int c = 0; c < NUM_CARE_LIST; c++) {
            if (count[c] == 0)
                continue;                    // only show care types that were used
            cout << left << setw(20) << CARE_NAMES_LIST[c]
                 << right << setw(15) << count[c]
                 << setw(20) << cost[c]
                 << setw(25) << cost[c] / count[c] << endl;
        }
        printTask4Line_List(80);
        cout << "Total Billing for Age Group: RM " << totalCost_List(groups[g]) << endl;
    }
}

// Option 6: everything for Task 4 in one table
inline void printAgeGroupSummary_List(PatientList groups[], const string& datasetName) {
    cout << endl << "DEMOGRAPHIC & BILLING SUMMARY - " << datasetName << " (LINKED LIST)" << endl;
    printTask4Line_List(110);
    cout << left << setw(44) << "Age Group" << right << setw(9) << "Patients" << "   "
         << left << setw(18) << "Most Requested"
         << right << setw(18) << "Total Cost (RM)" << setw(18) << "Avg/Patient (RM)" << endl;
    printTask4Line_List(110);

    int    all     = 0;
    double allCost = 0;
    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS_LIST; g++) {
        int    best  = mostRequestedCare_List(groups[g]);
        double total = totalCost_List(groups[g]);
        cout << left << setw(44) << GROUP_NAMES_LIST[g]
             << right << setw(9) << groups[g].size() << "   "
             << left << setw(18) << (best == -1 ? "-" : CARE_NAMES_LIST[best])
             << right << setw(18) << total
             << setw(18) << averageCost_List(groups[g]) << endl;
        all     += groups[g].size();
        allCost += total;
    }
    printTask4Line_List(110);
    cout << left << setw(44) << "ALL AGE GROUPS" << right << setw(9) << all << "   "
         << left << setw(18) << ""
         << right << setw(18) << allCost
         << setw(18) << (all > 0 ? allCost / all : 0.0) << endl;
}

// =====================================================================
// PART 6: RUN DEMOGRAPHIC & BILLING
// =====================================================================

inline int readTask4Choice_List(int low, int high) {
    // Uses readInt() from common.hpp, which reads the WHOLE line with getline.
    // (cin >> would leave the Enter key in the buffer, and the main menu would
    //  then read that empty line as an extra "Invalid choice".)
    int choice = readInt();
    while (choice < low || choice > high) {
        cout << "Invalid choice. Enter " << low << "-" << high << ": ";
        choice = readInt();
    }
    return choice;
}

inline void runTask4_List(PatientList& patients, string datasetName) {
    // Age categorisation: copy every patient into the linked list of its age group
    PatientList groups[NUM_GROUPS_LIST];
    int skipped = recategorisePatients_List(patients, groups);

    int choice;
    do {
        cout << endl << "===== PATIENT DEMOGRAPHIC & BILLING (LINKED LIST) - " << datasetName << " =====" << endl;
        cout << "1. Age group summary (patients, most requested care, total & average cost)" << endl;
        cout << "2. Billing breakdown by care type" << endl;
        cout << "3. View patients in an age group" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        choice = readTask4Choice_List(0, 3);

        if (choice == 1) {
            printAgeGroupSummary_List(groups, datasetName);
        }
        else if (choice == 2) {
            printBillingBreakdown_List(groups, datasetName);
        }
        else if (choice == 3) {
            for (int g = 0; g < NUM_GROUPS_LIST; g++)
                cout << (g + 1) << ". " << GROUP_NAMES_LIST[g] << endl;
            cout << "Choose age group (1-5): ";
            int g = readTask4Choice_List(1, NUM_GROUPS_LIST);
            printPatientsInGroup_List(groups[g - 1], g - 1);
        }

        if (choice != 0 && skipped > 0)
            cout << "Note: " << skipped << " patient(s) skipped (age outside 0-100)." << endl;
    } while (choice != 0);
}

#endif
