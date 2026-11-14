Emergency Ambulance Allocator

A  project that sends a patient to the best hospital, not just the nearest one.

Problem

In an emergency, ambulances usually go to the nearest hospital without checking whether it can actually treat the patient. That hospital may be closed, full, or missing the required facility or doctor.

Our Solution

The user enters:

Patient location (a junction on the city road map)
Severity: RED (life threatening), YELLOW (serious), GREEN (minor)
Emergency type: Cardiology, Trauma, General, ICU

The system then:

Finds the shortest road distance to every hospital (Dijkstra's algorithm)
Removes hospitals that are closed, have no free beds, or lack the required facility
Scores the remaining hospitals based on severity
Ranks them with a priority queue (min-heap) and picks the best one
Reserves a bed at that hospital
Severity	Strategy
RED	Nearest capable hospital (speed matters most)
YELLOW	Balance of distance and free beds
GREEN	Prefers a less crowded hospital
Concepts Used

OOP (C++)

Encapsulation: Hospital, Emergency (private data with getters)
Abstraction: Scorer (abstract class)
Inheritance and Polymorphism: RedScorer, YellowScorer, GreenScorer
Composition: AllocationEngine uses Graph and HospitalManager

DSA

Graph (adjacency list)
Dijkstra's shortest path algorithm
Priority queue (min-heap)
Project Structure
emergency-ambulance-allocator/
├── hospital-management/
│   ├── Hospital.h / Hospital.cpp
│   ├── HospitalManager.h / HospitalManager.cpp
│   └── test_hospital.cpp
├── emergency-management/
│   ├── Emergency.h
│   └── Scorer.h
├── Graph.h
├── AllocationEngine.h
├── main.cpp
└── README.md


How to Compile and Run

g++ -std=c++17 main.cpp hospital-management/Hospital.cpp hospital-management/HospitalManager.cpp -Ihospital-management -Iemergency-management -o allocator
allocator.exe


Demo Example

Patient at node 0, severity RED, Cardiology:

Metro Hospital is the nearest, but it has 0 beds, so it is skipped.
Heart Care is selected and one of its beds is reserved.
Team


Future Work
Load hospitals from a file
Real city map with named locations
Web frontend with a map
