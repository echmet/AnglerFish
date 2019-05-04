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

gearbox::AnalyteEstimates::ParameterVec EntireSetup::arrayToParamVec(const QJsonArray &array)
{
  gearbox::AnalyteEstimates::ParameterVec vec{};

  for (const auto &item : array) {
    if (!item.isObject())
      throw Exception{"Field in analyte array is not an object"};

    auto obj = item.toObject();
    DeserializeCommon::checkIfContains(ANAL_PVEC_FIXED, obj, QJsonValue::Bool);
    bool fixed = obj[ANAL_PVEC_FIXED].toBool();

    DeserializeCommon::checkIfContains(ANAL_PVEC_VALUE, obj, QJsonValue::Double);
    double val = obj[ANAL_PVEC_VALUE].toDouble();

    vec.emplace_back(val, fixed);
  }

  return vec;
}

gearbox::AnalyteEstimates EntireSetup::deserializeAnalyte(const QJsonObject &obj)
{
  DeserializeCommon::checkIfContainsInt(ANAL_CHARGE_LOW, obj);
  const int chargeLow = obj[ANAL_CHARGE_LOW].toInt();

  DeserializeCommon::checkIfContainsInt(ANAL_CHARGE_HIGH, obj);
  const int chargeHigh = obj[ANAL_CHARGE_HIGH].toInt();

  if (chargeHigh < chargeLow)
    throw Exception{"Invalid charge values"};

  DeserializeCommon::checkIfContains(ANAL_PKAS, obj, QJsonValue::Array);
  auto pKas = arrayToParamVec(obj[ANAL_PKAS].toArray());
  if (pKas.size() != chargeHigh - chargeLow)
    throw Exception{"Unexpected size of pKas array"};

  DeserializeCommon::checkIfContains(ANAL_MOBILITIES, obj, QJsonValue::Array);
  auto mobs = arrayToParamVec(obj[ANAL_MOBILITIES].toArray());
  if (mobs.size() != chargeHigh - chargeLow + 1)
    throw Exception{"Unexpected size of mobilities array"};

  return {chargeLow, chargeHigh, std::move(mobs), std::move(pKas)};
}

std::vector<gearbox::ChemicalBuffer> EntireSetup::deserializeBuffers(const QJsonArray &array, const gearbox::IonicEffectsModel &ionEffs)
{
  std::vector<gearbox::ChemicalBuffer> buffers{};

  for (const auto &item : array) {
    if (!item.isObject())
      throw Exception{"Invalid item in buffers array"};

    QJsonObject obj = item.toObject();

    auto gdmModel = std::make_unique<gdm::GDM>();

    DeserializeCommon::checkIfContains(BUF_COMPOSITION, obj, QJsonValue::Object);
    DeserializeCommon::deserializeComposition(*gdmModel, obj[BUF_COMPOSITION].toObject());

    DeserializeCommon::checkIfContains(BUF_CONCENTRATIONS, obj, QJsonValue::Object);
    DeserializeCommon::deserializeConcentrations(*gdmModel, obj[BUF_CONCENTRATIONS].toObject());

    DeserializeCommon::checkIfContains(BUF_EXP_MOBILITIES, obj, QJsonValue::Array);
    auto expMobsSer = obj[BUF_EXP_MOBILITIES].toArray();

    bool excluded = [&obj]() {
      try {
        DeserializeCommon::checkIfContains(BUF_EXCLUDED, obj, QJsonValue::Bool);
        return obj[BUF_EXCLUDED].toBool();
      } catch (const Exception &) {
        return false;
      }
    }();

    std::vector<double> expMobs{};
    for (const auto d : expMobsSer) {
      if (!d.isDouble())
        throw Exception{"Non-numeric value in experimental mobilities"};

      expMobs.emplace_back(d.toDouble());
    }

    gearbox::ChemicalBuffer buf{&ionEffs, gdmModel.release()};
    buf.setExperimentalMobilities(std::move(expMobs));
    buf.setExclude(excluded);

    buffers.emplace_back(std::move(buf));
  }

  return buffers;
}

std::tuple<std::vector<gearbox::ChemicalBuffer>, gearbox::AnalyteEstimates> EntireSetup::load(const QString &path,
                                                                                              const gearbox::IonicEffectsModel &ionEffs)
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

  DeserializeCommon::checkIfContains(ROOT_BUFFERS, root, QJsonValue::Array);
  auto buffers = deserializeBuffers(root[ROOT_BUFFERS].toArray(), ionEffs);

  DeserializeCommon::checkIfContains(ROOT_ANALYTE, root, QJsonValue::Object);
  auto analyte = deserializeAnalyte(root[ROOT_ANALYTE].toObject());

  return {std::move(buffers), std::move(analyte)};
}

} // namespace persistence
