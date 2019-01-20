#ifndef ANALYTEINPUTPARAMETERS_H
#define ANALYTEINPUTPARAMETERS_H

#include <vector>

class AnalyteInputParameters {
public:
  class Parameter {
  public:
    const double value;
    const bool fixed;

    Parameter(const double v, const bool f);
    Parameter(const Parameter &other) noexcept;
    Parameter & operator=(const Parameter &other) noexcept;
  };

  using ParameterVec = std::vector<Parameter>;

  AnalyteInputParameters(const int _chargeLow, const int _chargeHigh,
                         ParameterVec _mobilities, ParameterVec _pKas);
  AnalyteInputParameters(const AnalyteInputParameters &other);
  AnalyteInputParameters(AnalyteInputParameters &&other) noexcept;
  AnalyteInputParameters & operator=(const AnalyteInputParameters &other);
  AnalyteInputParameters & operator=(AnalyteInputParameters &&other) noexcept;

  const int chargeLow;
  const int chargeHigh;
  const ParameterVec mobilities;
  const ParameterVec pKas;
};

#endif // ANALYTEINPUTPARAMETERS_H
