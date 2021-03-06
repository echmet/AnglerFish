#include "caesinterface.h"

#include "eclutil.h"

#include <gearbox/ioniceffectsmodel.h>
#include <gdm/core/gdm.h>
#include <gdm/conversion/conversion.h>
#include <echmetcaes.h>
#include <echmetcaes_extended.h>
#include <echmetionprops.h>
#include <QObject>
#include <memory>

inline
void solverReleaser(ECHMET::CAES::Solver *ptr)
{
  ptr->destroy();
}

inline
void solverCtxReleaser(ECHMET::CAES::SolverContext *ptr)
{
  ptr->destroy();
}

using SolverContextWrap = std::unique_ptr<ECHMET::CAES::SolverContext,
                                          decltype(&solverCtxReleaser)>;
using SolverWrap = std::unique_ptr<ECHMET::CAES::Solver,
                                   decltype(&solverReleaser)>;

inline
SolverContextWrap makeSolverContext(const ChemicalSystemPtr &chemSystem)
{
  ECHMET::CAES::SolverContext *ctx{};

  auto tRet = ECHMET::CAES::createSolverContext(ctx, *chemSystem);
  if (tRet != ECHMET::RetCode::OK)
    throw CAESInterface::Exception{trstr("Failed to create solver context: ") +
                                   errstr(tRet)};

  return {ctx, solverCtxReleaser};
}

inline
SolverWrap makeSolver(const SolverContextWrap &ctx, const bool debHue, const bool onsFuo)
{
  ECHMET::NonidealityCorrections corrs = ECHMET::defaultNonidealityCorrections();

  if (debHue)
    ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_DEBYE_HUCKEL);
  if (onsFuo)
    ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_ONSAGER_FUOSS);

  auto solver = ECHMET::CAES::createSolver(ctx.get(), ECHMET::CAES::Solver::NONE, corrs);
  if (solver == nullptr)
    throw CAESInterface::Exception{trstr("Failed to create solver")};

  return {solver, solverReleaser};
}

CAESInterface::CAESInterface(const gdm::GDM &model, const gearbox::IonicEffectsModel &ionEffs) :
  h_model{model},
  h_ionEffs{ionEffs}
{
}

CAESInterface::BufferProperties CAESInterface::bufferProperties()
{
  ChemicalSystemPtr chemSystem{new ECHMET::SysComp::ChemicalSystem, chemicalSystemReleaser};
  CalcPropsPtr calcProps{new ECHMET::SysComp::CalculatedProperties, calcPropsReleaser};
  auto cVec = conversion::makeECHMETInConstituentVec(h_model);
  ECHMET::RetCode tRet{};

  tRet = ECHMET::SysComp::makeComposition(*chemSystem, *calcProps, cVec.get());
  if (tRet != ECHMET::RetCode::OK)
    throw Exception{trstr("Failed to make composition: ") + errstr(tRet)};

  RealVecWrap acVec{nullptr, realVecReleaser};
  try {
    acVec =   makeAnalyticalConcentrations(chemSystem, h_model);
  } catch (const std::bad_alloc &) {
    throw Exception{trstr("Insufficient memory")};
  }

  auto solverCtx = makeSolverContext(chemSystem);
  auto solver = makeSolver(solverCtx, h_ionEffs.debyeHuckel(), h_ionEffs.onsagerFuoss());

  tRet = solver->estimateDistributionSafe(acVec.get(), *calcProps);
  if (tRet != ECHMET::RetCode::OK)
    throw Exception{trstr("Failed to calculate distribution: ") + errstr(tRet)};

  double bufCap = [&]() {
    ECHMET::ECHMETReal bc{};

    ECHMET::NonidealityCorrections corrs = ECHMET::defaultNonidealityCorrections();

    if (h_ionEffs.debyeHuckel())
      ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_DEBYE_HUCKEL);
    if (h_ionEffs.onsagerFuoss())
      ECHMET::nonidealityCorrectionSet(corrs, ECHMET::NonidealityCorrectionsItems::CORR_ONSAGER_FUOSS);
    auto tRet = ECHMET::CAES::calculateBufferCapacity(bc, corrs, *chemSystem, *calcProps, acVec.get());

    if (tRet != ECHMET::RetCode::OK)
      throw Exception{"Failed to calculate buffer capacity"};

    return bc;
  }();

  const double is = h_ionEffs.debyeHuckel() ? calcProps->ionicStrength : 0.0;
  const double pH = ECHMET::IonProps::calculatepH_direct(calcProps->ionicConcentrations->at(0), is);
  return {pH, calcProps->ionicStrength, bufCap};
}
