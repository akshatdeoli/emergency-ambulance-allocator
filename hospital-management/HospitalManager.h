#pragma once

#include "Hospital.h"
#include <vector>

using namespace std;

class HospitalManager
{
private:
    vector<Hospital> hospitals;

public:
    void addHospital(const Hospital &hospital);

    void displayAllHospitals() const;

    vector<Hospital> findSuitableHospitals(
        const string &requiredFacility) const;

    Hospital *findHospitalById(int id);

    bool updateHospitalBeds(int id, int change);
};
