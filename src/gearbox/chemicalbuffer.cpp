#include "chemicalbuffer.h"
#include "gdmproxyimpl.h"

#include <gdm/conversion/conversion.h>
#include <gdm/core/gdm.h>
#include <calculators/caesinterface.h>

static const double MIN_CONCENTRATION{1.0e-11};

ChemicalBuffer::ChemicalBuffer() :
  m_gdmModel{new gdm::GDM{}},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_needsRecalculation{true}
{
}

ChemicalBuffer::ChemicalBuffer(gdm::GDM *model) :
  m_gdmModel{model},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_needsRecalculation{true}
{
  recalculate();
}

ChemicalBuffer::ChemicalBuffer(const ChemicalBuffer &other) :
  m_gdmModel{new gdm::GDM{}},
  m_composition{new GDMProxyImpl{*m_gdmModel, MIN_CONCENTRATION}},
  m_experimentalMobilities{other.experimentalMobilities()},
  m_pH{other.m_pH},
  m_ionicStrength{other.m_ionicStrength},
  m_needsRecalculation{other.m_needsRecalculation}
{
  *m_gdmModel = *other.m_gdmModel;
}

ChemicalBuffer::ChemicalBuffer(ChemicalBuffer &&other) noexcept :
  m_gdmModel{other.m_gdmModel},
  m_composition{other.m_composition},
  m_experimentalMobilities{std::move(other.experimentalMobilities())},
  m_pH{other.m_pH},
  m_ionicStrength{other.m_ionicStrength},
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

bool ChemicalBuffer::empty() const
{
  return m_gdmModel->size() == 0;
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

  CAESInterface iface{*m_gdmModel};

  try {
    auto props = iface.bufferProperties();

    m_pH = props.pH;
    m_ionicStrength = props.ionicStrength * 1000.0;

    m_needsRecalculation = false;
  } catch (const CAESInterface::Exception &ex) {
    throw Exception{ex.what()};
  }
}

void ChemicalBuffer::setExperimentalMobilities(std::vector<double> mobilities)
{
  m_experimentalMobilities = std::move(mobilities);
}
