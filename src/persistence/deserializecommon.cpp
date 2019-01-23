#include "deserializecommon.h"

#include "datakeys.h"
#include "types.h"

#include <gdm/core/gdm.h>
#include <gdm/core/common/gdmexcept.h>
#include <gdm/core/complexation/complexation.h>
#include <QJsonArray>
#include <QJsonObject>
#include <map>

namespace persistence {

inline
std::map<std::string, gdm::Complexation> deserializeNucleusComplexForms(const QJsonObject &obj, const int nucleusChargeLow, const int nucleusChargeHigh)
{
  std::map<std::string, gdm::Complexation> ret{};

  DeserializeCommon::checkIfContains(DataKeys::CPX_COMPLEX_FORMS, obj, QJsonValue::Array);
  const QJsonArray cforms = obj[DataKeys::CPX_COMPLEX_FORMS].toArray();

  for (const auto &item : cforms) {
    const QJsonObject cf = item.toObject();
    if (cf.isEmpty())
      throw Exception{"Invalid complex form entry"};

    /* Read nucleus charge */
    int charge;
    DeserializeCommon::checkIfContainsInt(DataKeys::CPX_NUCLEUS_CHARGE, cf);
    charge = cf[DataKeys:: CPX_NUCLEUS_CHARGE].toInt();

    if (charge < nucleusChargeLow || charge > nucleusChargeHigh)
      throw Exception{"Nucleus charge in complexForm definition is outside nucleus' charge range"};

    /* Read ligand groups */
    DeserializeCommon::checkIfContains(DataKeys::CPX_LIGAND_GROUPS, cf, QJsonValue::Array);
    const QJsonArray ligandGroups = cf[DataKeys::CPX_LIGAND_GROUPS].toArray();
    if (!ligandGroups.empty())
      throw Exception{"Complexing buffers are not supported"};
  }

  return ret;
}

inline
std::vector<std::pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>> deserializeConstituents(const QJsonArray &arr)
{
  std::vector<std::pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>> ret{};

  ret.reserve(arr.count());

  for (const auto &item : arr) {
    if (item.type() != QJsonValue::Object)
      throw Exception{"Constituent field is not an object"};

    const QJsonObject ctuent = item.toObject();
    if (ctuent.isEmpty())
      throw Exception{"Invalid constituent object"};

    /* Read type */
    gdm::ConstituentType type;
    DeserializeCommon::checkIfContains(DataKeys::CTUENT_TYPE, ctuent, QJsonValue::String);

    auto inType = ctuent[DataKeys::CTUENT_TYPE];
    if (inType == DataKeys::CTUENT_TYPE_NUCLEUS)
      type = gdm::ConstituentType::Nucleus;
    else if (inType == DataKeys::CTUENT_TYPE_LIGAND)
      type = gdm::ConstituentType::Ligand;
    else
      throw Exception{"Invalid \"type\" value"};

    /* Read name */
    std::string name{};
    DeserializeCommon::checkIfContains(DataKeys::CTUENT_NAME, ctuent, QJsonValue::String);
    QString inName = ctuent[DataKeys::CTUENT_NAME].toString();
    if (inName.isNull())
      throw Exception{"Invalid \"name\""};
    name = inName.toStdString();

    /* Read chargeLow */
    int chargeLow;
    DeserializeCommon::checkIfContainsInt(DataKeys::CTUENT_CHARGE_LOW, ctuent);
    chargeLow = ctuent[DataKeys::CTUENT_CHARGE_LOW].toInt();

    /* Read chargeHigh */
    int chargeHigh;
    DeserializeCommon::checkIfContainsInt(DataKeys::CTUENT_CHARGE_HIGH, ctuent);
    chargeHigh = ctuent[DataKeys::CTUENT_CHARGE_HIGH].toInt();

    /* Read viscosityCoefficient */
    double viscosityCoefficient;
    DeserializeCommon::checkIfContains(DataKeys::CTUENT_VISCOSITY_COEFFICIENT, ctuent, QJsonValue::Double);
    viscosityCoefficient = ctuent[DataKeys::CTUENT_VISCOSITY_COEFFICIENT].toDouble();

    if (chargeLow > chargeHigh)
      throw Exception{"Invalid charge values"};

    /* Read pKas */
    std::vector<double> pKas{};
    DeserializeCommon::checkIfContains(DataKeys::CTUENT_PKAS, ctuent, QJsonValue::Array);
    QJsonArray inpKas = ctuent[DataKeys::CTUENT_PKAS].toArray();
    if (inpKas.size() != chargeHigh - chargeLow)
      throw Exception{"Invalid pKa array size"};
    for (const auto d : inpKas) {
      if (!d.isDouble())
        throw Exception{"Unexpected type of pKa value"};

      pKas.emplace_back(d.toDouble());
    }

    /* Read mobilities */
    std::vector<double> mobilities{};
    DeserializeCommon::checkIfContains(DataKeys::CTUENT_MOBILITIES, ctuent, QJsonValue::Array);
    QJsonArray inMobilities = ctuent[DataKeys::CTUENT_MOBILITIES].toArray();
    if (inMobilities.size() != chargeHigh - chargeLow + 1)
      throw Exception{"Invalid mobilities array size"};
    for (const auto u : inMobilities) {
      if (!u.isDouble())
        throw Exception{"Unexpected type of mobility value"};

      mobilities.emplace_back(u.toDouble());
    }

    gdm::ChargeInterval charges{chargeLow, chargeHigh};
    gdm::PhysicalProperties physProps{charges, pKas, mobilities, viscosityCoefficient};

    if (type == gdm::ConstituentType::Nucleus) {
      auto complexForms = deserializeNucleusComplexForms(ctuent, chargeLow, chargeHigh);
      ret.emplace_back(std::make_pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>({type, name, physProps}, std::move(complexForms)));
    } else {
      if (ctuent.contains(DataKeys::CPX_COMPLEX_FORMS))
        throw Exception{"Ligands must not have \"complexForms\""};

      ret.emplace_back(std::make_pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>({type, name, physProps}, {}));
    }
  }

  return ret;
}

void DeserializeCommon::checkIfContains(const QString &str, const QJsonObject &obj, const QJsonValue::Type type)
{
  if (!obj.contains(str))
    throw Exception{std::string{"Missing "} + str.toStdString()};

  if (obj[str].type() != type)
    throw Exception{std::string{"Unexpected type of field "} + str.toStdString()};
}

void DeserializeCommon::checkIfContainsInt(const QString &str, const QJsonObject &obj)
{
  if (!obj.contains(str))
    throw Exception{std::string{"Missing "} + str.toStdString()};
}

void DeserializeCommon::deserializeComposition(gdm::GDM &gdm, const QJsonObject &obj)
{
  checkIfContains(DataKeys::CTUENT_CTUENTS, obj, QJsonValue::Array);
  try {
    const auto constituents = deserializeConstituents(obj[DataKeys::CTUENT_CTUENTS].toArray());

    /* Insert all constituents first*/
    for (const auto &ctuent : constituents) {
      bool inserted;
      std::tie(std::ignore, inserted) = gdm.insert(ctuent.first);
      if (!inserted)
        throw Exception("Duplicit constituents");
    }

    /* Insert all complexations */
    for (const auto & ctuent : constituents) {
      auto nucleusIt = gdm.find(ctuent.first.name());
      if (nucleusIt == gdm.end())
        throw Exception{"Internal deserialization error"};

      for (const auto &cf : ctuent.second) {
        auto ligandIt = gdm.find(cf.first);
        if (ligandIt == gdm.end())
          throw Exception{"ComplexForm refers to missing ligand"};

        if (gdm.haveComplexation(nucleusIt, ligandIt))
          throw Exception{"Internal deserialization error"};

        gdm.setComplexation(nucleusIt, ligandIt, cf.second);
      }
    }
  } catch (const gdm::LogicError &ex) {
    throw Exception{std::string{"Failed to parse constituents: "} + std::string{ex.what()}};
  }
}

void DeserializeCommon::deserializeConcentrations(gdm::GDM &gdm, const QJsonObject &obj)
{
  for (auto ctuentIt = gdm.begin(); ctuentIt != gdm.end(); ctuentIt++) {
    const QString name = QString::fromStdString(ctuentIt->name());
    checkIfContains(name, obj, QJsonValue::Array);

    QJsonArray arr = obj[name].toArray();
    if (arr.size() != 1)
      throw Exception{"Invalid concentrations array size"};

    if (arr.first().type() != QJsonValue::Double)
      throw Exception{"Unexpected type of concentration value"};

    gdm.setConcentrations(ctuentIt, { arr.first().toDouble() });
  }
}

} // namespace persistence
