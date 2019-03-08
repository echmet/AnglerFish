#include "utility.h"

namespace gearbox {
namespace utility {

bool isAcid(const gdm::GDM::const_iterator it)
{
  const auto &props = it->physicalProperties();
  return props.charges().low() < 0 && props.charges().high() == 0;
}

bool isBase(const gdm::GDM::const_iterator it)
{
  const auto &props = it->physicalProperties();
  return props.charges().low() == 0 && props.charges().high() > 0;
}

bool isStrongAcid(const gdm::GDM::const_iterator it)
{
  const auto &props = it->physicalProperties();

  /* Strong acid must have no positive states
   * and at least one negative state */
  if (props.charges().low() < 0 && props.charges().high() == 0)
    return props.pKa(-1) <= 1.0;
  return false;
}

bool isStrongBase(const gdm::GDM::const_iterator it)
{
  const auto &props = it->physicalProperties();

  /* Strong base must have no negative states
   * and at least one positive state */
  if (props.charges().low() == 0 && props.charges().high() > 0)
    return props.pKa(1) >= 13.0;
  return false;
}

gdm::GDM::const_iterator findDrivingConstituent(const gdm::GDM *buf)
{
  auto driving = buf->cend();
  for (auto it = buf->cbegin(); it != buf->cend(); ++it) {
    if (isStrongAcid(it)) {
      if (driving != buf->cend())
        return buf->cend(); /* We cannot have more than one driving constituent */
      driving = it;
    }

    if (isStrongBase(it)) {
      if (driving != buf->cend())
        return buf->cend();
      driving = it;
    }
  }

  return driving;
}

} // namespace utility
} // namespace gearbox
