#ifndef GEARBOX_UTILITY_H
#define GEARBOX_UTILITY_H

#include <gdm/core/gdm.h>

namespace gearbox {
namespace utility {

bool isAcid(const gdm::GDM::const_iterator it);
bool isBase(const gdm::GDM::const_iterator it);
bool isStrongAcid(const gdm::GDM::const_iterator it);
bool isStrongBase(const gdm::GDM::const_iterator it);
gdm::GDM::const_iterator findDrivingConstituent(const gdm::GDM *buf);

} // namespace utility
} // namespace gearbox

#endif // GEARBOX_UTILITY_H
