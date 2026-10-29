#pragma once

#include <string>
#include <vector>

using namespace std;

class Hospital
{
private:
    int id;
    string name;
    int locationNode;
    int availableBeds;
    vector<string> facilities;
    bool isOpen;

public:
    Hospital(int id, string name, int locationNode,
             int availableBeds, vector<string> facilities, bool isOpen);

    int getId() const;
    string getName() const;
    int getLocationNode() const;
    int getAvailableBeds() const;
    bool getIsOpen() const;

    void displayDetails() const;
    bool hasAvailableBed() const;
    bool hasFacility(const string &facility) const;
    bool canTreatEmergency(const string &requiredFacility) const;
    void updateBeds(int change);
};
