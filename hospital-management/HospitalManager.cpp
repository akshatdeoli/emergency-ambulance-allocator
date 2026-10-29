#include "HospitalManager.h"
#include <iostream>

void HospitalManager::addHospital(const Hospital &hospital)
{
    hospitals.push_back(hospital);
}

void HospitalManager::displayAllHospitals() const
{
    if (hospitals.empty())
    {
        cout << "No hospitals found." << endl;
        return;
    }

    for (int i = 0; i < hospitals.size(); i++)
    {
        hospitals[i].displayDetails();
    }
}

vector<Hospital> HospitalManager::findSuitableHospitals(
    const string &requiredFacility) const
{
    vector<Hospital> suitableHospitals;

    for (int i = 0; i < hospitals.size(); i++)
    {
        if (hospitals[i].canTreatEmergency(requiredFacility))
        {
            suitableHospitals.push_back(hospitals[i]);
        }
    }

    return suitableHospitals;
}

Hospital *HospitalManager::findHospitalById(int id)
{
    for (int i = 0; i < hospitals.size(); i++)
    {
        if (hospitals[i].getId() == id)
        {
            return &hospitals[i];
        }
    }

    return nullptr;
}

bool HospitalManager::updateHospitalBeds(int id, int change)
{
    Hospital *hospital = findHospitalById(id);

    if (hospital == nullptr)
    {
        return false;
    }

    hospital->updateBeds(change);
    return true;
}
