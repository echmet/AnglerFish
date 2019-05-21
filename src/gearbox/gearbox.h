#ifndef GEARBOX_H
#define GEARBOX_H

#include "analyteestimates.h"
#include "scalarfitresultsmapping.h"

#include <QObject>

namespace gearbox {

class GearboxPrivate;

class AnalyteEstimates;
class ChemicalBuffersModel;
class DatabaseProxy;
class FitResultsModel;
class IonicEffectsModel;
class LimitMobilityConstraintsModel;
class MobilityCurveModel;

class Gearbox : public QObject {
  Q_OBJECT
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
  LimitMobilityConstraintsModel & limitMobilityConstraintsModel() const;
  MobilityCurveModel & mobilityCurveModel() const;
  ScalarFitResultsMapping::MapperModel & scalarResultsModel() const;
  void setAnalyteEstimates(const int chargeLow, const int chargeHigh,
                           AnalyteEstimates::ParameterVec mobilities, AnalyteEstimates::ParameterVec pKas);
  void setAnalyteEstimates(AnalyteEstimates estimates);

  void clearAnalyteEstimates();
  void invalidateAll();
  void invalidateResults();

signals:
  void analyteEstimatesChanged();

private:
  GearboxPrivate *m_gboxPriv;
};

} // namespace gearbox

#endif // GEARBOX_H
