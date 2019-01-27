#include "gearboxprivate.h"

#include <QObject>

namespace gearbox {

GearboxPrivate::GearboxPrivate() :
  m_fittedMobilities{QObject::tr("Mobility")},
  m_fittedpKas{QObject::tr("pKa")},
  m_scalFRMapping{nullptr}
{
  m_scalFRData.resize(m_scalFRMapping.indexFromItem(ScalarFitResultsMapping::Items::LAST_INDEX));
  m_scalFRMapping.setUnderlyingData(&m_scalFRData);
}

} // namespace gearbox
