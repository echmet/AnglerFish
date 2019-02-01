#ifndef CHEMICALBUFFER_H
#define CHEMICALBUFFER_H

#include <stdexcept>
#include <vector>

namespace gearbox {
  class GDMProxy;
  class IonicEffectsModel;
} // namespace gearbox

namespace gdm {
  class GDM;
} // namespace gdm

namespace gearbox {

class ChemicalBuffer {
public:
  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  ChemicalBuffer(const IonicEffectsModel *ionEffs);
  ChemicalBuffer(const IonicEffectsModel *ionEffs, gdm::GDM *model);
  ChemicalBuffer(const ChemicalBuffer &other);
  ChemicalBuffer(ChemicalBuffer &&other) noexcept;
  ~ChemicalBuffer();

  GDMProxy & composition();
  bool empty() const;
  const std::vector<double> & experimentalMobilities() const;
  double ionicStrength();
  double ionicStrength() const;
  void invalidate();
  const gdm::GDM * model() const;
  double pH();
  double pH() const;
  void setExperimentalMobilities(std::vector<double> mobilities);

  ChemicalBuffer & operator=(ChemicalBuffer &&other) noexcept;

private:
  void recalculate();

  const IonicEffectsModel *h_ionEffs;
  gdm::GDM *m_gdmModel;
  GDMProxy *m_composition;

  std::vector<double> m_experimentalMobilities;

  double m_pH;
  double m_ionicStrength;

  bool m_needsRecalculation;
};

} // namespace gearbox

#endif // CHEMICALBUFFER_H
