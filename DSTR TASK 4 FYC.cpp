
#ifndef AGEGROUP_ARRAY_HPP
#define AGEGROUP_ARRAY_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include "PatientArray.hpp"

using namespace std;

const int NUM_GROUPS = 5;
const int NUM_CARE = 7;   // 6 care types from the brief + "Other"

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

// PART 3: TOTAL COST (Task 4b)
// Cost = Length of Stay x Base Cost Per Hour x Days Visits Per Year

inline double calculatePatientCost(const Patient& p) {
    return totalCost(p);
}


// PART 1: AGE CATEGORISATION (Task 4a)


// Returns the age group (0-4) of an age, or -1 if the age is invalid
inline int categoriseAge(int age) {
    if (age >= 0 && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61 && age <= 100) return 4;
    return -1;
}

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


// PART 2: MOST REQUESTED CARE TYPE 


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
        cost[c] = 0;
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


// PART 4: AVERAGE COST
// Average Cost = Total Medical Cost in Age Group / Number of Patients

inline double averageCost_Array(PatientArray& group) {
    if (group.size() == 0)
        return 0;                            // avoid dividing by zero
    return totalCost_Array(group) / group.size();
}


// PART 5: DISPLAY RESULTS

inline void printTask4Line(int width) {
    cout << string(width, '-') << endl;
}



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

    int    all = 0;
    double allCost = 0;
    cout << fixed << setprecision(2);
    for (int g = 0; g < NUM_GROUPS; g++) {
        int    best = mostRequestedCare_Array(groups[g]);
        double total = totalCost_Array(groups[g]);
        cout << left << setw(44) << GROUP_NAMES[g]
            << right << setw(9) << groups[g].size() << "   "
            << left << setw(18) << (best == -1 ? "-" : CARE_NAMES[best])
            << right << setw(18) << total
            << setw(18) << averageCost_Array(groups[g]) << endl;
        all += groups[g].size();
        allCost += total;
    }
    printTask4Line(110);
    cout << left << setw(44) << "ALL AGE GROUPS" << right << setw(9) << all << "   "
        << left << setw(18) << ""
        << right << setw(18) << allCost
        << setw(18) << (all > 0 ? allCost / all : 0.0) << endl;
}


// PART 6: RUN DEMOGRAPHIC & BILLING


// Read a menu choice between low and high; ask again if the input is invalid
inline int readTask4Choice(int low, int high) {
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

inline void runTask4_Array(PatientArray& patients, string datasetName) {
    // Age categorisation: copy every patient into the array of its age group
    PatientArray groups[NUM_GROUPS];
    int skipped = recategorisePatients_Array(patients, groups);

    int choice;
    do {
        cout << endl << "===== PATIENT DEMOGRAPHIC & BILLING (ARRAY) - " << datasetName << " =====" << endl;
        cout << "1. Age group summary (patients, most requested care, total & average cost)" << endl;
        cout << "2. Billing breakdown by care type" << endl;
        cout << "3. View patients in an age group" << endl;
        cout << "0. Back" << endl;
        cout << "Choice: ";
        choice = readTask4Choice(0, 3);

        if (choice == 1) {
            printAgeGroupSummary_Array(groups, datasetName);
        }
        else if (choice == 2) {
            printBillingBreakdown_Array(groups, datasetName);
        }
        else if (choice == 3) {
            for (int g = 0; g < NUM_GROUPS; g++)
                cout << (g + 1) << ". " << GROUP_NAMES[g] << endl;
            cout << "Choose age group (1-5): ";
            int g = readTask4Choice(1, NUM_GROUPS);
            printPatientsInGroup_Array(groups[g - 1], g - 1);
        }

        if (choice != 0 && skipped > 0)
            cout << "Note: " << skipped << " patient(s) skipped (age outside 0-100)." << endl;
    } while (choice != 0);
}

#endif
