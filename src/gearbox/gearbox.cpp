#include "gearbox.h"

#include <cassert>

std::unique_ptr<Gearbox> Gearbox::s_me{nullptr};

void Gearbox::initialize()
{
  s_me = std::unique_ptr<Gearbox>(new Gearbox{});
}

Gearbox * Gearbox::instance()
{
  assert(s_me != nullptr);

  return s_me.get();
}

Gearbox::Gearbox() :
  m_analInputParams{0, 0, {}, {}},
  m_mobilitiesResultsModel{"Mobility"},
  m_pKaResultsModel{"pKa"}
{
}

const AnalyteInputParameters & Gearbox::analyteInputParameters() const noexcept
{
  return m_analInputParams;
}

void Gearbox::clearAnalyteInputParameters()
{
  m_analInputParams = AnalyteInputParameters(0, 0, {{0, false}}, {});
}

ChemicalBuffersModel & Gearbox::chemicalBuffersModel() noexcept
{
  return m_chemBufsModel;
}

DatabaseProxy & Gearbox::databaseProxy() noexcept
{
  return m_databaseProxy;
}

FitResultsModel & Gearbox::mobilitiesResultsModel()
{
  return m_mobilitiesResultsModel;
}

FitResultsModel & Gearbox::pKaResultsModel()
{
  return m_pKaResultsModel;
}

void Gearbox::setAnalyteInputParameters(const int chargeLow, const int chargeHigh,
                                        AnalyteInputParameters::ParameterVec mobilities, AnalyteInputParameters::ParameterVec pKas)
{
  m_analInputParams = AnalyteInputParameters{chargeLow, chargeHigh,
                                             std::move(mobilities), std::move(pKas)};
}

void Gearbox::setAnalyteInputParameters(AnalyteInputParameters params)
{
  m_analInputParams = std::move(params);
}
