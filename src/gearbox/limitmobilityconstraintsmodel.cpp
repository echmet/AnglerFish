#include "limitmobilityconstraintsmodel.h"

namespace gearbox {

LimitMobilityConstraintsModel::LimitMobilityConstraintsModel() :
  m_enabled{true}
{
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
