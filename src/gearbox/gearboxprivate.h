#ifndef GEARBOXPRIVATE_H
#define GEARBOXPRIVATE_H

#include "analyteestimates.h"
#include "chemicalbuffersmodel.h"
#include "databaseproxy.h"
#include "fitresultsmodel.h"
#include "ioniceffectsmodel.h"
#include "limitmobilityconstraintsmodel.h"
#include "mobilitycurvemodel.h"
#include "scalarfitresultsmapping.h"

#include <memory>

namespace gearbox {

class GearboxPrivate {
public:
  explicit GearboxPrivate();
  GearboxPrivate(const GearboxPrivate &other) = delete;

  AnalyteEstimates m_analyteEstimates;
  ChemicalBuffersModel m_chemBufsModel;
  IonicEffectsModel m_ionEffsModel;

  DatabaseProxy m_databaseProxy;

  FitResultsModel m_fittedMobilities;
  FitResultsModel m_fittedpKas;

  MobilityCurveModel m_mobilityCurveModel;

  LimitMobilityConstraintsModel m_limMobConstrsModel;

  ScalarFitResultsMapping::MapperModel m_scalFRMapping;
  QVector<double> m_scalFRData;
};

} // gearbox

#endif // GEARBOXPRIVATE_H
