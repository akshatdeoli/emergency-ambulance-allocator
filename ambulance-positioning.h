#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include "MinCostFlow.h"
#include "RoadGraph.h"
#include "Scenario.h"   // randInt / rand01 (portable RNG helpers)

struct Coverage {
    double meanMin = 0;       // demand-weighted mean response (minutes)
    double worstMin = 0;      // worst response among demand-bearing cells
    double coveredShare = 0;  // share of demand reachable within the target radius
    double cost = 0;          // the objective the planners minimise
};

struct MovePlan { int ambulanceId; int fromNode; int toNode; double minutes; };

struct PositioningPlan {
    std::vector<int> targets;                 // chosen junctions
    std::vector<MovePlan> moves;
    std::vector<int> beforeNodes, afterNodes; // per-ambulance junction before / after
    Coverage before, after;
    double totalMoveMinutes = 0;
};

class CoverageModel {
public:
    struct Cell { Location center; int node; double offRoadMin; double weight; };

    static constexpr double kOffRoadKmph = 25.0;

    CoverageModel(const RoadGraph& g, int rows, int cols, const std::vector<double>& nodeDemand,
                  int historySize, std::mt19937& rng, double radiusMin = 8.0, double worstWeight = 0.25)
        : g_(g), rows_(rows), cols_(cols), radius_(radiusMin), worstW_(worstWeight) {
        const int V = g.size();
        times_.resize(V);
        for (int v = 0; v < V; ++v) times_[v] = g.shortestTimes(v);

        minLat_ = minLon_ = INF_MIN; maxLat_ = maxLon_ = -INF_MIN;
        for (int v = 0; v < V; ++v) {
            Location p = g.nodeLocation(v);
            minLat_ = std::min(minLat_, p.lat); maxLat_ = std::max(maxLat_, p.lat);
            minLon_ = std::min(minLon_, p.lon); maxLon_ = std::max(maxLon_, p.lon);
        }
        double padLat = 0.08 * (maxLat_ - minLat_) + 0.002, padLon = 0.08 * (maxLon_ - minLon_) + 0.002;
        minLat_ -= padLat; maxLat_ += padLat; minLon_ -= padLon; maxLon_ += padLon;

        cells_.resize(static_cast<size_t>(rows) * cols);
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c) {
                Cell& cell = cells_[r * cols + c];
                cell.center = {minLat_ + (r + 0.5) * (maxLat_ - minLat_) / rows,
                               minLon_ + (c + 0.5) * (maxLon_ - minLon_) / cols};
                cell.node = g.nearestNode(cell.center);
                cell.offRoadMin = haversineKm(cell.center, g.nodeLocation(cell.node)) / kOffRoadKmph * 60.0;
            }

