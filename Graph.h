#pragma once
#include <vector>
#include <queue>
#include <climits>

using namespace std;

class Graph
{
private:
    int numNodes;
    vector<vector<pair<int, int>>> adj;

public:
    Graph(int numNodes)
    {
        this->numNodes = numNodes;
        adj.resize(numNodes);
    }

    void addRoad(int u, int v, int distance)
    {
        adj[u].push_back(make_pair(v, distance));
        adj[v].push_back(make_pair(u, distance));
    }

    vector<int> dijkstra(int source) const
    {
        vector<int> dist(numNodes, INT_MAX);

        priority_queue<pair<int, int>,
                       vector<pair<int, int>>,
                       greater<pair<int, int>>> pq;

        dist[source] = 0;
        pq.push(make_pair(0, source));

        while (!pq.empty())
        {
            int currentDist = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (currentDist > dist[u])
            {
                continue;
            }

            for (size_t i = 0; i < adj[u].size(); i++)
            {
                int v = adj[u][i].first;
                int roadLength = adj[u][i].second;

                if (dist[u] + roadLength < dist[v])
                {
                    dist[v] = dist[u] + roadLength;
                    pq.push(make_pair(dist[v], v));
                }
            }
        }

        return dist;
    }
};
