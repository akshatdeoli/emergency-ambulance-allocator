#pragma once
#include <functional>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "common.h"

class RoadGraph {
public:
    struct Edge { int to; double minutes; };

    int addNode(const std::string& name, Location loc) {
        nodes_.push_back({name, loc});
        adj_.emplace_back();
        return static_cast<int>(nodes_.size()) - 1;
    }

    void addRoad(int a, int b, double speedKmph) {
        if (a < 0 || b < 0 || a >= size() || b >= size() || speedKmph <= 0)
            throw std::invalid_argument("addRoad: bad node id or speed");
        double km = haversineKm(nodes_[a].loc, nodes_[b].loc);
        double minutes = km / speedKmph * 60.0;
        adj_[a].push_back({b, minutes});
        adj_[b].push_back({a, minutes});
    }
    int nearestNode(const Location& p) const {
        int best = -1;
        double bestKm = INF_MIN;
        for (int i = 0; i < size(); ++i) {
            double d = haversineKm(p, nodes_[i].loc);
            if (d < bestKm) { bestKm = d; best = i; }
        }
        return best;
    }

 
    std::vector<double> shortestTimes(int source) const {
        std::vector<double> dist(nodes_.size(), INF_MIN);
        using Item = std::pair<double, int>;  // (time, node)
        std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;  // min-heap
        dist[source] = 0.0;
        pq.push({0.0, source});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;  // outdated queue entry, skip
            for (const Edge& e : adj_[u]) {
                double nd = d + e.minutes;
                if (nd < dist[e.to]) {
                    dist[e.to] = nd;
                    pq.push({nd, e.to});
                }
            }
        }
        return dist;
    }

    const std::vector<Edge>& edgesFrom(int i) const { return adj_.at(i); }
    int size() const { return static_cast<int>(nodes_.size()); }
    const std::string& nodeName(int i) const { return nodes_.at(i).name; }
    Location nodeLocation(int i) const { return nodes_.at(i).loc; }

private:
    struct Node { std::string name; Location loc; };
    std::vector<Node> nodes_;
    std::vector<std::vector<Edge>> adj_;  // adjacency list
};
