#ifndef PEAKMASTERCOMPATIBILITY_H
#define PEAKMASTERCOMPATIBILITY_H

#include <gdm/core/gdm.h>
#include <gdm/core/complexation/complexation.h>
#include <map>
#include <string>
#include <vector>

class ChemicalBuffer;
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
  /* Common */
  static void checkIfContains(const QString &str, const QJsonObject &obj);

  /* Deserialization */
  static void deserializeComposition(gdm::GDM &gdm, const QJsonObject &obj);
  static void deserializeConcentrations(gdm::GDM &gdm, const QJsonObject &obj);
  static std::vector<std::pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>> deserializeConstituents(const QJsonArray &arr);
  static std::map<std::string, gdm::Complexation> deserializeNucleusComplexForms(const QJsonObject &obj, const int nucleusChargeLow, const int nucleusChargeHigh);

  /* Serialization */
  static QJsonObject generateFromNucleusComplexForm(const std::string &ligandName, gdm::ChargeNumber ligandCharge, const gdm::ComplexForm &complexForm);
  static QJsonObject serializeComposition(const gdm::GDM &gdm);
  static QJsonObject serializeConcentrations(const gdm::GDM &gdm);
  static QJsonObject serializeSystem();

  static const QString ROOT_COMPOSITION_BGE;
  static const QString ROOT_COMPOSITION_SAMPLE;
  static const QString ROOT_CONCENTRATIONS_BGE;
  static const QString ROOT_CONCENTRATIONS_SAMPLE;
  static const QString ROOT_SYSTEM;

  static const QString CTUENT_CTUENTS;
  static const QString CTUENT_TYPE;
  static const QString CTUENT_NAME;
  static const QString CTUENT_CHARGE_LOW;
  static const QString CTUENT_CHARGE_HIGH;
  static const QString CTUENT_PKAS;
  static const QString CTUENT_MOBILITIES;
  static const QString CTUENT_VISCOSITY_COEFFICIENT;

  static const QString CTUENT_TYPE_NUCLEUS;
  static const QString CTUENT_TYPE_LIGAND;
  static const QString CPX_NAME;
  static const QString CPX_CHARGE;
  static const QString CPX_MAX_COUNT;
  static const QString CPX_PBS;
  static const QString CPX_MOBILITIES;

  static const QString CPX_COMPLEX_FORMS;
  static const QString CPX_NUCLEUS_CHARGE;
  static const QString CPX_LIGAND_GROUPS;
  static const QString CPX_LIGANDS;

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
