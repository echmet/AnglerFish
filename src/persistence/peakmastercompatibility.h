#ifndef PEAKMASTERCOMPATIBILITY_H
#define PEAKMASTERCOMPATIBILITY_H

#include <gdm/core/gdm.h>

class QJsonArray;
class QJsonObject;
class QString;

namespace persistence {

class PeakMasterCompatibility {
public:
  PeakMasterCompatibility() = delete;

  static gdm::GDM load(const QString &path);
  static void save(QString path, const gdm::GDM *modelBGE, const gdm::GDM *modelSample);

private:
  static QJsonObject serializeSystem();

  static const QString ROOT_COMPOSITION_BGE;
  static const QString ROOT_COMPOSITION_SAMPLE;
  static const QString ROOT_CONCENTRATIONS_BGE;
  static const QString ROOT_CONCENTRATIONS_SAMPLE;
  static const QString ROOT_SYSTEM;

  static const QString SYS_TOTAL_LENGTH;
  static const QString SYS_DETECTOR_POSITION;
  static const QString SYS_DRIVING_VOLTAGE;
  static const QString SYS_POSITIVE_VOLTAGE;
  static const QString SYS_EOF_TYPE;
  static const QString SYS_EOF_VALUE;
  static const QString SYS_CORRECT_FOR_DEBYE_HUCKEL;
  static const QString SYS_CORRECT_FOR_ONSAGER_FUOSS;
  static const QString SYS_CORRECT_FOR_VISCOSITY;
  static const QString SYS_INJECTION_ZONE_LENGTH;

  static const QString SYS_EOF_TYPE_NONE;
  static const QString SYS_EOF_TYPE_MOBILITY;
  static const QString SYS_EOF_TYPE_TIME;
};

} // namespace persistence

#endif // PEAKMASTERCOMPATIBILITY_H
