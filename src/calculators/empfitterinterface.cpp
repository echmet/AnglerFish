#include "empfitterinterface.h"

#include "eclutil.h"

#include <gdm/conversion/conversion.h>
#include <gearbox/gearbox.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <gearbox/ioniceffectsmodel.h>
#include <gearbox/fitresultsmodel.h>
#include <gearbox/limitmobilityconstraintsmodel.h>
#include <gearbox/mobilitycurvemodel.h>
#include <echmetelmigparamsfitter.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <tuple>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // _WIN32

#ifdef _WIN32
inline
void wcharDeleter(wchar_t *ptr)
{
  delete[] ptr;
};
#endif // _WIN32

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

inline
void tpiVecReleaser(ECHMET::ElmigParamsFitter::TracepointInfoVec *vec)
{
  if (vec != nullptr) {
    for (size_t idx = 0; idx < vec->size(); idx++)
      vec->elem(idx).description->destroy();

    vec->destroy();
  }
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
using TPIVWrap = std::unique_ptr<ECHMET::ElmigParamsFitter::TracepointInfoVec,
                                 decltype(&tpiVecReleaser)>;

namespace calculators {

const char * EMPFitterInterface::ANALYTE_NAME{"__ANALYTE__"};

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

  analyte->name = ECHMET::createFixedString(EMPFitterInterface::ANALYTE_NAME);
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
    if (b.empty() || b.exclude())
      continue;

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
void setResults(const InSystemWrap &system, const FitResultsPtr &results, gearbox::Gearbox &gbox,
                double &rSquared)
{
  static const auto relStdErr = [](const double v, const double stdErr) {
    return (std::abs(stdErr / v) * 100.0);
  };

  static const auto set = [](const auto &r, QVector<gearbox::FitResultsModel::Result> &data) {
    data.push_back(gearbox::FitResultsModel::Result{r.charge, r.value, r.stdErr, relStdErr(r.value, r.stdErr)});
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
  auto t = expectedAndResidual(system, results);

  auto fitted = std::move(std::get<0>(t));
  auto residuals = std::move(std::get<1>(t));

  std::sort(fitted.begin(), fitted.end());
  std::sort(residuals.begin(), residuals.end());

  model.setFitted(std::move(fitted), std::move(residuals));
  rSquared = results->rSquared;
}

EMPFitterInterface::EMPFitterInterface(gearbox::Gearbox &gbox, const bool unscaledStdErrs) :
  h_gbox{gbox},
  m_rSquared{-1.0},
  m_unscaledStdErrs{unscaledStdErrs}
{
}

void EMPFitterInterface::calculateProvisional()
{
  ECHMET::ElmigParamsFitter::ExpectedCurvePointVec *expected{};

  auto system = prepare(h_gbox);
  auto results = FitResultsPtr{new ECHMET::ElmigParamsFitter::FitResults{nullptr, nullptr, 0.0}, resultsReleaser};

  auto fitRet = ECHMET::ElmigParamsFitter::provisionalCurve(*system, expected);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{"Cannot get provisional curve: "} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw EMPFitterInterface:: Exception{err.toStdString()};
  }

  QVector<QPointF> expectedQV{};
  for (size_t idx = 0; idx < expected->size(); idx++) {
    const auto &pt = expected->at(idx);

    expectedQV.push_back({pt.pH, pt.expected});
  }

  expected->destroy();

  h_gbox.mobilityCurveModel().setProvisional(std::move(expectedQV));
}

void EMPFitterInterface::fit()
{
  m_rSquared = 0.0;

  /* Panzer vor! */
  auto system = prepare(h_gbox);

  auto fixer = FixerWrap{ECHMET::ElmigParamsFitter::createParametersFixer(), fixerReleaser};
  fixParameters(fixer, h_gbox);

  auto options = ECHMET::ElmigParamsFitter::defaultFitOptions();
  if (!h_gbox.limitMobilityConstraintsModel().enabled())
    ECHMET::EnumOps::operator|=(options, ECHMET::ElmigParamsFitter::FO_DISABLE_MOB_CONSTRAINTS);
  if (m_unscaledStdErrs)
    ECHMET::EnumOps::operator|=(options, ECHMET::ElmigParamsFitter::FO_UNSCALED_STDERRS);

  auto results = FitResultsPtr{new ECHMET::ElmigParamsFitter::FitResults{nullptr, nullptr, 0.0}, resultsReleaser};
  auto fitRet = ECHMET::ElmigParamsFitter::process(*system, fixer.get(), options, *results);
  if (fitRet != ECHMET::ElmigParamsFitter::RetCode::OK) {
    const auto err = QString{QObject::tr("Fit failed: ")} + QString{ECHMET::ElmigParamsFitter::EMPFerrorToString(fitRet)};
    throw Exception{err.toStdString()};
  }

  setResults(system, results, h_gbox, m_rSquared);
}

EMPFitterInterface::MobilityConstraints EMPFitterInterface::mobilityConstraints(const double mobility)
{
  const auto low = ECHMET::ElmigParamsFitter::mobilityLowerBound() * mobility;
  const auto high = ECHMET::ElmigParamsFitter::mobilityUpperBound() * mobility;

  return { low, high };
}

void EMPFitterInterface::propagateRSquared()
{
  /* Yes, you are reading this right. Although it would make perfect sense to update
     this value from setReslults(), we cannot do that because setResults() is called
     by fit() which is expected to be executed from a worker thread. Why is that
     a problem? Well, apparently on Win32 the dataChanged() signal does not propagate
     correctly, leading to the View ignore the change.
     Hence we provide this method that can be executed separately from the main thread.
     It is nasty but it seems to fix the problem. Great, another three hours of my life
     flushed down the drain...
  */
  h_gbox.scalarResultsModel().setItem(gearbox::ScalarFitResultsMapping::Items::R_SQUARED,
                                      m_rSquared, Qt::EditRole);
}

void EMPFitterInterface::setTracepoints(const std::vector<TracepointState> &states)
{
  if (states.empty())
    ECHMET::ElmigParamsFitter::toggleAllTracepoints(false);
  else {
    for (const auto &item : states)
      ECHMET::ElmigParamsFitter::toggleTracepoint(item.TPID, item.enabled);
  }
}

std::vector<EMPFitterInterface::TracepointInfo> EMPFitterInterface::tracepointInformation()
{
  std::vector<TracepointInfo> tpi{};

  auto tpiVec = TPIVWrap{ECHMET::ElmigParamsFitter::tracepointInfo(), tpiVecReleaser};
  if (tpiVec == nullptr)
    return tpi;

  for (size_t idx = 0; idx < tpiVec->size(); idx++) {
    const auto &t = tpiVec->at(idx);
    tpi.emplace_back(t.id, t.description->c_str());
  }

  return tpi;
}

const char * EMPFitterInterface::versionString()
{
  return ECHMET::ElmigParamsFitter::versionString();
}

bool EMPFitterInterface::writeTrace(const char *path)
{
  if (path == nullptr)
    return true;

  if (strlen(path) == 0)
    return true;

  auto traceRaw = ECHMET::ElmigParamsFitter::trace();
  if (traceRaw == nullptr)
    return false;

  std::string trace = traceRaw->c_str();
  traceRaw->destroy();

#ifdef _WIN32
  using WCharWrap = std::unique_ptr<wchar_t, decltype(&wcharDeleter)>;

  const auto wPath = [](const char *path) -> WCharWrap {
    wchar_t *wPath{nullptr};

    const auto len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, nullptr, 0);
    if (len < 1)
      return {nullptr, wcharDeleter};

    wPath = new (std::nothrow) wchar_t[len];
    if (wPath == nullptr)
      return {nullptr, wcharDeleter};

    const auto ret = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wPath, len);
    if (ret != len) {
      delete[] wPath;
      return {nullptr, wcharDeleter};
    }

    return {wPath, wcharDeleter};
  }(path);

  if (wPath == nullptr)
    return false;

  auto fh = CreateFileW(wPath.get(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, NULL);
  if (fh == INVALID_HANDLE_VALUE)
    return false;

  DWORD written{0};
  auto ret = WriteFile(fh, trace.data(), trace.length(), &written, NULL);

  CloseHandle(fh);

  return ret;
#else
  std::ofstream ofs{path};
  if (!ofs.is_open())
    return false;

  ofs << trace;

  return ofs.good();
#endif // _WIN32
}

} // namespace calculators
