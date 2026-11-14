
#pragma once

#include <string>

enum class Severity
{
    RED,
    YELLOW,
    GREEN
};

class Emergency
{
private:
    int patientNode;
    Severity severity;
    std::string requiredFacility;

public:
    Emergency(int patientNode,
              Severity severity,
              const std::string& requiredFacility)
        : patientNode(patientNode),
          severity(severity),
          requiredFacility(requiredFacility)
    {
    }

    int getPatientNode() const
    {
        return patientNode;
    }

    Severity getSeverity() const
    {
        return severity;
    }

    std::string getRequiredFacility() const
    {
        return requiredFacility;
    }
};
