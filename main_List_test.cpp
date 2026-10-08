// =====================================================================
// TEST main for Member 2 (List version)
// Loads the 3 CSV datasets and lets you run Task 4 on each one.
// The CSV loader here is only for testing: in the final program,
// Member 1's code loads the data and the team menu calls runTask4_List().
// =====================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "PatientList.hpp"
#include "AgeGroup_List.hpp"
using namespace std;

// Read one CSV file into the structure. Returns the number of patients loaded.
// CSV columns: PatientID,Age,CareType,LengthOfStay,BaseCostPerHour,DaysVisitsPerYear
int loadCSV(string fileName, PatientList& patients) {
    ifstream file(fileName);
    if (!file.is_open())
        return -1;                           // file not found here

    string line;
    getline(file, line);                     // skip the header row

    int loaded = 0;
    while (getline(file, line)) {
        Patient p;
        if (parsePatientLine(line, p)) {
            patients.insertAtEnd(p);
            loaded++;
        }
    }
    return loaded;
}

// Try one folder with both file name spellings (with "_" or with a space).
// Returns true if the file was found and loaded.
bool loadFromFolder(string folder, string name1, string name2, PatientList& patients) {
    string names[2] = { name1, name2 };
    for (int n = 0; n < 2; n++) {
        int loaded = loadCSV(folder + names[n], patients);
        if (loaded >= 0) {
            cout << "Loaded " << loaded << " patients from " << folder + names[n] << endl;
            return true;
        }
    }
    return false;
}

// Look in the usual folders first. Returns true if the file was found.
bool loadDataset(string name1, string name2, PatientList& patients) {
    const int NUM_FOLDERS = 4;
    string folders[NUM_FOLDERS] = { "data/", "", "../data/", "../../data/" };
    for (int f = 0; f < NUM_FOLDERS; f++) {
        if (loadFromFolder(folders[f], name1, name2, patients))
            return true;
    }
    return false;
}

// If the files were not found, ask the user where the CSV files are.
// Tip: in File Explorer, open the folder with the CSV files, click the
// address bar, copy the path (e.g. C:\Users\ASUS\Downloads\data) and paste it.
string askForFolder() {
    string folder;
    cout << endl << "Type or paste the folder that contains the CSV files" << endl
         << "(e.g. C:\\Users\\ASUS\\Downloads\\data), or 0 to skip: ";
    getline(cin, folder);
    if (folder == "0")
        return "";
    // remove quotes added by "Copy as path"
    if (!folder.empty() && folder[0] == '"') folder.erase(0, 1);
    if (!folder.empty() && folder[folder.size() - 1] == '"') folder.erase(folder.size() - 1);
    // make sure the folder ends with a slash
    if (!folder.empty() && folder[folder.size() - 1] != '\\' && folder[folder.size() - 1] != '/')
        folder += "/";
    return folder;
}

int main() {
    PatientList facilityA, facilityB, facilityC;
    bool okA = loadDataset("dataset1_facility_a.csv", "dataset1 facility_a.csv", facilityA);
    bool okB = loadDataset("dataset2_facility_b.csv", "dataset2 facility_b.csv", facilityB);
    bool okC = loadDataset("dataset3_facility_c.csv", "dataset3 facility_c.csv", facilityC);

    // Not found in the usual folders: keep asking until found or the user skips
    while (!okA || !okB || !okC) {
        cout << endl << "Could not find the CSV files in the program's folder." << endl;
        string folder = askForFolder();
        if (folder == "")
            break;
        if (!okA) okA = loadFromFolder(folder, "dataset1_facility_a.csv", "dataset1 facility_a.csv", facilityA);
        if (!okB) okB = loadFromFolder(folder, "dataset2_facility_b.csv", "dataset2 facility_b.csv", facilityB);
        if (!okC) okC = loadFromFolder(folder, "dataset3_facility_c.csv", "dataset3 facility_c.csv", facilityC);
    }

    int choice;
    do {
        cout << endl << "===== MEMBER 2 TEST MENU (LIST) =====" << endl;
        cout << "1. Facility A - General Hospital    (" << facilityA.size() << " patients)" << endl;
        cout << "2. Facility B - University Medical  (" << facilityB.size() << " patients)" << endl;
        cout << "3. Facility C - Community Clinic    (" << facilityC.size() << " patients)" << endl;
        cout << "0. Exit" << endl;
        cout << "Choose a dataset: ";
        choice = readTask4Choice_List(0, 3);

        if (choice == 1)      runTask4_List(facilityA, "Facility A");
        else if (choice == 2) runTask4_List(facilityB, "Facility B");
        else if (choice == 3) runTask4_List(facilityC, "Facility C");
    } while (choice != 0);

    return 0;
}
