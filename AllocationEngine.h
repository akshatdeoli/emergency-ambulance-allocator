#pragma once
#include <vector>
#include <queue>
#include <climits>
#include <memory>
#include <string>
#include "Graph.h"
#include "Emergency.h"
#include "Scorer.h"
#include "HospitalManager.h"

using namespace std;

struct Candidate
{
    int hospitalId;
    int distance;
    double score;

    bool operator>(const Candidate &other) const
    {
        return score > other.score;
    }
};

struct AllocationResult
{
    Hospital *hospital;
    int distance;
    string strategy;
};

class AllocationEngine
{
private:
    const Graph &graph;
    HospitalManager &manager;

public:
    AllocationEngine(const Graph &graph, HospitalManager &manager)
        : graph(graph), manager(manager)
    {
    }

    AllocationResult allocate(const Emergency &emergency)
    {
        vector<int> dist = graph.dijkstra(emergency.getPatientNode());

        unique_ptr<Scorer> scorer = makeScorer(emergency.getSeverity());

        vector<Hospital> suitable =
            manager.findSuitableHospitals(emergency.getRequiredFacility());

        priority_queue<Candidate, vector<Candidate>, greater<Candidate>> heap;

        for (size_t i = 0; i < suitable.size(); i++)
        {
            const Hospital &h = suitable[i];

            if (!h.canTreatEmergency(emergency.getRequiredFacility()))
            {
                continue;
            }

            int d = dist[h.getLocationNode()];
            if (d == INT_MAX)
            {
                continue;
            }

            Candidate c;
            c.hospitalId = h.getId();
            c.distance = d;
            c.score = scorer->score(d, h.getAvailableBeds());
            heap.push(c);
        }

        AllocationResult result;
        result.strategy = scorer->name();

        if (heap.empty())
        {
            result.hospital = nullptr;
            result.distance = -1;
            return result;
        }

        Candidate best = heap.top();
        manager.updateHospitalBeds(best.hospitalId, -1);

        result.hospital = manager.findHospitalById(best.hospitalId);
        result.distance = best.distance;
        return result;
    }
};
