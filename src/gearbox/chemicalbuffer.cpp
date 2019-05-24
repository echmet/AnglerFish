#include "chemicalbuffer.h"
#include "gdmproxyimpl.h"

#include <gdm/conversion/conversion.h>
#include <calculators/caesinterface.h>
#include <gearbox/utility.h>
#include <trstr.h>
#include <cassert>

namespace gearbox {

static const double MIN_CONCENTRATION{1.0e-11};
static const double CCORR_PREC{1.0e-6};

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
  m_bufferCapacity{other.m_bufferCapacity},
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
  m_bufferCapacity{other.m_bufferCapacity},
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

double ChemicalBuffer::bufferCapacity()
{
  recalculate();

  return m_bufferCapacity;
}

double ChemicalBuffer::bufferCapacity() const
{
  return m_bufferCapacity;
}

GDMProxy & ChemicalBuffer::composition()
{
  return *m_composition;
}

void ChemicalBuffer::correctConcentration(const double targetpH, const std::string &name)
{
  if (m_gdmModel->empty())
    throw Exception{trstr("No constituents")};

  const auto ctuent = m_gdmModel->find(name);
  if (ctuent == m_gdmModel->cend())
    throw Exception{trstr("Constituent not found")};

  const double cLeft{CCORR_PREC};
  const double cRight  = [&]() {
    if (m_gdmModel->size() == 1)
      return 500.0;

    double cMax{0.0};

    for (auto it = m_gdmModel->cbegin(); it != m_gdmModel->cend(); ++it) {
      double c = m_gdmModel->concentrations(it).at(0);
      if (c > cMax)
        cMax = c;
    }

    return 50.0 * cMax;
  }();

  correctConcentrationInternal(ctuent, cLeft, cRight, targetpH);
}

void ChemicalBuffer::correctConcentrationInternal(const gdm::GDM::const_iterator weak, double cLeft, double cRight,
                                                  const double targetpH)
{
  static const size_t MAX_ITERS{300};

  const double cOriginal{m_gdmModel->concentrations(weak).front()};

  if (cRight < 2.0 * CCORR_PREC)
    cRight = 2.0 * CCORR_PREC;
  assert(cLeft < cRight);

  double cNow = (cRight - cLeft) / 2.0 + cLeft;

  std::vector<double> cVec{cLeft};
  auto restoreConc = [&]() {
    cVec[0] = cOriginal;
    m_gdmModel->setConcentrations(weak, cVec);
    recalculate(true);
  };

  const bool acidic = [&, this]() {
    cVec[0] = cLeft;
    this->m_gdmModel->setConcentrations(weak, cVec);

    recalculate(true);
    const double pHLeft = this->m_pH;

    cVec[0] = cRight;
    this->m_gdmModel->setConcentrations(weak, cVec);

    recalculate(true);
    const double pHRight = this->m_pH;

    return pHRight < pHLeft;
  }();

  size_t iters{0};
  cVec[0] = cNow;
  auto adjustCNow = [&,this]() -> std::function<void()> {
    if (acidic) {
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
    return targetpH + CCORR_PREC > this->m_pH && targetpH - CCORR_PREC < this->m_pH;
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
    restoreConc();
    throw Exception{trstr("Failed to correct concentration")};
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
    m_bufferCapacity = props.bufferCapacity;

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
  m_bufferCapacity = other.m_bufferCapacity;
  m_needsRecalculation = other.m_needsRecalculation;
  m_exclude = other.m_exclude;

  other.m_gdmModel = nullptr;
  other.m_composition = nullptr;

  return *this;
}

} // namespace gearbox
