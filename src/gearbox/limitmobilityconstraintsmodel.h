#ifndef LIMITMOBILITYCONSTRAINTSMODEL_H
#define LIMITMOBILITYCONSTRAINTSMODEL_H

namespace gearbox {

class LimitMobilityConstraintsModel {
public:
  class Constraints {
  public:
    double low;
    double high;
  };

  LimitMobilityConstraintsModel();

  Constraints constraintsForMobility(const double mobility) const;
  bool enabled() const;
  void setEnabled(const bool enabled);

private:
  bool m_enabled;
};

} // namespace gearbox

#endif // LIMITMOBILITYCONSTRAINTSMODEL_H
