#ifndef GEARBOX_H
#define GEARBOX_H

#include "analyteinputparameters.h"
#include "chemicalbuffersmodel.h"
#include "databaseproxy.h"
#include "fitresultsmodel.h"
#include "mobilitycurvemodel.h"

#include <memory>

class Gearbox {
public:
  static void initialize();
  static Gearbox * instance();

  const AnalyteInputParameters & analyteInputParameters() const noexcept;
  ChemicalBuffersModel & chemicalBuffersModel() noexcept;
  void clearAnalyteInputParameters();
  DatabaseProxy & databaseProxy() noexcept;
  MobilityCurveModel & mobilityCurveModel();
  const MobilityCurveModel & mobilityCurveModel() const;
  FitResultsModel & mobilitiesResultsModel();
  const FitResultsModel & mobilitiesResultsModel() const;
  FitResultsModel & pKaResultsModel();
  const FitResultsModel & pKaResultsModel() const;
  void setAnalyteInputParameters(const int chargeLow, const int chargeHigh,
                                 AnalyteInputParameters::ParameterVec mobilities, AnalyteInputParameters::ParameterVec pKas);
  void setAnalyteInputParameters(AnalyteInputParameters params);

private:
  Gearbox();

  AnalyteInputParameters m_analInputParams;
  ChemicalBuffersModel m_chemBufsModel;
  DatabaseProxy m_databaseProxy;
  MobilityCurveModel m_mobCurveModel;

  FitResultsModel m_mobilitiesResultsModel;
  FitResultsModel m_pKaResultsModel;

  static std::unique_ptr<Gearbox> s_me;
};

#endif // GEARBOX_H
