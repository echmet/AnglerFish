#include "gdmproxy.h"

namespace gearbox {

GDMProxy::GDMProxy(const double minimumConcentration) :
  m_minimumConcentration{minimumConcentration}
{
}

GDMProxy::~GDMProxy()
{
}

} // namespace gearbox
