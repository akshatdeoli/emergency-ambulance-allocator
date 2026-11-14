#include <iostream>
#include <limits>
#include <cstdlib>
#include "HospitalManager.h"
#include "AllocationEngine.h"
using namespace std;

const int NUM_NODES = 8;

int readInt(const string &prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            return value;
        }

        if (cin.eof())
        {
            exit(0);
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a number.\n";
    }
}

int main()
{
    Graph city(NUM_NODES);
    city.addRoad(0, 1, 4);
    city.addRoad(1, 2, 3);
    city.addRoad(0, 3, 7);
    city.addRoad(3, 4, 2);
    city.addRoad(2, 5, 6);
    city.addRoad(4, 6, 5);
    city.addRoad(5, 7, 2);

    HospitalManager manager;
    manager.addHospital(Hospital(1, "City Hospital", 2, 3, {"Trauma", "General"}, true));
    manager.addHospital(Hospital(2, "Heart Care", 4, 2, {"Cardiology", "ICU"}, true));
    manager.addHospital(Hospital(3, "Metro Hospital", 1, 0, {"Cardiology"}, true));
    manager.addHospital(Hospital(4, "Night Clinic", 3, 5, {"General"}, false));

    AllocationEngine engine(city, manager);

    string facilityNames[] = {"Cardiology", "Trauma", "General", "ICU"};

    int choice;
    do
    {
        cout << "\n===== EMERGENCY AMBULANCE ALLOCATOR =====\n";
        cout << "1. New emergency\n";
        cout << "2. Show all hospitals\n";
        cout << "3. Exit\n";
        choice = readInt("Enter choice: ");

        if (choice == 1)
        {
            int node = readInt("Patient location node (0-7): ");
            int sev = readInt("Severity (1 = RED, 2 = YELLOW, 3 = GREEN): ");
            int type = readInt("Emergency type (1 = Cardiology, 2 = Trauma, 3 = General, 4 = ICU): ");

            if (node < 0 || node >= NUM_NODES || sev < 1 || sev > 3 || type < 1 || type > 4)
            {
                cout << "Invalid input!\n";
                continue;
            }

            Severity severity = Severity::GREEN;
            if (sev == 1)
            {
                severity = Severity::RED;
            }
            else if (sev == 2)
            {
                severity = Severity::YELLOW;
            }

            Emergency emergency(node, severity, facilityNames[type - 1]);
            AllocationResult result = engine.allocate(emergency);

            if (result.hospital == nullptr)
            {
                cout << "\nNo suitable hospital available! Contact control room.\n";
            }
            else
            {
                cout << "\nStrategy used: " << result.strategy << endl;
                cout << "Distance: " << result.distance << " km" << endl;
                cout << "Send ambulance to:";
                result.hospital->displayDetails();
            }
        }
        else if (choice == 2)
        {
            manager.displayAllHospitals();
        }

    } while (choice != 3);

    cout << "Goodbye!\n";
    return 0;
}
