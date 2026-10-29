#include "Hospital.h"
#include <iostream>

Hospital::Hospital(int id, string name, int locationNode,
                   int availableBeds, vector<string> facilities, bool isOpen)
{
    this->id = id;
    this->name = name;
    this->locationNode = locationNode;
    this->availableBeds = availableBeds;
    this->facilities = facilities;
    this->isOpen = isOpen;
}

int Hospital::getId() const
{
    return id;
}

string Hospital::getName() const
{
    return name;
}

int Hospital::getLocationNode() const
{
    return locationNode;
}

int Hospital::getAvailableBeds() const
{
    return availableBeds;
}

bool Hospital::getIsOpen() const
{
    return isOpen;
}

bool Hospital::hasAvailableBed() const
{
    return availableBeds > 0;
}

bool Hospital::hasFacility(const string &facility) const
{
    for (int i = 0; i < facilities.size(); i++)
    {
        if (facilities[i] == facility)
        {
            return true;
        }
    }

    return false;
}

bool Hospital::canTreatEmergency(const string &requiredFacility) const
{
    if (!isOpen)
    {
        return false;
    }

    if (!hasAvailableBed())
    {
        return false;
    }

    return hasFacility(requiredFacility);
}

void Hospital::updateBeds(int change)
{
    availableBeds = availableBeds + change;

    if (availableBeds < 0)
    {
        availableBeds = 0;
    }
}

void Hospital::displayDetails() const
{
    cout << "\nHospital ID: " << id << endl;
    cout << "Name: " << name << endl;
    cout << "Location Node: " << locationNode << endl;
    cout << "Available Beds: " << availableBeds << endl;
    cout << "Status: " << (isOpen ? "Open" : "Closed") << endl;

    cout << "Facilities: ";
    for (int i = 0; i < facilities.size(); i++)
    {
        cout << facilities[i];

        if (i != facilities.size() - 1)
        {
            cout << ", ";
        }
    }

    cout << endl;
}