#include "empfitterinterface.h"

#include "eclutil.h"

#include <gdm/conversion/conversion.h>
#include <gearbox/gearbox.h>
#include <echmetelmigparamsfitter.h>
#include <tuple>

#include <QDebug>

inline
void inSystemReleaser(ECHMET::ElmigParamsFitter::InSystem *inSys)
{
  ECHMET::ElmigParamsFitter::releaseInSystem(*inSys);
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

inline
void fixParameters(FixerWrap &fixer)
{
  auto &params = Gearbox::instance()->analyteInputParameters();

  auto work = [&fixer, &params](const AnalyteInputParameters::ParameterVec &vec, const ECHMET::ElmigParamsFitter::FixedParameterType fpt)
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
    }
  };

  work(params.pKas, ECHMET::ElmigParamsFitter::FixedParameterType::FPT_PKA);
  work(params.mobilities, ECHMET::ElmigParamsFitter::FixedParameterType::FPT_MOBILITY);
}

inline
void makeAnalyte(InSystemWrap &inSystem)
{
  auto &params = Gearbox::instance()->analyteInputParameters();

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

  inSystem->analyte = *analyte.release();
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
void setResults(ECHMET::ElmigParamsFitter::FitResults &results)
{
  static const auto relStDev = [](auto v, auto stDev) {
    return (std::abs(stDev / v) * 100.0);
  };

  static const auto set = [](const auto &r, QVector<FitResultsModel::Result> &data) {
    data.push_back(FitResultsModel::Result{r.charge, r.value, r.stDev, relStDev(r.value, r.stDev)});
  };

  static const auto walk = [](const auto v, FitResultsModel &model) {
    QVector<FitResultsModel::Result> vec{};
    for (size_t idx = 0; idx < v->size(); idx++)
      set(v->at(idx), vec);

    model.setNewData(std::move(vec));
  };

  walk(results.mobilities, Gearbox::instance()->mobilitiesResultsModel());
  walk(results.pKas, Gearbox::instance()->pKaResultsModel());
}

class EMPFitterInterface::Context {
public:
  Context() :
    system{nullptr, inSystemReleaser},
    results{nullptr, resultsReleaser}
  {
  }

  InSystemWrap system;
  FitResultsPtr results;
};

EMPFitterInterface::EMPFitterInterface() :
  m_ctx{new Context{}},
  m_resultsAvailable{false}
{
}

EMPFitterInterface::~EMPFitterInterface()
{
}

QVector<QPointF > EMPFitterInterface::expectedCurve()
{
  if (!m_resultsAvailable)
    throw Exception{trstr("Fit results are not available")};

  assert(m_ctx->results != nullptr);

  ECHMET::ElmigParamsFitter::ExpectedCurvePointVec *expected{};

  auto fitRet = ECHMET::ElmigParamsFitter::expectedCurve(*m_ctx->system, *m_ctx->results, expected);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{"Cannot get expected curve: "} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw Exception{err.toStdString()};
  }

  QVector<QPointF> retVec{};
  for (size_t idx = 0; idx < expected->size(); idx++) {
    auto pt = expected->at(idx);
    retVec.push_back(QPointF{pt.pH, pt.expected});
  }

  expected->destroy();

  return retVec;
}

void EMPFitterInterface::fit(/* TODO: IS corrections */)
{
  prepare();

  assert(m_ctx != nullptr);

  auto fixer = FixerWrap{ECHMET::ElmigParamsFitter::createParametersFixer(), fixerReleaser};
  fixParameters(fixer);

  auto results = FitResultsPtr{new ECHMET::ElmigParamsFitter::FitResults{nullptr, nullptr}, resultsReleaser};
  auto fitRet = ECHMET::ElmigParamsFitter::process(*m_ctx->system, fixer.get(), *results);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{"Fit failed: "} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw Exception{err.toStdString()};
  }

  setResults(*results);

  m_ctx->results = std::move(results);
  m_resultsAvailable = true;
}

void EMPFitterInterface::prepare(/* TODO: IS corrections */)
{
  assert(m_ctx != nullptr);

  auto gbox = Gearbox::instance();

  InSystemWrap inSystem{new ECHMET::ElmigParamsFitter::InSystem{}, inSystemReleaser};
  inSystem->buffers = nullptr;

  makeAnalyte(inSystem);

  auto inBufVec = InBufferVecWrap{ECHMET::ElmigParamsFitter::createInBufferVec(), ECHMET::ElmigParamsFitter::releaseInBufferVec};
  if (inBufVec == nullptr) {
   throw Exception{trstr("Insufficient memory")};
  }

  ECHMET::RetCode tRet{};

  for (const auto &b : gbox->chemicalBuffersModel()) {
    try {
      for (const auto &uExp : b.experimentalMobilities()) {
        ECHMET::ElmigParamsFitter::InBuffer inBuf{nullptr, nullptr, 0.0};

        auto bufComp = makeBuffer(b.model());

        tRet = inBufVec->push_back(inBuf);
        if (tRet != ECHMET::RetCode::OK) {
          throw Exception{errstr(tRet)};
        }

        const size_t sz = inBufVec->size();
        auto &_buf = inBufVec->operator[](sz - 1);
        _buf.composition = std::get<0>(bufComp).release();
        _buf.concentrations = std::get<1>(bufComp).release();
        _buf.uEffExperimental = uExp;
      }
    } catch (const std::bad_alloc &) {
      throw Exception{trstr("Insufficient memory")};
    }
  }

  ECHMET::NonidealityCorrections corrs{};
  ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_DEBYE_HUCKEL);
  ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_ONSAGER_FUOSS);

  inSystem->buffers = inBufVec.release();
  inSystem->corrections = corrs;

  m_ctx->system= std::move(inSystem);
}
