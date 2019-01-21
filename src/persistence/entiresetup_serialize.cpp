#include "entiresetup.h"

#include "types.h"
#include "serializecommon.h"

#include <gearbox/chemicalbuffersmodel.h>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace persistence {

QJsonArray EntireSetup::aParamVecToArray(const AnalyteInputParameters::ParameterVec &vec)
{
  QJsonArray arr{};

  for (const auto &item : vec) {
    QJsonObject obj{};

    obj[ANAL_PVEC_FIXED] = item.fixed;
    obj[ANAL_PVEC_VALUE] = item.value;

    arr.append(obj);
  }

  return arr;
}

QJsonObject EntireSetup::serializeAnalyte(const AnalyteInputParameters &analyte)
{
  QJsonObject serAnal{};

  serAnal[ANAL_CHARGE_LOW] = analyte.chargeLow;
  serAnal[ANAL_CHARGE_HIGH] = analyte.chargeHigh;

  serAnal[ANAL_PKAS] = aParamVecToArray(analyte.pKas);
  serAnal[ANAL_MOBILITIES] = aParamVecToArray(analyte.mobilities);

  return serAnal;
}

QJsonArray EntireSetup::serializeBuffers(const ChemicalBuffersModel &buffers)
{
  QJsonArray serArr{};

  for (const auto &buf : buffers) {
    QJsonObject serBuf{};

    const auto &model = *buf.model();

    auto composition = SerializeCommon::serializeComposition(model);
    auto concentrations = SerializeCommon::serializeConcentrations(model);
    auto expMobilities = SerializeCommon::generateFromDoubles(buf.experimentalMobilities());

    serBuf[BUF_COMPOSITION] = composition;
    serBuf[BUF_CONCENTRATIONS] = concentrations;
    serBuf[BUF_EXP_MOBILITIES] = expMobilities;

    serArr.append(serBuf);
  }

  return serArr;
}

void EntireSetup::save(QString path, const ChemicalBuffersModel &buffers, const AnalyteInputParameters &analyte)
{
  if (!path.endsWith(".json", Qt::CaseInsensitive))
    path.append(".json");

  QFile fh{path};

  if (!fh.open(QIODevice::WriteOnly | QIODevice::Text))
    throw Exception{"Cannot open output file"};

  auto serBuffers = serializeBuffers(buffers);
  auto serAnalyte = serializeAnalyte(analyte);

  QJsonDocument doc{
    {
      {ROOT_BUFFERS, serBuffers},
      {ROOT_ANALYTE, serAnalyte}
    }
  };

  auto str = doc.toJson();

  fh.write(str);
}

} // namespace persistence

