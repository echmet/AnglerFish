#ifndef CONVERSION_CONVERSION_H
#define CONVERSION_CONVERSION_H

#include "../core/gdm.h"

#include <echmetsyscomp.h>
#include <memory>

namespace conversion {

using InConstituentVecWrap = std::unique_ptr<ECHMET::SysComp::InConstituentVec, decltype(&ECHMET::SysComp::releaseInputData)>;

InConstituentVecWrap makeECHMETInConstituentVec(const gdm::GDM& doc);

std::unique_ptr<ECHMET::RealVec, void(*)(const ECHMET::RealVec*)>
    makeECHMETAnalyticalConcentrationsVec(const gdm::GDM& doc, std::size_t block, const ECHMET::SysComp::ConstituentVec* constituents);

} // namespace conversion

#endif // CONVERSION_CONVERSION_H
