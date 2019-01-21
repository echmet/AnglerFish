#include "peakmastercompatibility.h"

#include "types.h"

#include <gearbox/chemicalbuffer.h>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace persistence {

inline
QJsonArray generateFromDoubles(const std::vector<double> &src)
{
  QJsonArray ret{};

  std::copy(src.begin(), src.end(), std::back_inserter(ret));

  return ret;
}

QJsonObject PeakMasterCompatibility::generateFromNucleusComplexForm(const std::string &ligandName, gdm::ChargeNumber ligandCharge, const gdm::ComplexForm &complexForm)
{
  QJsonObject ret{};

  ret[CPX_NAME] = QString::fromStdString(ligandName);
  ret[CPX_CHARGE] = ligandCharge;
  ret[CPX_MAX_COUNT] = static_cast<int>(complexForm.pBs().size());
  ret[CPX_PBS] = generateFromDoubles(complexForm.pBs());
  ret[CPX_MOBILITIES] = generateFromDoubles(complexForm.mobilities());

  return ret;
}

QJsonObject PeakMasterCompatibility::serializeComposition(const gdm::GDM &gdm)
{
   QJsonArray constituents{};

   for (auto it = gdm.cbegin(); it != gdm.cend(); ++it) {
     QJsonObject constituent{};

     switch (it->type()) {
     case gdm::ConstituentType::Nucleus:
       constituent[CTUENT_TYPE] = CTUENT_TYPE_NUCLEUS;
       break;
     case gdm::ConstituentType::Ligand:
       constituent[CTUENT_TYPE] = CTUENT_TYPE_LIGAND;
       break;
     }

     constituent[CTUENT_NAME] = QString::fromStdString(it->name());

     constituent[CTUENT_CHARGE_LOW] = it->physicalProperties().charges().low();
     constituent[CTUENT_CHARGE_HIGH] = it->physicalProperties().charges().high();

     constituent[CTUENT_PKAS] = generateFromDoubles(it->physicalProperties().pKas());
     constituent[CTUENT_MOBILITIES] = generateFromDoubles(it->physicalProperties().mobilities());
     constituent[CTUENT_VISCOSITY_COEFFICIENT] = it->physicalProperties().viscosityCoefficient();

     //complexForms
     if (it->type() == gdm::ConstituentType::Nucleus) {
       QJsonArray complexForms{};

       auto nucleusCharges = it->physicalProperties().charges();
       auto foundComplexations = findComplexations(gdm.composition(), it);

       for (auto nucleusCharge = nucleusCharges.low(); nucleusCharge <= nucleusCharges.high(); ++nucleusCharge) {
         QJsonObject complexForm{};

         complexForm[CPX_NUCLEUS_CHARGE] = nucleusCharge;

         QJsonArray ligandGroups{};

         for (const auto &foundComplexation : foundComplexations) {
           Q_ASSERT(foundComplexation.first->type() == gdm::ConstituentType::Ligand);

           const auto& ligandIt = foundComplexation.first;
           const auto& complexation = foundComplexation.second;

           auto ligandCharges = ligandIt->physicalProperties().charges();
           for (auto ligandCharge = ligandCharges.low(); ligandCharge <= ligandCharges.high(); ++ligandCharge) {
             gdm::ChargeCombination charges{nucleusCharge, ligandCharge};

             auto complexFormIt = complexation.find(charges);

             if (complexFormIt != complexation.end()) {
               auto ligand = generateFromNucleusComplexForm(ligandIt->name(), ligandCharge, *complexFormIt);

               QJsonObject ligandGroup{{CPX_LIGANDS, QJsonArray{ligand}}};

               ligandGroups.push_back(ligandGroup);
             }
           }
         }
         complexForm[CPX_LIGAND_GROUPS] = ligandGroups;

         complexForms.push_back(complexForm);
       }
       constituent[CPX_COMPLEX_FORMS] = complexForms;
    }
    constituents.push_back(constituent);
  }

  return QJsonObject{{CTUENT_CTUENTS, constituents}};
}

QJsonObject PeakMasterCompatibility::serializeConcentrations(const gdm::GDM &gdm)
{
  QJsonObject concentrations;

  for (auto it = gdm.cbegin(); it != gdm.cend(); it++) {
    auto sampleConcs = gdm.concentrations(it);
    assert(sampleConcs.size() == 1);

    const QString name = QString::fromStdString(it->name());
    QJsonArray _concs;
    for (auto d : sampleConcs)
      _concs.append(d);

    concentrations.insert(name, _concs);
  }

  return concentrations;
}

QJsonObject PeakMasterCompatibility::serializeSystem()
{
  QJsonObject sys{
    {SYS_TOTAL_LENGTH, 50.0},
    {SYS_DETECTOR_POSITION, 41.5},
    {SYS_DRIVING_VOLTAGE, 20000},
    {SYS_POSITIVE_VOLTAGE, true},
    {SYS_EOF_TYPE, SYS_EOF_TYPE_NONE},
    {SYS_EOF_VALUE, 0.0},
    {SYS_CORRECT_FOR_DEBYE_HUCKEL, true},
    {SYS_CORRECT_FOR_ONSAGER_FUOSS, true},
    {SYS_CORRECT_FOR_VISCOSITY, false},
    {SYS_INJECTION_ZONE_LENGTH, 1.0}
  };

  return sys;
}

void PeakMasterCompatibility::save(QString path, const gdm::GDM *modelBGE, const gdm::GDM *modelSample)
{
  if (!path.endsWith(".json", Qt::CaseInsensitive))
    path.append(".json");

  QFile fh{path};

  if (!fh.open(QIODevice::WriteOnly | QIODevice::Text))
    throw Exception{"Cannot open output file"};

  QJsonObject compositionBGE = serializeComposition(*modelBGE);
  QJsonObject compositionSample = serializeComposition(*modelSample);
  QJsonObject concentrationsBGE = serializeConcentrations(*modelBGE);
  QJsonObject concentrationsSample = serializeConcentrations(*modelSample);
  QJsonObject sys = serializeSystem();

  QJsonDocument doc{{{ROOT_COMPOSITION_BGE, compositionBGE},
                     {ROOT_COMPOSITION_SAMPLE, compositionSample},
                     {ROOT_CONCENTRATIONS_BGE, concentrationsBGE},
                     {ROOT_CONCENTRATIONS_SAMPLE, concentrationsSample},
                     {ROOT_SYSTEM, sys},
                    }};

  auto str = doc.toJson();

  fh.write(str);
}

} // namespace persistence
