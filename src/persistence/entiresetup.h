#ifndef ENTIRESETUP_H
#define ENTIRESETUP_H

#include <gearbox/analyteinputparameters.h>
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

  static std::tuple<std::vector<ChemicalBuffer>, AnalyteInputParameters> load(const QString &path);
  static void save(QString path, const ChemicalBuffersModel &buffers, const AnalyteInputParameters &analyte);

private:
  static AnalyteInputParameters::ParameterVec arrayToParamVec(const QJsonArray &array);
  static AnalyteInputParameters deserializeAnalyte(const QJsonObject &obj);
  static std::vector<ChemicalBuffer> deserializeBuffers(const QJsonArray &array);
  static QJsonArray aParamVecToArray(const AnalyteInputParameters::ParameterVec &vec);
  static QJsonObject serializeAnalyte(const AnalyteInputParameters &analyte);
  static QJsonArray serializeBuffers(const ChemicalBuffersModel &buffers);

  static const QString ROOT_BUFFERS;
  static const QString ROOT_ANALYTE;

  static const QString BUF_COMPOSITION;
  static const QString BUF_CONCENTRATIONS;
  static const QString BUF_EXP_MOBILITIES;

  static const QString ANAL_CHARGE_LOW;
  static const QString ANAL_CHARGE_HIGH;
  static const QString ANAL_PKAS;
  static const QString ANAL_MOBILITIES;
  static const QString ANAL_PVEC_FIXED;
  static const QString ANAL_PVEC_VALUE;

};

} // namespace persistence

#endif // ENTIRESETUP_H
