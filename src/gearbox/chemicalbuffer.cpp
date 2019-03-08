#include "chemicalbuffer.h"
#include "gdmproxyimpl.h"

#include <gdm/conversion/conversion.h>
#include <gdm/core/gdm.h>
#include <calculators/caesinterface.h>
#include <trstr.h>

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

void ChemicalBuffer::correctConcentration()
{
  if (m_gdmModel->size() > 2)
    throw Exception{trstr("Automatic correction works with binary buffers only")};
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

void ChemicalBuffer::recalculate()
{
  if (!m_needsRecalculation)
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
