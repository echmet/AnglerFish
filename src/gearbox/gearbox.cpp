#include "gearbox.h"

#include "analyteestimates.h"
#include "gearboxprivate.h"

namespace gearbox {

Gearbox::Gearbox() :
  m_gboxPriv{new GearboxPrivate{}}
{
}

Gearbox::~Gearbox()
{
  delete m_gboxPriv;
}

const AnalyteEstimates & Gearbox::analyteEstimates() const
{
  return m_gboxPriv->m_analyteEstimates;
}

void Gearbox::clearAnalyteEstimates()
{
  m_gboxPriv->m_analyteEstimates = AnalyteEstimates{};
}

ChemicalBuffersModel & Gearbox::chemicalBuffersModel() const
{
  return m_gboxPriv->m_chemBufsModel;
}

DatabaseProxy & Gearbox::databaseProxy() const
{
  return m_gboxPriv->m_databaseProxy;
}

FitResultsModel & Gearbox::fittedMobilitiesModel() const
{
  return m_gboxPriv->m_fittedMobilities;
}

FitResultsModel & Gearbox::fittedpKasModel() const
{
  return m_gboxPriv->m_fittedpKas;
}

void Gearbox::invalidateAll()
{
  m_gboxPriv->m_fittedpKas.setNewData({});
  m_gboxPriv->m_fittedMobilities.setNewData({});
  m_gboxPriv->m_mobilityCurveModel.invalidateAll();
  m_gboxPriv->m_scalFRMapping.setData(m_gboxPriv->m_scalFRMapping.modelIndexFromItem(ScalarFitResultsMapping::Items::R_SQUARED), QVariant{0}, Qt::EditRole);
}

void Gearbox::invalidateResults()
{
  m_gboxPriv->m_fittedpKas.setNewData({});
  m_gboxPriv->m_fittedMobilities.setNewData({});
  m_gboxPriv->m_mobilityCurveModel.invalidateResults();
  m_gboxPriv->m_scalFRMapping.setData(m_gboxPriv->m_scalFRMapping.modelIndexFromItem(ScalarFitResultsMapping::Items::R_SQUARED), QVariant{0}, Qt::EditRole);
}

IonicEffectsModel & Gearbox::ionicEffectsModel()
{
  return m_gboxPriv->m_ionEffsModel;
}

const IonicEffectsModel & Gearbox::ionicEffectsModel() const
{
  return m_gboxPriv->m_ionEffsModel;
}

LimitMobilityConstraintsModel & Gearbox::limitMobilityConstraintsModel() const
{
  return m_gboxPriv->m_limMobConstrsModel;
}

MobilityCurveModel & Gearbox::mobilityCurveModel() const
{
  return m_gboxPriv->m_mobilityCurveModel;
}

ScalarFitResultsMapping::MapperModel & Gearbox::scalarResultsModel() const
{
  return m_gboxPriv->m_scalFRMapping;
}

void Gearbox::setAnalyteEstimates(const int chargeLow, const int chargeHigh,
                                  AnalyteEstimates::ParameterVec mobilities, AnalyteEstimates::ParameterVec pKas)
{
  m_gboxPriv->m_analyteEstimates = AnalyteEstimates{chargeLow,
                                                   chargeHigh,
                                                   std::move(mobilities),
                                                   std::move(pKas)};
}

void Gearbox::setAnalyteEstimates(AnalyteEstimates estimates)
{
  m_gboxPriv->m_analyteEstimates = std::move(estimates);
}

} // namespace gearbox
