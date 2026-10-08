#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <chrono>

using namespace std;

const int NUM_DATASETS = 3;

const char* const DATASET_FILES[NUM_DATASETS] = {
    "dataset1_facility_a.csv",
    "dataset2_facility_b.csv",
    "dataset3_facility_c.csv"
};

const char* const DATASET_NAMES[NUM_DATASETS] = {
    "Facility A (General Hospital)",
    "Facility B (University Medical Center)",
    "Facility C (Community Health Clinic)"
};

const int ID_LEN   = 12;
const int CARE_LEN = 20;

const int NUM_CARE_TYPES = 7;
const char* const CARE_TYPES[NUM_CARE_TYPES] = {
    "Emergency", "Outpatient", "Inpatient", "Vaccination",
    "Rehabilitation", "Routine Checkup", "Other"
};

struct Patient {
    char   patientID[ID_LEN];
    int    age;
    char   careType[CARE_LEN];
    double lengthOfStay;
    double baseCostPerHour;
    int    daysVisitsPerYear;

    Patient() {
        patientID[0] = '\0';
        age = 0;
        careType[0] = '\0';
        lengthOfStay = 0.0;
        baseCostPerHour = 0.0;
        daysVisitsPerYear = 0;
    }

    Patient(const char* id, int a, const char* care, double stay, double cost, int visits) {
        strncpy(patientID, id, ID_LEN - 1);
        patientID[ID_LEN - 1] = '\0';
        age = a;
        strncpy(careType, care, CARE_LEN - 1);
        careType[CARE_LEN - 1] = '\0';
        lengthOfStay = stay;
        baseCostPerHour = cost;
        daysVisitsPerYear = visits;
    }
};

inline double totalCost(const Patient& p) {
    return p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;
}

const int NUM_AGE_GROUPS = 5;

const char* const AGE_GROUP_NAMES[NUM_AGE_GROUPS] = {
    "Pediatrics & Adolescents",
    "Young Adults / University Students",
    "Working Adults (Early Career)",
    "Working Adults (Late Career)",
    "Senior Citizens / Geriatric Care"
};

const char* const AGE_GROUP_RANGES[NUM_AGE_GROUPS] = {
    "0-17", "18-25", "26-45", "46-60", "61-100"
};

inline int getAgeGroup(int age) {
    if (age < 0 || age > 100) return -1;
    if (age <= 17) return 0;
    if (age <= 25) return 1;
    if (age <= 45) return 2;
    if (age <= 60) return 3;
    return 4;
}

class Timer {
private:
    chrono::steady_clock::time_point startTime;
public:
    Timer() : startTime(chrono::steady_clock::now()) {}
    void start() { startTime = chrono::steady_clock::now(); }
    double elapsedMs() const {
        return chrono::duration<double, milli>(
            chrono::steady_clock::now() - startTime).count();
    }
    double elapsedUs() const {
        return chrono::duration<double, micro>(
            chrono::steady_clock::now() - startTime).count();
    }
};

inline bool parsePatientLine(const string& rawLine, Patient& out) {
    string line = rawLine;
    while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
        line.erase(line.size() - 1);
    if (line.empty()) return false;

    string field[6];
    int f = 0;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ',') {
            if (++f >= 6) return false;
        } else {
            field[f] += line[i];
        }
    }
    if (f != 5) return false;
    for (int i = 0; i < 6; i++) if (field[i].empty()) return false;

    strncpy(out.patientID, field[0].c_str(), ID_LEN - 1);
    out.patientID[ID_LEN - 1] = '\0';
    strncpy(out.careType, field[2].c_str(), CARE_LEN - 1);
    out.careType[CARE_LEN - 1] = '\0';

    out.age               = atoi(field[1].c_str());
    out.lengthOfStay      = atof(field[3].c_str());
    out.baseCostPerHour   = atof(field[4].c_str());
    out.daysVisitsPerYear = atoi(field[5].c_str());
    return true;
}

inline void printLine(int width, char ch = '-') {
    cout << string(width, ch) << "\n";
}

const int PATIENT_TABLE_WIDTH = 99;

inline void printPatientTableHeader() {
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

inline void printPatientRow(int rowNo, const Patient& p) {
    int g = getAgeGroup(p.age);
    cout << left
         << setw(6)  << rowNo
         << setw(11) << p.patientID
         << setw(5)  << p.age
         << setw(9)  << (g >= 0 ? AGE_GROUP_RANGES[g] : "N/A")
         << setw(17) << p.careType
         << fixed << setprecision(1)
         << setw(10) << p.lengthOfStay
         << setw(13) << p.baseCostPerHour
         << setw(10) << p.daysVisitsPerYear
         << right << setprecision(2)
         << setw(18) << totalCost(p) << "\n";
}

inline string formatBytes(size_t bytes) {
    char buf[32];
    if (bytes < 1024)
        snprintf(buf, sizeof(buf), "%zu B", bytes);
    else if (bytes < 1024 * 1024)
        snprintf(buf, sizeof(buf), "%.2f KB", bytes / 1024.0);
    else
        snprintf(buf, sizeof(buf), "%.2f MB", bytes / (1024.0 * 1024.0));
    return string(buf);
}

inline int readInt() {
    string line;
    if (!getline(cin, line)) return 0;
    if (line.empty()) return -1;
    for (size_t i = 0; i < line.size(); i++)
        if (line[i] < '0' || line[i] > '9') return -1;
    return atoi(line.c_str());
}

inline void waitForEnter() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

#endif
