#include "empfitterinterface.h"

#include "eclutil.h"

#include <gdm/conversion/conversion.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <gearbox/ioniceffectsmodel.h>
#include <gearbox/fitresultsmodel.h>
#include <gearbox/mobilitycurvemodel.h>
#include <echmetelmigparamsfitter.h>
#include <algorithm>
#include <limits>
#include <tuple>

inline
bool operator<(const QPointF &lhs, const QPointF &rhs)
{
  return lhs.x() < rhs.x();
}

inline
void inSystemReleaser(ECHMET::ElmigParamsFitter::InSystem *inSys)
{
  ECHMET::ElmigParamsFitter::releaseInSystem(*inSys);
  delete inSys;
}

inline
void fixerReleaser(ECHMET::ElmigParamsFitter::ParametersFixer *fixer)
{
  fixer->destroy();
}

inline
void resultsReleaser(ECHMET::ElmigParamsFitter::FitResults *results)
{
  ECHMET::ElmigParamsFitter::releaseResults(*results);
  delete results;
}

using FitResultsPtr = std::unique_ptr<ECHMET::ElmigParamsFitter::FitResults,
                                      decltype(&resultsReleaser)>;
using InBufferVecWrap = std::unique_ptr<ECHMET::ElmigParamsFitter::InBufferVec,
                                        decltype(&ECHMET::ElmigParamsFitter::releaseInBufferVec)>;
using InConstituentVecWrap = conversion::InConstituentVecWrap;
using InSystemWrap = std::unique_ptr<ECHMET::ElmigParamsFitter::InSystem,
                                     decltype(&inSystemReleaser)>;
using FixerWrap = std::unique_ptr<ECHMET::ElmigParamsFitter::ParametersFixer,
                                  decltype(&fixerReleaser)>;

namespace calculators {

inline
std::tuple<QVector<QPointF>, QVector<QPointF>>
expectedAndResidual(const InSystemWrap &system, const FitResultsPtr &results)
{
  ECHMET::ElmigParamsFitter::ExpectedCurvePointVec *expected{};

  auto fitRet = ECHMET::ElmigParamsFitter::expectedCurve(*system, *results, expected);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{"Cannot get expected curve: "} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw EMPFitterInterface:: Exception{err.toStdString()};
  }

  QVector<QPointF> mobsVec{};
  QVector<QPointF> resVec{};
  for (size_t idx = 0; idx < expected->size(); idx++) {
    auto pt = expected->at(idx);
    const double pH = pt.pH;

    mobsVec.push_back({pH, pt.expected});
    resVec.push_back({pH, pt.expected - pt.experimental});
  }

  expected->destroy();

  return {std::move(mobsVec), std::move(resVec)};
}

inline
void fixParameters(FixerWrap &fixer, gearbox::Gearbox &gbox)
{
  auto &params = gbox.analyteEstimates();

  auto work = [&fixer, &params](const gearbox::AnalyteEstimates::ParameterVec &vec,
                                const ECHMET::ElmigParamsFitter::FixedParameterType fpt)
  {
    int charge = params.chargeLow;
    for (const auto &val : vec) {
      if (charge == 0) {
        charge++;
        if (fpt == ECHMET::ElmigParamsFitter::FixedParameterType::FPT_MOBILITY)
          continue;
      }
      if (val.fixed)
        fixer->add(fpt, charge, val.value);

      charge++;
    }
  };

  work(params.pKas, ECHMET::ElmigParamsFitter::FixedParameterType::FPT_PKA);
  work(params.mobilities, ECHMET::ElmigParamsFitter::FixedParameterType::FPT_MOBILITY);
}

inline
void makeAnalyte(InSystemWrap &inSystem, gearbox::Gearbox &gbox)
{
  auto &params = gbox.analyteEstimates();

  InConstituentPtr analyte{new ECHMET::SysComp::InConstituent, inConstituentReleaser};
  analyte->name = nullptr;
  analyte->pKas = nullptr;
  analyte->mobilities = nullptr;
  analyte->complexForms = nullptr;
  analyte->viscosityCoefficient = 0.0;

  analyte->name = ECHMET::createFixedString("__ANALYTE__");
  if (analyte->name == nullptr)
    throw std::bad_alloc{};

  analyte->ctype = ECHMET::SysComp::ConstituentType::LIGAND;
  analyte->chargeLow = params.chargeLow;
  analyte->chargeHigh = params.chargeHigh;

  analyte->mobilities = ECHMET::createRealVec(params.mobilities.size());
  if (analyte->mobilities == nullptr)
    throw std::bad_alloc{};
  for (const auto &p : params.mobilities)
    analyte->mobilities->push_back(p.value);

  analyte->pKas = ECHMET::createRealVec(params.pKas.size());
  if (analyte->pKas == nullptr)
    throw std::bad_alloc{};
  for (const auto &p : params.pKas)
    analyte->pKas->push_back(p.value);

  auto aptr = analyte.release();

  inSystem->analyte = *aptr;

  delete aptr;
}

inline
std::tuple<InConstituentVecWrap,
           RealVecWrap>
