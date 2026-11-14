
#pragma once

#include <string>
#include <memory>
#include "Emergency.h"

using namespace std;

class Scorer
{
public:
    virtual double score(int distance, int beds) const = 0;

    virtual string name() const = 0;

    virtual ~Scorer() {}
};

class RedScorer : public Scorer
{
public:
    double score(int distance, int beds) const override
    {
        return distance;
    }

    string name() const override
    {
        return "RED: nearest capable hospital (speed is most important)";
    }
};

class YellowScorer : public Scorer
{
public:
    double score(int distance, int beds) const override
    {
        return distance - 2.0 * beds;
    }

    string name() const override
    {
        return "YELLOW: balance of distance and free beds";
    }
};

class GreenScorer : public Scorer
{
public:
    double score(int distance, int beds) const override
    {
        return distance - 5.0 * beds;
    }

    string name() const override
    {
        return "GREEN: prefers less crowded hospital";
    }
};

inline unique_ptr<Scorer> makeScorer(Severity severity)
{
    if (severity == Severity::RED)
    {
        return make_unique<RedScorer>();
    }
    else if (severity == Severity::YELLOW)
    {
        return make_unique<YellowScorer>();
    }
    else
    {
        return make_unique<GreenScorer>();
    }
}