        // simulated incident history: pick a junction by demand, jitter ~500 m
        std::vector<double> cum(V);
        double total = 0;
        for (int v = 0; v < V; ++v) { total += std::max(0.0, nodeDemand.at(v)); cum[v] = total; }
        std::vector<double> count(cells_.size(), 0.0);
        for (int h = 0; h < historySize; ++h) {
            double u = rand01(rng) * total;
            int v = static_cast<int>(std::lower_bound(cum.begin(), cum.end(), u) - cum.begin());
            v = std::min(v, V - 1);
            double u1 = std::max(rand01(rng), 1e-12), u2 = rand01(rng);
            double mag = std::sqrt(-2.0 * std::log(u1)) * 0.005;       // Box-Muller, sigma 0.005 deg
            Location p = g.nodeLocation(v);
            p.lat += mag * std::cos(6.283185307179586 * u2);
            p.lon += mag * std::sin(6.283185307179586 * u2);
            int r = clampIdx((p.lat - minLat_) / (maxLat_ - minLat_) * rows, rows);
            int c = clampIdx((p.lon - minLon_) / (maxLon_ - minLon_) * cols, cols);
            count[r * cols + c] += 1.0;
        }
        double norm = 0;
        for (double& x : count) { x += 0.05; norm += x; }              // smoothing: no cell has zero demand
        for (size_t i = 0; i < cells_.size(); ++i) cells_[i].weight = count[i] / norm;
    }

    // Response time (min) from the nearest chosen junction to one cell.
    double responseFor(const Cell& c, const std::vector<int>& nodes) const {
        double best = INF_MIN;
        for (int s : nodes) best = std::min(best, times_[s][c.node]);
        return best == INF_MIN ? 999.0 : c.offRoadMin + best;
    }

    std::vector<double> cellResponse(const std::vector<int>& nodes) const {
        std::vector<double> out;
        for (const Cell& c : cells_) out.push_back(responseFor(c, nodes));
        return out;
    }

    Coverage evaluate(const std::vector<int>& nodes) const {
        Coverage cv;
        const double uniform = 1.0 / cells_.size();
        for (const Cell& c : cells_) {
            double r = nodes.empty() ? 999.0 : responseFor(c, nodes);
            cv.meanMin += c.weight * r;
            if (c.weight >= 0.5 * uniform) cv.worstMin = std::max(cv.worstMin, r);
            if (r <= radius_) cv.coveredShare += c.weight;
        }
        cv.cost = cv.meanMin + worstW_ * cv.worstMin;
        return cv;
    }

    // Greedy construction, then swap local search until no swap helps.
    std::vector<int> planHeuristic(int k) const {
        k = std::min(k, g_.size());
        std::vector<int> S;
        auto has = [&](int n) { return std::find(S.begin(), S.end(), n) != S.end(); };
        for (int step = 0; step < k; ++step) {
            int best = -1; double bestCost = INF_MIN;
            for (int n = 0; n < g_.size(); ++n) {
                if (has(n)) continue;
                S.push_back(n);
                double c = evaluate(S).cost;
                S.pop_back();
                if (c < bestCost) { bestCost = c; best = n; }
            }
            S.push_back(best);
        }
        bool improved = true;
        while (improved) {
            improved = false;
            for (size_t idx = 0; idx < S.size(); ++idx)
                for (int n = 0; n < g_.size(); ++n) {
                    if (has(n)) continue;
                    std::vector<int> T = S; T[idx] = n;
                    if (evaluate(T).cost < evaluate(S).cost - 1e-9) { S = T; improved = true; }
                }
        }
        std::sort(S.begin(), S.end());
        return S;
    }

    // Exact optimum by enumerating all C(V,k) sets (falls back if too many).
    std::vector<int> planExact(int k) const {
        const int V = g_.size();
        k = std::min(k, V);
        double combos = 1;
        for (int i = 1; i <= k; ++i) combos = combos * (V - k + i) / i;
        if (combos > 3e6) return planHeuristic(k);
        std::vector<int> cur, best;
        double bestCost = INF_MIN;
        std::function<void(int)> rec = [&](int start) {
            if (static_cast<int>(cur.size()) == k) {
                double c = evaluate(cur).cost;
                if (c < bestCost) { bestCost = c; best = cur; }
                return;
            }
            for (int n = start; n < V; ++n) { cur.push_back(n); rec(n + 1); cur.pop_back(); }
        };
        rec(0);
        return best;
    }

    // idle = (ambulanceId, currentJunction). Pick best spots, then assign
    // ambulances to spots with minimum total driving time (matching via flow).
    PositioningPlan reposition(const std::vector<std::pair<int, int>>& idle, bool exact = true) const {
        PositioningPlan plan;
        const int a = static_cast<int>(idle.size());
        if (a == 0) return plan;
        for (auto& p : idle) plan.beforeNodes.push_back(p.second);
        plan.before = evaluate(plan.beforeNodes);

        const int m = std::min(a, g_.size());
        plan.targets = exact ? planExact(m) : planHeuristic(m);

        const int SRC = 0, SINK = 1 + a + m;
        MinCostFlow net(SINK + 1);
        std::vector<std::vector<std::pair<int, int>>> arcs(a);
        for (int i = 0; i < a; ++i) {
            net.addEdge(SRC, 1 + i, 1, 0);
            for (int j = 0; j < m; ++j) {
                double t = times_[idle[i].second][plan.targets[j]];
                if (t == INF_MIN) continue;
                arcs[i].push_back({net.addEdge(1 + i, 1 + a + j, 1, std::llround(t * 1000.0)), j});
            }
        }
        for (int j = 0; j < m; ++j) net.addEdge(1 + a + j, SINK, 1, 0);
        net.solve(SRC, SINK, m);

        plan.afterNodes = plan.beforeNodes;
        for (int i = 0; i < a; ++i)
            for (auto [edge, j] : arcs[i])
                if (net.flowOn(edge) == 1) {
                    int to = plan.targets[j];
                    double t = times_[idle[i].second][to];
                    plan.afterNodes[i] = to;
                    if (to != idle[i].second) plan.moves.push_back({idle[i].first, idle[i].second, to, t});
                    plan.totalMoveMinutes += t;
                }
        plan.after = evaluate(plan.afterNodes);
        return plan;
    }

    const std::vector<Cell>& cells() const { return cells_; }
    int rows() const { return rows_; }
    int cols() const { return cols_; }
    double radius() const { return radius_; }
    double minLat() const { return minLat_; } double maxLat() const { return maxLat_; }
    double minLon() const { return minLon_; } double maxLon() const { return maxLon_; }

private:
    static int clampIdx(double v, int n) { return std::max(0, std::min(n - 1, static_cast<int>(v))); }

    const RoadGraph& g_;
    int rows_, cols_;
    double radius_, worstW_;
    double minLat_, maxLat_, minLon_, maxLon_;
    std::vector<Cell> cells_;
    std::vector<std::vector<double>> times_;   // all-pairs junction travel times
};
