#ifndef PATIENT_LIST_HPP
#define PATIENT_LIST_HPP

#include "common.hpp"

using namespace std;

struct PatientNode {
    Patient      data;
    PatientNode* next;
    PatientNode(const Patient& p) : data(p), next(NULL) {}
};

class PatientList {
private:
    PatientNode* head;
    PatientNode* tail;
    int          count;

    PatientList(const PatientList&);
    PatientList& operator=(const PatientList&);

public:
    PatientList() : head(NULL), tail(NULL), count(0) {}

    ~PatientList() { clear(); }

    void insertAtEnd(const Patient& p) {
        PatientNode* node = new PatientNode(p);
        if (head == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        count++;
    }

    void add(const Patient& p) {
        insertAtEnd(p);
    }

    void clear() {
        PatientNode* cur = head;
        while (cur != NULL) {
            PatientNode* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
        head = tail = NULL;
        count = 0;
    }

    PatientNode* getHead() const { return head; }
    PatientNode* getTail() const { return tail; }

    void setHead(PatientNode* newHead) {
        head = newHead;
        tail = head;
        count = 0;
        if (head != NULL) {
            count = 1;
            while (tail->next != NULL) {
                tail = tail->next;
                count++;
            }
        }
    }

    int  size()    const { return count; }
    bool isEmpty() const { return count == 0; }

    PatientList* clone() const {
        PatientList* copy = new PatientList();
        for (PatientNode* cur = head; cur != NULL; cur = cur->next)
            copy->insertAtEnd(cur->data);
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

    size_t memoryBytes()  const { return (size_t)count * sizeof(PatientNode); }
    size_t dataBytes()    const { return (size_t)count * sizeof(Patient); }
    size_t overheadBytes() const { return memoryBytes() - dataBytes(); }

    void displayAll(int pageSize = 0) const {
        printPatientTableHeader();
        int i = 0;
        for (PatientNode* cur = head; cur != NULL; cur = cur->next) {
            i++;
            printPatientRow(i, cur->data);
            if (pageSize > 0 && i % pageSize == 0 && i < count) {
                cout << "-- showing " << i << " of " << count << " records --";
                waitForEnter();
                printPatientTableHeader();
            }
        }
        printLine(PATIENT_TABLE_WIDTH);
        cout << "Total records: " << count << "\n";
    }
};

#endif
