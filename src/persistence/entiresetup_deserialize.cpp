#include "entiresetup.h"

#include "deserializecommon.h"
#include "types.h"

#include <trstr.h>
#include <gearbox/chemicalbuffer.h>
#include <gdm/core/gdm.h>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <memory>

namespace persistence {

AnalyteInputParameters::ParameterVec EntireSetup::arrayToParamVec(const QJsonArray &array)
{
  AnalyteInputParameters::ParameterVec vec{};

  for (const auto &item : array) {
    if (!item.isObject())
      throw Exception{"Field in analyte array is not an object"};

    auto obj = item.toObject();
    DeserializeCommon::checkIfContains(ANAL_PVEC_FIXED, obj);
    if (!obj[ANAL_PVEC_FIXED].isBool())
      throw Exception{"Fixed field is not boolean"};
    bool fixed = obj[ANAL_PVEC_FIXED].toBool();

    DeserializeCommon::checkIfContains(ANAL_PVEC_VALUE, obj);
    if (!obj[ANAL_PVEC_VALUE].isDouble())
      throw Exception{"Value field is not double"};
    double val = obj[ANAL_PVEC_VALUE].toDouble();

    vec.emplace_back(val, fixed);
  }

  return vec;
}

AnalyteInputParameters EntireSetup::deserializeAnalyte(const QJsonObject &obj)
{
  DeserializeCommon::checkIfContains(ANAL_CHARGE_LOW, obj);
  const int chargeLow = obj[ANAL_CHARGE_LOW].toInt();

  DeserializeCommon::checkIfContains(ANAL_CHARGE_HIGH, obj);
  const int chargeHigh = obj[ANAL_CHARGE_HIGH].toInt();

  if (chargeHigh < chargeLow)
    throw Exception{"Invalid charge values"};

  DeserializeCommon::checkIfContains(ANAL_PKAS, obj);
  if (!obj[ANAL_PKAS].isArray())
    throw Exception{"pKas is not an array"};
  auto pKas = arrayToParamVec(obj[ANAL_PKAS].toArray());
  if (pKas.size() != chargeHigh - chargeLow)
    throw Exception{"Unexpected size of pKas array"};

  DeserializeCommon::checkIfContains(ANAL_MOBILITIES, obj);
  if (!obj[ANAL_MOBILITIES].isArray())
    throw Exception{"Mobilities is not an array"};
  auto mobs = arrayToParamVec(obj[ANAL_MOBILITIES].toArray());
  if (mobs.size() != chargeHigh - chargeLow + 1)
    throw Exception{"Unexpected size of mobilities array"};

  return {chargeLow, chargeHigh, std::move(mobs), std::move(pKas)};
}

std::vector<ChemicalBuffer> EntireSetup::deserializeBuffers(const QJsonArray &array)
{
  std::vector<ChemicalBuffer> buffers{};

  for (const auto &item : array) {
    if (!item.isObject())
      throw Exception{"Invalid item in buffers array"};

    QJsonObject obj = item.toObject();

    auto gdmModel = std::make_unique<gdm::GDM>();

    DeserializeCommon::checkIfContains(BUF_COMPOSITION, obj);
    if (!obj[BUF_COMPOSITION].isObject())
      throw Exception{"Composition is not an object"};
    DeserializeCommon::deserializeComposition(*gdmModel, obj[BUF_COMPOSITION].toObject());

    DeserializeCommon::checkIfContains(BUF_CONCENTRATIONS, obj);
    if (!obj[BUF_CONCENTRATIONS].isObject())
      throw Exception{"Concentrations is not an object"};
    DeserializeCommon::deserializeConcentrations(*gdmModel, obj[BUF_CONCENTRATIONS].toObject());

    DeserializeCommon::checkIfContains(BUF_EXP_MOBILITIES, obj);
    if (!obj[BUF_EXP_MOBILITIES].isArray())
      throw Exception{"Experimental mobilities field is not an array"};
    auto expMobsSer = obj[BUF_EXP_MOBILITIES].toArray();

    std::vector<double> expMobs{};
    for (const auto d : expMobsSer) {
      if (!d.isDouble())
        throw Exception{"Non-numeric value in experimental mobilities"};

      expMobs.emplace_back(d.toDouble());
    }

    ChemicalBuffer buf{gdmModel.release()};
    buf.setExperimentalMobilities(std::move(expMobs));

    buffers.emplace_back(std::move(buf));
  }

  return buffers;
}

std::tuple<std::vector<ChemicalBuffer>, AnalyteInputParameters> EntireSetup::load(const QString &path)
{
  QFile fh{path};

  if (!fh.open(QIODevice::ReadOnly))
    throw Exception{trstr("Cannot open input file")};

  QJsonParseError parseError{};
  QJsonDocument doc = QJsonDocument::fromJson(fh.readAll(), &parseError);
  if (doc.isNull())
    throw Exception{parseError.errorString().toStdString()};

  auto root = doc.object();
  if (root.empty())
    throw Exception{trstr("Bad root object")};

  DeserializeCommon::checkIfContains(ROOT_BUFFERS, root);
  if (!root[ROOT_BUFFERS].isArray())
    throw Exception{"Buffers is not an array"};
  auto buffers = deserializeBuffers(root[ROOT_BUFFERS].toArray());

  DeserializeCommon::checkIfContains(ROOT_ANALYTE, root);
  if (!root[ROOT_ANALYTE].isObject())
    throw Exception{"Analyte is not an object"};
  auto analyte = deserializeAnalyte(root[ROOT_ANALYTE].toObject());

  return {std::move(buffers), std::move(analyte)};
}

} // namespace persistence
