#ifndef CHEMICALBUFFER_H
#define CHEMICALBUFFER_H

#include <stdexcept>
#include <vector>

namespace gdm {
  class GDM;
} // namespace gdm

class GDMProxy;

class ChemicalBuffer {
public:
  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  ChemicalBuffer();
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

private:
  void recalculate();

  gdm::GDM *m_gdmModel;
  GDMProxy *m_composition;

  std::vector<double> m_experimentalMobilities;

  double m_pH;
  double m_ionicStrength;

  bool m_needsRecalculation;
};

#endif // CHEMICALBUFFER_H
