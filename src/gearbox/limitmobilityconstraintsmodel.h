#ifndef LIMITMOBILITYCONSTRAINTSMODEL_H
#define LIMITMOBILITYCONSTRAINTSMODEL_H

#include <tuple>

namespace gearbox {

class LimitMobilityConstraintsModel {
public:
  using Constraints = std::tuple<double, double>;

  LimitMobilityConstraintsModel();

  Constraints constraintsForMobility(const double mobility) const;
  bool enabled() const;
  void setEnabled(const bool enabled);


private:
  bool m_enabled;
};

} // namespace gearbox

#endif // LIMITMOBILITYCONSTRAINTSMODEL_H
