Emergency Ambulance Allocator 🚑

A C++ project that helps allocate emergency patients to the most suitable hospital based on road distance, bed availability, hospital facilities, and emergency severity.

About the Project

In an emergency, choosing the nearest hospital isn't always the best option. It might be full, closed, or unable to provide the treatment a patient needs.

The Emergency Ambulance Allocator aims to solve this problem by considering more than just distance. It finds hospitals that can actually handle the emergency and selects the most suitable one based on the patient's condition.

How It Works

The user provides three details:

Patient Location: A junction (node) on the city road map.

Severity: RED (life-threatening), YELLOW (serious), or GREEN (minor).

Emergency Type: Cardiology, Trauma, General, or ICU.

The system then:

Calculates the shortest road distance from the patient's location to every hospital using Dijkstra's algorithm.

Filters out hospitals that are closed, have no available beds, or lack the required medical facility.

Evaluates the remaining hospitals according to the patient's emergency severity.

Ranks the hospitals using a priority queue (min-heap).

Selects the most suitable hospital and reserves one bed.

Hospital Selection Strategy

Different emergencies require different priorities when selecting a hospital.

Severity

Selection Strategy

🔴 RED

Prioritizes the nearest capable hospital because every second matters.

🟡 YELLOW

Balances distance and available beds.

🟢 GREEN

Prefers hospitals with more available beds to avoid overcrowding.

Concepts Used

Object-Oriented Programming (OOP)

Encapsulation: Hospital and Emergency classes keep their data private and provide access through getters.

Abstraction: Scorer is an abstract class that defines the scoring interface.

Inheritance: RedScorer, YellowScorer, and GreenScorer inherit from the Scorer class.

Polymorphism: Different scoring strategies are implemented through the same interface.

Composition: AllocationEngine uses Graph and HospitalManager to perform hospital allocation.

Data Structures and Algorithms (DSA)

Graph (Adjacency List): Represents the city's road network.

Dijkstra's Algorithm: Finds the shortest distance from the patient's location to hospitals.

Priority Queue (Min-Heap): Ranks suitable hospitals according to their scores.
