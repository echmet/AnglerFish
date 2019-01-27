#include "analyteestimates.h"

namespace gearbox {

AnalyteEstimates::Parameter::Parameter(const double v, const bool f) :
  value{v},
  fixed{f}
{
}

AnalyteEstimates::Parameter::Parameter(const Parameter &other) noexcept :
  value{other.value},
  fixed{other.fixed}
{
}

AnalyteEstimates::Parameter & AnalyteEstimates::Parameter::operator=(const Parameter &other) noexcept
{
  const_cast<double&>(value) = other.value;
  const_cast<bool&>(fixed) = other.fixed;

  return *this;
}

AnalyteEstimates::AnalyteEstimates() :
  chargeLow{0},
  chargeHigh{0}
{
}

AnalyteEstimates::AnalyteEstimates(const int _chargeLow, const int _chargeHigh,
                                   ParameterVec _mobilities, ParameterVec _pKas) :
  chargeLow{_chargeLow},
  chargeHigh{_chargeHigh},
  mobilities{std::move(_mobilities)},
  pKas{std::move(_pKas)}
{
}

AnalyteEstimates::AnalyteEstimates(const AnalyteEstimates &other) :
  chargeLow{other.chargeLow},
  chargeHigh{other.chargeHigh},
  mobilities{other.mobilities},
  pKas{other.pKas}
{
}

AnalyteEstimates::AnalyteEstimates(AnalyteEstimates &&other) noexcept :
  chargeLow{other.chargeLow},
  chargeHigh{other.chargeHigh},
  mobilities{std::move(const_cast<ParameterVec&>(other.mobilities))},
  pKas{std::move(const_cast<ParameterVec&>(other.pKas))}
{
}

AnalyteEstimates & AnalyteEstimates::operator=(const AnalyteEstimates &other)
{
  const_cast<int&>(chargeLow) = other.chargeLow;
  const_cast<int&>(chargeHigh) = other.chargeHigh;
  const_cast<ParameterVec&>(mobilities) = other.mobilities;
  const_cast<ParameterVec&>(pKas) = other.pKas;

  return *this;
}

AnalyteEstimates & AnalyteEstimates::operator=(AnalyteEstimates &&other) noexcept
{
  const_cast<int&>(chargeLow) = other.chargeLow;
  const_cast<int&>(chargeHigh) = other.chargeHigh;
  const_cast<ParameterVec&>(mobilities) = std::move(other.mobilities);
  const_cast<ParameterVec&>(pKas) = std::move(other.pKas);

  return *this;
}

} // namespace gearbox
