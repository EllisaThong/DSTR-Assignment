#ifndef PATIENT_ARRAY_HPP
#define PATIENT_ARRAY_HPP

#include "common.hpp"

using namespace std;

class PatientArray {
private:
    Patient* data;
    int      count;
    int      cap;
    int      resizeCount;
    static const int INITIAL_CAPACITY = 16;

    void grow() {
        int newCap = cap * 2;
        Patient* bigger = new Patient[newCap];
        for (int i = 0; i < count; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        cap = newCap;
        resizeCount++;
    }

    PatientArray(const PatientArray&);
    PatientArray& operator=(const PatientArray&);

public:
    PatientArray() : data(new Patient[INITIAL_CAPACITY]), count(0),
                     cap(INITIAL_CAPACITY), resizeCount(0) {}

    ~PatientArray() { delete[] data; }

    void insertAtEnd(const Patient& p) {
        if (count == cap) grow();
        data[count++] = p;
    }

    void add(const Patient& p) {
        insertAtEnd(p);
    }

    Patient&       at(int i)       { return data[i]; }
    const Patient& at(int i) const { return data[i]; }
    Patient&       operator[](int i)       { return data[i]; }
    const Patient& operator[](int i) const { return data[i]; }

    int size()        const { return count; }
    int capacity()    const { return cap; }
    int resizes()     const { return resizeCount; }
    bool isEmpty()    const { return count == 0; }

    void clear() { count = 0; }

    PatientArray* clone() const {
        PatientArray* copy = new PatientArray();
        for (int i = 0; i < count; i++) copy->insertAtEnd(data[i]);
        return copy;
    }

    int loadFromCSV(const char* filename, int& skipped) {
        ifstream file(filename);
        if (!file.is_open()) return -1;

        skipped = 0;
        int loaded = 0;
        string line;
        getline(file, line);
        while (getline(file, line)) {
            Patient p;
            if (parsePatientLine(line, p)) { insertAtEnd(p); loaded++; }
            else if (!line.empty() && line != "\r") skipped++;
        }
        return loaded;
    }

    size_t memoryBytes() const { return (size_t)cap * sizeof(Patient); }
    size_t usedBytes()   const { return (size_t)count * sizeof(Patient); }
    size_t wastedBytes() const { return memoryBytes() - usedBytes(); }

    void displayAll(int pageSize = 0) const {
        printPatientTableHeader();
        for (int i = 0; i < count; i++) {
            printPatientRow(i + 1, data[i]);
            if (pageSize > 0 && (i + 1) % pageSize == 0 && i + 1 < count) {
                cout << "-- showing " << (i + 1) << " of " << count << " records --";
                waitForEnter();
                printPatientTableHeader();
            }
        }
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total records: " << count << "\n";
    }
};

#endif
