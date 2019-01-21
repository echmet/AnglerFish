#ifndef SERIALIZECOMMON_H
#define SERIALIZECOMMON_H

#include <gdm/core/gdm.h>
#include <gdm/core/complexation/complexform.h>
#include <QJsonArray>
#include <QJsonObject>

namespace persistence {

class SerializeCommon {
public:
  SerializeCommon() = delete ;

  static QJsonArray generateFromDoubles(const std::vector<double> &src);
  static QJsonObject serializeComposition(const gdm::GDM &gdm);
  static QJsonObject serializeConcentrations(const gdm::GDM &gdm);

private:
  static QJsonObject generateFromNucleusComplexForm(const std::string &ligandName, gdm::ChargeNumber ligandCharge, const gdm::ComplexForm &complexForm);
};

} // namespace persistence

#endif // SERIALIZECOMMON_H
