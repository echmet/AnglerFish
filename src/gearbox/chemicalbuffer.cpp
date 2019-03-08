#include "chemicalbuffer.h"
#include "gdmproxyimpl.h"

#include <gdm/conversion/conversion.h>
#include <gdm/core/gdm.h>
#include <calculators/caesinterface.h>
#include <gearbox/utility.h>
#include <trstr.h>
#include <cassert>

namespace gearbox {

static const double MIN_CONCENTRATION{1.0e-11};

ChemicalBuffer::ChemicalBuffer(const gearbox::IonicEffectsModel *ionEffs) :
  h_ionEffs{ionEffs},
  m_gdmModel{new gdm::GDM{}},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_exclude{false},
  m_needsRecalculation{true}
{
}

ChemicalBuffer::ChemicalBuffer(const gearbox::IonicEffectsModel *ionEffs, gdm::GDM *model) :
  h_ionEffs{ionEffs},
  m_gdmModel{model},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_exclude{false},
  m_needsRecalculation{true}
{
  recalculate();
}

ChemicalBuffer::ChemicalBuffer(const ChemicalBuffer &other) :
  h_ionEffs{other.h_ionEffs},
  m_gdmModel{new gdm::GDM{}},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_experimentalMobilities{other.experimentalMobilities()},
  m_pH{other.m_pH},
  m_ionicStrength{other.m_ionicStrength},
  m_exclude{other.m_exclude},
  m_needsRecalculation{other.m_needsRecalculation}
{
  *m_gdmModel = *other.m_gdmModel;
}

ChemicalBuffer::ChemicalBuffer(ChemicalBuffer &&other) noexcept :
  h_ionEffs{other.h_ionEffs},
  m_gdmModel{other.m_gdmModel},
  m_composition{other.m_composition},
  m_experimentalMobilities{std::move(other.m_experimentalMobilities)},
  m_pH{other.m_pH},
  m_ionicStrength{other.m_ionicStrength},
  m_exclude{other.m_exclude},
  m_needsRecalculation{other.m_needsRecalculation}
{
  other.m_gdmModel = nullptr;
  other.m_composition = nullptr;
}

ChemicalBuffer::~ChemicalBuffer()
{
  delete m_composition;
  delete m_gdmModel;
}

GDMProxy & ChemicalBuffer::composition()
{
  return *m_composition;
}

void ChemicalBuffer::correctConcentration(const double targetpH)
{
  static const size_t MAX_ITERS{300};
  static const double PREC{1.0e-6};

  if (m_gdmModel->size() != 2)
    throw Exception{trstr("Automatic correction works with binary buffers only")};

  auto strong = utility::findDrivingConstituent(m_gdmModel);
  if (strong == m_gdmModel->cend())
    throw Exception{trstr("Driving component not identified")};

  /* Find weak component */
  auto weak = m_gdmModel->cbegin();
  for (;weak != m_gdmModel->cend(); weak++) {
    if (weak != strong)
      break;
  }
  assert(weak != m_gdmModel->cend());

  const double cOriginal{m_gdmModel->concentrations(weak).front()};
  double cLeft{m_gdmModel->concentrations(strong).front() * 0.5};
  double cRight{50.0 * cLeft};

  assert(cLeft < cRight);

  if (cLeft > cOriginal)
    throw Exception{trstr("Concentration of weak component must be at least 50 % of the strong component")};

  double cNow = (cRight - cLeft) / 2.0 + cLeft;
  const bool weakIsAcid = utility::isAcid(weak);
  if (!weakIsAcid && !utility::isBase(weak))
    throw Exception{"Weak component cannot be an ampholyte"};

  if ((weakIsAcid && utility::isAcid(strong)) || (!weakIsAcid && utility::isBase(strong)))
    throw Exception{"Buffer must consist of weak acid and strong base or vice versa"};

  size_t iters{0};
  std::vector<double> cVec{cNow};
  auto adjustCNow = [&,this]() -> std::function<void()> {
    if (utility::isAcid(weak)) {
      return [&,this]() {
        if (this->m_pH > targetpH)
          cLeft = cNow;
        else
          cRight = cNow;
      };
    }
    return [&,this]() {
      if (this->m_pH > targetpH)
        cRight = cNow;
      else
        cLeft = cNow;
    };
  }();

  auto pHMatches = [&,this]() {
    return targetpH + PREC > this->m_pH && targetpH - PREC < this->m_pH;
  };

  m_gdmModel->setConcentrations(weak, cVec);
  recalculate(true);
  while (!pHMatches() && iters < MAX_ITERS) {
    adjustCNow();
    cNow = (cRight - cLeft) / 2.0 + cLeft;
    cVec[0] = cNow;
    m_gdmModel->setConcentrations(weak, cVec);

    recalculate(true);
    iters++;
  }

  if (iters >= MAX_ITERS) {
    cVec[0] = cOriginal;
    m_gdmModel->setConcentrations(weak, cVec);
    recalculate(true);

    throw Exception{"Failed to correct concentration"};
  }
}

bool ChemicalBuffer::empty() const
{
  return m_gdmModel->empty();
}

bool ChemicalBuffer::exclude() const
{
  return m_exclude;
}

const std::vector<double> & ChemicalBuffer::experimentalMobilities() const
{
  return m_experimentalMobilities;
}

double ChemicalBuffer::ionicStrength()
{
  recalculate();

  return m_ionicStrength;
}

double ChemicalBuffer::ionicStrength() const
{
  return m_ionicStrength;
}

void ChemicalBuffer::invalidate()
{
  m_needsRecalculation = true;
}

const gdm::GDM * ChemicalBuffer::model() const
{
  return m_gdmModel;
}

double ChemicalBuffer::pH()
{
  recalculate();

  return m_pH;
}

double ChemicalBuffer::pH() const
{
  return m_pH;
}

void ChemicalBuffer::recalculate(const bool force)
{
  if (!m_needsRecalculation && !force)
    return;

  CAESInterface iface{*m_gdmModel, *h_ionEffs};

  try {
    auto props = iface.bufferProperties();

    m_pH = props.pH;
    m_ionicStrength = props.ionicStrength * 1000.0;

    m_needsRecalculation = false;
  } catch (const CAESInterface::Exception &ex) {
    throw Exception{ex.what()};
  }
}

void ChemicalBuffer::setExclude(const bool exclude)
{
  m_exclude = exclude;
}

void ChemicalBuffer::setExperimentalMobilities(std::vector<double> mobilities)
{
  m_experimentalMobilities = std::move(mobilities);
}

ChemicalBuffer & ChemicalBuffer::operator=(ChemicalBuffer &&other) noexcept
{
  h_ionEffs = other.h_ionEffs;
  m_gdmModel = other.m_gdmModel;
  m_composition = other.m_composition;
  m_experimentalMobilities = std::move(other.m_experimentalMobilities);
  m_pH = other.m_pH;
  m_ionicStrength = other.m_ionicStrength;
  m_needsRecalculation = other.m_needsRecalculation;

  other.m_gdmModel = nullptr;
  other.m_composition = nullptr;

  return *this;
}

} // namespace gearbox
