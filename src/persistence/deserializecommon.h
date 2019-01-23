#ifndef DESERIALIZECOMMON_H
#define DESERIALIZECOMMON_H

#include <QJsonValue>

class QJsonObject;
class QString;

namespace gdm {
  class GDM;
}

namespace persistence {

class DeserializeCommon {
public:
  DeserializeCommon() = delete;

  static void checkIfContains(const QString &str, const QJsonObject &obj, const QJsonValue::Type type);
  static void checkIfContainsInt(const QString &str, const QJsonObject &obj);
  static void deserializeComposition(gdm::GDM &gdm, const QJsonObject &obj);
  static void deserializeConcentrations(gdm::GDM &gdm, const QJsonObject &obj);
};

} // namespace persistence

#endif // DESERIALIZECOMMON_H
