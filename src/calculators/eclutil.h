#ifndef ECLUTIL_H
#define ECLUTIL_H

#include <gdm/core/gdm.h>
#include <gdm/conversion/conversion.h>
#include <echmetsyscomp.h>
#include <QObject>
#include <memory>

inline
std::string trstr(const char *str)
{
  return std::string{QObject::tr(str).toUtf8().data()};
}

inline
std::string errstr(const ECHMET::RetCode tRet)
{
  return ECHMET::errorToString(tRet);
}

inline
void chemicalSystemReleaser(ECHMET::SysComp::ChemicalSystem *ptr)
{
  ECHMET::SysComp::releaseChemicalSystem(*ptr);
}

inline
void calcPropsReleaser(ECHMET::SysComp::CalculatedProperties *ptr)
{
  ECHMET::SysComp::releaseCalculatedProperties(*ptr);
}

inline
void inConstituentReleaser(ECHMET::SysComp::InConstituent *inC)
{
  ECHMET::SysComp::releaseInConstituent(*inC);
}

inline
void realVecReleaser(ECHMET::RealVec *ptr)
{
  ptr->destroy();
}

using ChemicalSystemPtr = std::unique_ptr<ECHMET::SysComp::ChemicalSystem,
                                          decltype(&chemicalSystemReleaser)>;
using CalcPropsPtr = std::unique_ptr<ECHMET::SysComp::CalculatedProperties,
                                     decltype(&calcPropsReleaser)>;
using InConstituentPtr = std::unique_ptr<ECHMET::SysComp::InConstituent,
                                         decltype(&inConstituentReleaser)>;
using RealVecWrap = std::unique_ptr<ECHMET::RealVec,
                                    decltype(&realVecReleaser)>;

inline
RealVecWrap makeAnalyticalConcentrations(const ChemicalSystemPtr &chemSystem, const gdm::GDM &model)
{
  auto acVec = conversion::makeECHMETAnalyticalConcentrationsVec(model, 0, chemSystem->constituents);

  return {acVec.release(), realVecReleaser};
}

#endif // ECLUTIL_H