makeBuffer(const gdm::GDM *model)
{
  ChemicalSystemPtr chemSystem{new ECHMET::SysComp::ChemicalSystem{}, chemicalSystemReleaser};
  CalcPropsPtr calcProps{new ECHMET::SysComp::CalculatedProperties{}, calcPropsReleaser};

  auto inCVec = conversion::makeECHMETInConstituentVec(*model);

  auto tRet = ECHMET::SysComp::makeComposition(*chemSystem, *calcProps, inCVec.get());
  if (tRet != ECHMET::RetCode::OK)
    throw std::runtime_error{trstr("Failed to make SysComp composition: ") + errstr(tRet)};

  auto acVec = makeAnalyticalConcentrations(chemSystem, *model);

  return {std::move(inCVec), std::move(acVec)};
}

inline
InSystemWrap prepare(gearbox::Gearbox &gbox)
{
  InSystemWrap inSystem{new ECHMET::ElmigParamsFitter::InSystem{}, inSystemReleaser};
  inSystem->buffers = nullptr;

  makeAnalyte(inSystem, gbox);

  auto inBufVec = InBufferVecWrap{ECHMET::ElmigParamsFitter::createInBufferVec(), ECHMET::ElmigParamsFitter::releaseInBufferVec};
  if (inBufVec == nullptr) {
    throw EMPFitterInterface::Exception{trstr("Insufficient memory")};
  }

  ECHMET::RetCode tRet{};

  for (const auto &b : gbox.chemicalBuffersModel()) {
    try {
      for (const auto &uExp : b.experimentalMobilities()) {
        ECHMET::ElmigParamsFitter::InBuffer inBuf{nullptr, nullptr, 0.0};

        auto bufComp = makeBuffer(b.model());

        tRet = inBufVec->push_back(inBuf);
        if (tRet != ECHMET::RetCode::OK) {
          throw EMPFitterInterface::Exception{errstr(tRet)};
        }

        const size_t sz = inBufVec->size();
        auto &_buf = inBufVec->operator[](sz - 1);
        _buf.composition = std::get<0>(bufComp).release();
        _buf.concentrations = std::get<1>(bufComp).release();
        _buf.uEffExperimental = uExp;
      }
    } catch (const std::bad_alloc &) {
      throw EMPFitterInterface::Exception{trstr("Insufficient memory")};
    }
  }

  ECHMET::NonidealityCorrections corrs = ECHMET::defaultNonidealityCorrections();
  if (gbox.ionicEffectsModel().debyeHuckel())
    ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_DEBYE_HUCKEL);
  if (gbox.ionicEffectsModel().onsagerFuoss())
    ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_ONSAGER_FUOSS);

  inSystem->buffers = inBufVec.release();
  inSystem->corrections = corrs;

  return inSystem;
}

inline
void setResults(const InSystemWrap &system, const FitResultsPtr &results, gearbox::Gearbox &gbox)
{
  static const auto relStDev = [](auto v, auto stDev) {
    return (std::abs(stDev / v) * 100.0);
  };

  static const auto set = [](const auto &r, QVector<gearbox::FitResultsModel::Result> &data) {
    data.push_back(gearbox::FitResultsModel::Result{r.charge, r.value, r.stDev, relStDev(r.value, r.stDev)});
  };

  static const auto walk = [](const auto v, gearbox::FitResultsModel &model) {
    QVector<gearbox::FitResultsModel::Result> vec{};
    for (size_t idx = 0; idx < v->size(); idx++)
      set(v->at(idx), vec);

    model.setNewData(std::move(vec));
  };

  walk(results->mobilities, gbox.fittedMobilitiesModel());
  walk(results->pKas, gbox.fittedpKasModel());

  auto &model = gbox.mobilityCurveModel();
  const auto t = expectedAndResidual(system, results);

  auto fitted = std::move(std::get<0>(t));
  auto residuals = std::move(std::get<1>(t));

  std::sort(fitted.begin(), fitted.end());
  std::sort(residuals.begin(), residuals.end());

  model.setFitted(std::move(fitted));
  model.setResiduals(std::move(residuals));
  gbox.scalarResultsModel().setItem(gearbox::ScalarFitResultsMapping::Items::R_SQUARED,
                                    results->rSquared, Qt::EditRole);
}

EMPFitterInterface::EMPFitterInterface(gearbox::Gearbox &gbox) :
  h_gbox{gbox}
{
}

void EMPFitterInterface::fit()
{
  /* Panzer vor! */
  auto system = prepare(h_gbox);

  auto fixer = FixerWrap{ECHMET::ElmigParamsFitter::createParametersFixer(), fixerReleaser};
  fixParameters(fixer, h_gbox);

  auto results = FitResultsPtr{new ECHMET::ElmigParamsFitter::FitResults{nullptr, nullptr, 0.0}, resultsReleaser};
  auto fitRet = ECHMET::ElmigParamsFitter::process(*system, fixer.get(), *results);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{"Fit failed: "} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw Exception{err.toStdString()};
  }

  setResults(system, results, h_gbox);
}

} // namespace calculators
