#ifndef ANALYTEESTIMATES_H
#define ANALYTEESTIMATES_H

#include <vector>

namespace gearbox {

class AnalyteEstimates {
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

  AnalyteEstimates();
  AnalyteEstimates(const int _chargeLow, const int _chargeHigh,
                   ParameterVec _mobilities, ParameterVec _pKas);
  AnalyteEstimates(const AnalyteEstimates &other);
  AnalyteEstimates(AnalyteEstimates &&other) noexcept;
  AnalyteEstimates & operator=(const AnalyteEstimates &other);
  AnalyteEstimates & operator=(AnalyteEstimates &&other) noexcept;

  const int chargeLow;
  const int chargeHigh;
  const ParameterVec mobilities;
  const ParameterVec pKas;
};

} // namespace gearbox

#endif // ANALYTEESTIMATES_H
