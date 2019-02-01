#include "utility.h"

#include <gdm/core/gdm.h>
#include <gearbox/doubletostringconvertor.h>
#include <cassert>

namespace summary {

inline
std::string abbreviateName(const std::string &name)
{
  return name; /* TODO: Implement */
}

std::string Utility::bufferToString(const gdm::GDM *buf, const bool abbreviate)
{
  const auto &loc = gearbox::DoubleToStringConvertor::locale();

  std::string str{};

  auto last = buf->cend();
  last--;
  for (auto it = buf->cbegin(); it != buf->cend(); ++it) {
    const auto &name = it->name();
    const auto &cs = buf->concentrations(it);

    assert(cs.size() == 1);

    str += abbreviate ? abbreviateName(name) : name + "(" + loc.toString(cs[0]).toStdString() + ")";
    if (it != last)
      str += "_";
  }

  return str;
}

} // namespace summary
