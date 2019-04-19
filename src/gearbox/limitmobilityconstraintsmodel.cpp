#include "limitmobilityconstraintsmodel.h"

#include <gearbox/gearbox.h>
#include <calculators/empfitterinterface.h>

namespace gearbox {

LimitMobilityConstraintsModel::LimitMobilityConstraintsModel() :
  m_enabled{true}
{
}

LimitMobilityConstraintsModel::Constraints LimitMobilityConstraintsModel::constraintsForCharge(const int charge,
                                                                                               Gearbox &gbox) const
{
  calculators::EMPFitterInterface iface{gbox, false};

  auto ret = iface.mobilityConstraintsForCharge(charge);

  return { ret.low, ret.high };
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
