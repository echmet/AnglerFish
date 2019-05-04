#ifndef ENTIRESETUP_H
#define ENTIRESETUP_H

#include <gearbox/analyteestimates.h>
#include <gearbox/chemicalbuffersmodel.h>
#include <QString>
#include <tuple>
#include <vector>

class QJsonArray;
class QJsonObject;

namespace persistence {

class EntireSetup {
public:
  EntireSetup() = delete;

  static std::tuple<std::vector<gearbox::ChemicalBuffer>, gearbox::AnalyteEstimates> load(const QString &path, const gearbox::IonicEffectsModel &ionEffs);
  static void save(QString path, const gearbox::ChemicalBuffersModel &buffers, const gearbox::AnalyteEstimates &analyte);

private:
  static gearbox::AnalyteEstimates::ParameterVec arrayToParamVec(const QJsonArray &array);
  static gearbox::AnalyteEstimates deserializeAnalyte(const QJsonObject &obj);
  static std::vector<gearbox::ChemicalBuffer> deserializeBuffers(const QJsonArray &array, const gearbox::IonicEffectsModel &ionEffs);
  static QJsonArray aParamVecToArray(const gearbox::AnalyteEstimates::ParameterVec &vec);
  static QJsonObject serializeAnalyte(const gearbox::AnalyteEstimates &analyte);
  static QJsonArray serializeBuffers(const gearbox::ChemicalBuffersModel &buffers);

  static const QString ROOT_BUFFERS;
  static const QString ROOT_ANALYTE;

  static const QString BUF_COMPOSITION;
  static const QString BUF_CONCENTRATIONS;
  static const QString BUF_EXP_MOBILITIES;
  static const QString BUF_EXCLUDED;

  static const QString ANAL_CHARGE_LOW;
  static const QString ANAL_CHARGE_HIGH;
  static const QString ANAL_PKAS;
  static const QString ANAL_MOBILITIES;
  static const QString ANAL_PVEC_FIXED;
  static const QString ANAL_PVEC_VALUE;

};

} // namespace persistence

#endif // ENTIRESETUP_H
