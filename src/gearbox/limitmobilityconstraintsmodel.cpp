#include "limitmobilityconstraintsmodel.h"

#include <calculators/empfitterinterface.h>

namespace gearbox {

LimitMobilityConstraintsModel::LimitMobilityConstraintsModel() :
  m_enabled{true}
{
}

LimitMobilityConstraintsModel::Constraints LimitMobilityConstraintsModel::constraintsForMobility(const double mobility) const
{
  auto ctrs = calculators::EMPFitterInterface::mobilityConstraints(mobility);

  return { mobility - ctrs.low, mobility + ctrs.high };
}

bool LimitMobilityConstraintsModel::enabled() const
{
  return m_enabled;
}

void LimitMobilityConstraintsModel::setEnabled(const bool enabled)
{
  m_enabled = enabled;
}

} // namespace gearbox
