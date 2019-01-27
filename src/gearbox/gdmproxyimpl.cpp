#include "gdmproxyimpl.h"

#include <gdm/core/gdm.h>
#include "gdmproxy_p.h"

#include <cassert>

namespace gearbox {

GDMProxyImpl::GDMProxyImpl(gdm::GDM &model, const double minimumConcentration) :
  GDMProxy{minimumConcentration},
  h_model{model}
{
}

GDMProxyImpl::~GDMProxyImpl()
{
}

GDMProxyImpl::const_iterator GDMProxyImpl::cbegin() const noexcept
{
  return h_model.cbegin();
}

std::vector<double> GDMProxyImpl::concentrations(const std::string &name) const noexcept
{
  assert(h_model.find(name) != h_model.cend());

  auto sampleIt = h_model.find(name);

  auto sampleConc = h_model.concentrations(sampleIt);
  assert(sampleConc.size() == 1);

  return {sampleConc.at(0)};
}

bool GDMProxyImpl::complexes(const std::string &name) const noexcept
{
  const auto it = h_model.find(name);

  assert(it != h_model.cend());

  const auto found = gdm::findComplexations(h_model.composition(), it);
  return (found.size() > 0);
}

bool GDMProxyImpl::contains(const std::string &name) const noexcept
{
  const auto sampleIt = h_model.find(name);

  return sampleIt != h_model.cend();
}

GDMProxyImpl::const_iterator GDMProxyImpl::cend() const noexcept
{
  return h_model.cend();
}

void GDMProxyImpl::erase(const std::string &name) noexcept
{
  const auto sampleIt = h_model.find(name);

  GDMProxy_p::eraseComplexations(sampleIt, h_model);

  if (sampleIt != h_model.cend())
    h_model.erase(sampleIt);
}

gdm::Constituent GDMProxyImpl::get(const std::string &name)
{
  auto it = h_model.find(name);
  if (it == h_model.cend())
    throw GDMProxyException{"Constituent not found"};

  return *it;
}

bool GDMProxyImpl::insert(const gdm::Constituent &ctuent) noexcept
{
  try {
    h_model.insert(ctuent);
  } catch (std::bad_alloc &) {
    erase(ctuent.name());
    return false;
  }

  setConcentrations(ctuent.name(), { 0 } );

  return true;
}

bool GDMProxyImpl::isNucleus(const std::string &name) const noexcept
{
  const auto it = h_model.find(name);
  assert(it != h_model.cend());

  return it->type() == gdm::ConstituentType::Nucleus;
}

void GDMProxyImpl::setConcentrations(const std::string &name, const std::vector<double> &concentrations) noexcept
{
  const auto C = [this](const double d) {
    if (d >= m_minimumConcentration)
      return d;
    return m_minimumConcentration;
  };

  assert(h_model.find(name) != h_model.cend());
  assert(concentrations.size() == 1);

  auto sampleIt = h_model.find(name);

  h_model.setConcentrations(sampleIt, { C(concentrations.at(0)) } );
}

bool GDMProxyImpl::update(const std::string &name, const gdm::Constituent &ctuent)
{
  auto it = h_model.find(name);
  assert(it != h_model.cend());

  if (!GDMProxy_p::typeChangeOk(ctuent, *it, h_model))
    throw GDMProxyException{"Type changes are not allowed for complexing constituents"};

  auto ret = h_model.update(it, ctuent);

  return std::get<1>(ret);
}

} // namespace gearbox
