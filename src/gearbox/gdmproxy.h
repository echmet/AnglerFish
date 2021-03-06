#ifndef GDMPROXY_H
#define GDMPROXY_H

#include <gdm/core/basiccomposition.h>
#include <string>
#include <stdexcept>
#include <vector>

namespace gdm {
  class Constituent;
}

class GDMProxyException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

namespace gearbox {

class GDMProxy {
public:
  using const_iterator = gdm::BasicComposition::const_iterator;

  GDMProxy(const double minimumConcentration);
  virtual ~GDMProxy() = 0;
  virtual const_iterator cbegin() const noexcept = 0;
  virtual std::vector<double> concentrations(const std::string &name) const noexcept = 0;
  virtual bool complexes(const std::string &name) const noexcept = 0;
  virtual bool contains(const std::string &name) const noexcept = 0;
  virtual const_iterator cend() const noexcept = 0;
  virtual void erase(const std::string &name) noexcept = 0;
  virtual gdm::Constituent get(const std::string &name) = 0;
  virtual bool insert(const gdm::Constituent &ctuent) noexcept = 0;
  virtual bool isNucleus(const std::string &name) const noexcept = 0;
  virtual void setConcentrations(const std::string &name, const std::vector<double> &concentrations) noexcept = 0;
  virtual bool update(const std::string &name, const gdm::Constituent &ctuent) = 0;

protected:
  const double m_minimumConcentration;
};

} // namespace gearbox

#endif // GDMPROXY_H
