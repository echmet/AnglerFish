#ifndef GDMPROXYIMPL_H
#define GDMPROXYIMPL_H

#include "gdmproxy.h"

namespace gdm {
  class GDM;
}

namespace gearbox {

class GDMProxyImpl : public GDMProxy {
public:
  GDMProxyImpl(gdm::GDM &model, const double minimumConcentration);
  virtual ~GDMProxyImpl() override;
  virtual const_iterator cbegin() const noexcept override;
  virtual std::vector<double> concentrations(const std::string &name) const noexcept override;
  virtual bool complexes(const std::string &name) const noexcept override;
  virtual bool contains(const std::string &name) const noexcept override;
  virtual const_iterator cend() const noexcept override;
  virtual void erase(const std::string &name) noexcept override;
  virtual gdm::Constituent get(const std::string &name) override;
  virtual bool insert(const gdm::Constituent &ctuent) noexcept override;
  virtual bool isNucleus(const std::string &name) const noexcept override;
  virtual void setConcentrations(const std::string &name, const std::vector<double> &concentrations) noexcept override;
  virtual bool update(const std::string &name, const gdm::Constituent &ctuent) override;

private:
  gdm::GDM &h_model;
};

} // namespace gearbox

#endif // GDMPROXYIMPL_H
