#ifndef GEARBOX_H
#define GEARBOX_H

#include "analyteestimates.h"
#include "scalarfitresultsmapping.h"

namespace gearbox {

class GearboxPrivate;

class AnalyteEstimates;
class ChemicalBuffersModel;
class DatabaseProxy;
class FitResultsModel;
class IonicEffectsModel;
class MobilityCurveModel;

class Gearbox {
public:
  explicit Gearbox();
  Gearbox(const Gearbox &other) = delete;
  ~Gearbox();

  const AnalyteEstimates & analyteEstimates() const;
  ChemicalBuffersModel & chemicalBuffersModel() const;
  DatabaseProxy & databaseProxy() const;
  FitResultsModel & fittedMobilitiesModel() const;
  FitResultsModel & fittedpKasModel() const;
  IonicEffectsModel & ionicEffectsModel();
  const IonicEffectsModel & ionicEffectsModel() const;
  MobilityCurveModel & mobilityCurveModel() const;
  ScalarFitResultsMapping::MapperModel & scalarResultsModel() const;
  void setAnalyteEstimates(const int chargeLow, const int chargeHigh,
                           AnalyteEstimates::ParameterVec mobilities, AnalyteEstimates::ParameterVec pKas);
  void setAnalyteEstimates(AnalyteEstimates estimates);

  void clearAnalyteEstimates();
  void invalidateAll();
  void invalidateResults();

private:
  GearboxPrivate *m_gboxPriv;
};

} // namespace gearbox

#endif // GEARBOX_H
