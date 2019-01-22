#ifndef GEARBOX_H
#define GEARBOX_H

#include "analyteinputparameters.h"
#include "chemicalbuffersmodel.h"
#include "databaseproxy.h"
#include "fitresultsmodel.h"

#include <memory>

class Gearbox {
public:
  static void initialize();
  static Gearbox * instance();

  const AnalyteInputParameters & analyteInputParameters() const noexcept;
  ChemicalBuffersModel & chemicalBuffersModel() noexcept;
  DatabaseProxy & databaseProxy() noexcept;
  FitResultsModel & mobilitiesResultsModel();
  FitResultsModel & pKaResultsModel();
  void setAnalyteInputParameters(const int chargeLow, const int chargeHigh,
                                 AnalyteInputParameters::ParameterVec mobilities, AnalyteInputParameters::ParameterVec pKas);
  void setAnalyteInputParameters(AnalyteInputParameters params);

private:
  Gearbox();

  AnalyteInputParameters m_analInputParams;
  ChemicalBuffersModel m_chemBufsModel;
  DatabaseProxy m_databaseProxy;

  FitResultsModel m_mobilitiesResultsModel;
  FitResultsModel m_pKaResultsModel;

  static std::unique_ptr<Gearbox> s_me;
};

#endif // GEARBOX_H
