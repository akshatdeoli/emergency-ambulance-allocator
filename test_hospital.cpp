#include <iostream>
#include <vector>
#include "HospitalManager.h"

using namespace std;

int main()
{
    HospitalManager manager;

    Hospital h1(
        1,
        "City Care Hospital",
        2,
        0,
        {"General", "ICU"},
        true);

    Hospital h2(
        2,
        "Trauma Centre",
        3,
        5,
        {"Trauma Care", "ICU", "General"},
        true);

    Hospital h3(
        3,
        "Heart Hospital",
        4,
        3,
        {"Cardiac Care", "ICU"},
        true);

    Hospital h4(
        4,
        "Closed Trauma Hospital",
        5,
        4,
        {"Trauma Care", "General"},
        false);

    manager.addHospital(h1);
    manager.addHospital(h2);
    manager.addHospital(h3);
    manager.addHospital(h4);

    cout << "ALL HOSPITALS" << endl;
    manager.displayAllHospitals();

    string requiredFacility = "Trauma Care";

    cout << "\nEmergency requires: " << requiredFacility << endl;

    vector<Hospital> suitableHospitals =
        manager.findSuitableHospitals(requiredFacility);

    cout << "\nSUITABLE HOSPITALS" << endl;

    if (suitableHospitals.empty())
    {
        cout << "No suitable hospital found." << endl;
    }
    else
    {
        for (int i = 0; i < suitableHospitals.size(); i++)
        {
            suitableHospitals[i].displayDetails();
        }
    }

    cout << "\nRemoving 2 beds from Trauma Centre..." << endl;
    manager.updateHospitalBeds(2, -2);

    Hospital *traumaHospital = manager.findHospitalById(2);

    if (traumaHospital != nullptr)
    {
        traumaHospital->displayDetails();
    }

    return 0;
}