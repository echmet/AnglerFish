#include "peakmastercompatibility.h"

#include "types.h"

#include <trstr.h>
#include <gearbox/chemicalbuffer.h>
#include <gdm/core/common/gdmexcept.h>
#include <QFile>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace persistence {

std::map<std::string, gdm::Complexation>
PeakMasterCompatibility::deserializeNucleusComplexForms(const QJsonObject &obj, const int nucleusChargeLow, const int nucleusChargeHigh)
{
  std::map<std::string, gdm::Complexation> ret{};

  checkIfContains(CPX_COMPLEX_FORMS, obj);
  const QJsonArray cforms = obj[CPX_COMPLEX_FORMS].toArray();

  for (const auto &item : cforms) {
    const QJsonObject cf = item.toObject();
    if (cf.isEmpty())
      throw Exception{"Invalid complex form entry"};

    /* Read nucleus charge */
    int charge;
    checkIfContains(CPX_NUCLEUS_CHARGE, cf);
    charge = cf[CPX_NUCLEUS_CHARGE].toInt();

    if (charge < nucleusChargeLow || charge > nucleusChargeHigh)
      throw Exception{"Nucleus charge in complexForm definition is outside nucleus' charge range"};

    /* Read ligand groups */
    checkIfContains(CPX_LIGAND_GROUPS, cf);
    const QJsonArray ligandGroups = cf[CPX_LIGAND_GROUPS].toArray();
    if (!ligandGroups.empty())
      throw Exception{"Complexing buffers are not supported"};
  }

  return ret;
}

std::vector<std::pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>>
PeakMasterCompatibility::deserializeConstituents(const QJsonArray &arr)
{
  std::vector<std::pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>> ret{};

  ret.reserve(arr.count());

  for (const auto &item : arr) {
    const QJsonObject ctuent = item.toObject();
    if (ctuent.isEmpty())
      throw Exception{"Invalid constituent object"};

    /* Read type */
    gdm::ConstituentType type;
    checkIfContains(CTUENT_TYPE, ctuent);

    auto inType = ctuent[CTUENT_TYPE];
    if (inType == CTUENT_TYPE_NUCLEUS)
      type = gdm::ConstituentType::Nucleus;
    else if (inType == CTUENT_TYPE_LIGAND)
      type = gdm::ConstituentType::Ligand;
    else
      throw Exception{"Invalid \"type\" value"};

    /* Read name */
    std::string name{};
    checkIfContains(CTUENT_NAME, ctuent);
    QString inName = ctuent[CTUENT_NAME].toString();
    if (inName.isNull())
      throw Exception{"Invalid \"name\""};
    name = inName.toStdString();

    /* Read chargeLow */
    int chargeLow;
    checkIfContains(CTUENT_CHARGE_LOW, ctuent);
    chargeLow = ctuent[CTUENT_CHARGE_LOW].toInt();

    /* Read chargeHigh */
    int chargeHigh;
    checkIfContains(CTUENT_CHARGE_HIGH, ctuent);
    chargeHigh = ctuent[CTUENT_CHARGE_HIGH].toInt();

    /* Read viscosityCoefficient */
    double viscosityCoefficient;
    checkIfContains(CTUENT_VISCOSITY_COEFFICIENT, ctuent);
    viscosityCoefficient = ctuent[CTUENT_VISCOSITY_COEFFICIENT].toDouble();

    if (chargeLow > chargeHigh)
      throw Exception{"Invalid charge values"};

    /* Read pKas */
    std::vector<double> pKas{};
    checkIfContains(CTUENT_PKAS, ctuent);
    QJsonArray inpKas = ctuent[CTUENT_PKAS].toArray();
    if (inpKas.size() != chargeHigh - chargeLow)
      throw Exception{"Invalid pKa array size"};
    for (const auto d : inpKas)
      pKas.emplace_back(d.toDouble());

    /* Read mobilities */
    std::vector<double> mobilities{};
    checkIfContains(CTUENT_MOBILITIES, ctuent);
    QJsonArray inMobilities = ctuent[CTUENT_MOBILITIES].toArray();
    if (inMobilities.size() != chargeHigh - chargeLow + 1)
      throw Exception{"Invalid mobilities array size"};
    for (const auto u : inMobilities)
      mobilities.emplace_back(u.toDouble());

    gdm::ChargeInterval charges{chargeLow, chargeHigh};
    gdm::PhysicalProperties physProps{charges, pKas, mobilities, viscosityCoefficient};

    if (type == gdm::ConstituentType::Nucleus) {
      auto complexForms = deserializeNucleusComplexForms(ctuent, chargeLow, chargeHigh);
      ret.emplace_back(std::make_pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>({type, name, physProps}, std::move(complexForms)));
    } else {
      if (ctuent.contains(CPX_COMPLEX_FORMS))
        throw Exception{"Ligands must not have \"complexForms\""};

      ret.emplace_back(std::make_pair<gdm::Constituent, std::map<std::string, gdm::Complexation>>({type, name, physProps}, {}));
    }
  }

  return ret;
}

void PeakMasterCompatibility::deserializeComposition(gdm::GDM &gdm, const QJsonObject &obj)
{
  checkIfContains(CTUENT_CTUENTS, obj);
  try {
    const auto constituents = deserializeConstituents(obj[CTUENT_CTUENTS].toArray());

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

void PeakMasterCompatibility::deserializeConcentrations(gdm::GDM &gdm, const QJsonObject &obj)
{
  for (auto ctuentIt = gdm.begin(); ctuentIt != gdm.end(); ctuentIt++) {
    const QString name = QString::fromStdString(ctuentIt->name());
    checkIfContains(name, obj);

    QJsonArray arr = obj[name].toArray();
    if (arr.size() != 1)
      throw Exception{"Invalid concentrations array size"};

    gdm.setConcentrations(ctuentIt, { arr.first().toDouble() });
  }

}

gdm::GDM PeakMasterCompatibility::load(const QString &path)
{
  gdm::GDM model;

  QFile fh{path};

  if (!fh.open(QIODevice::ReadOnly))
    throw Exception{trstr("Cannot open input file")};


  QJsonParseError parseError{};
  QJsonDocument doc = QJsonDocument::fromJson(fh.readAll(), &parseError);
  if (doc.isNull())
    throw Exception{parseError.errorString().toStdString()};

  QJsonObject obj = doc.object();
  if (obj.isEmpty())
    throw Exception{"Bad root object"};

  checkIfContains(ROOT_COMPOSITION_BGE, obj);
  deserializeComposition(model, obj[ROOT_COMPOSITION_BGE].toObject());

  checkIfContains(ROOT_CONCENTRATIONS_BGE, obj);
  deserializeConcentrations(model, obj[ROOT_CONCENTRATIONS_BGE].toObject());

  return model;
}

} // namespace persistence
