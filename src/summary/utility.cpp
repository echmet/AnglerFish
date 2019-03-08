#include "utility.h"

#include <gdm/core/gdm.h>
#include <gearbox/doubletostringconvertor.h>
#include <gearbox/utility.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#if __cplusplus > 201402L
  #define HAVE_CPP17
  #include <execution>
#endif // C++17
#include <map>
#include <sstream>

namespace summary {

static
const std::map<std::string, std::string> ABBREV_SHORTHANDS{{
  { "lithium", "Li" },
  { "sodium", "Na" },
  { "potassium", "K" },
  { "magnesium", "Mg" },
  { "calcium", "Ca" },
  { "hydrochloric acid", "HCl" }
}};

inline
bool isLtr(const char ch)
{
  return ch >= 'a' && ch <= 'z';
}

inline
std::vector<std::string> split(const std::string &str)
{
  std::istringstream iss{str};

  std::vector<std::string> toks{};
  std::string s{};
  while (std::getline(iss, s, ' '))
    toks.emplace_back(std::move(s));

  return toks;
}

inline
std::string abbreviateName(std::string name)
{
#ifdef HAVE_CPP17
  std::transform(std::execution::parallel_unsequenced_policy, name.begin(), name.end(), name.begin(), [](char ch) { return  std::tolower(ch); });
#else
  std::transform(name.begin(), name.end(), name.begin(), [](char ch) { return  std::tolower(ch); });
#endif // HAVE_CPP17

  auto sit = ABBREV_SHORTHANDS.find(name);
  if (sit != ABBREV_SHORTHANDS.cend())
    return sit->second;

  auto toks = split(name);

  std::string abbrName{};

  for (const auto &t : toks) {
    std::string s{};

    auto it = t.cbegin();
    for (; it != s.cend(); ++it) {
      if (!isLtr(*it))
        s.append(1, *it);
      else
        break;
    }

    if (it != t.cend())
      s.append(1, std::toupper(*it));
    it++;

    size_t ctr{0};
    while (it != t.cend() && ctr < 3) {
      s.append(1, *it);
      if (isLtr(*it))
        ctr++;
      it++;
    }

    abbrName += s;
  }

  return abbrName;
}

std::string Utility::bufferToString(const gdm::GDM *buf, const bool abbreviate)
{
  const auto &loc = gearbox::DoubleToStringConvertor::locale();

  auto toString = [&](const gdm::GDM::const_iterator it) {
    assert(it != buf->cend());

    const auto &name = it->name();
    const auto &cs = buf->concentrations(it);

    assert(cs.size() == 1);

    return (abbreviate ? abbreviateName(name) : name) + "(" + loc.toString(cs[0]).toStdString() + ")";
  };

  std::string str{};

  const auto driving = gearbox::utility::findDrivingConstituent(buf);

  auto last = buf->cend();
  if (driving == buf->cend())
    last--;
  for (auto it = buf->cbegin(); it != buf->cend(); ++it) {
    if (it == driving)
      continue;

    str += toString(it);

    if (it != last && !abbreviate)
      str += "_";
  }

  if (driving != buf->cend())
    str += toString(driving);

  return str;
}

} // namespace summary
