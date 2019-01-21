#include "serializecommon.h"

#include "datakeys.h"

#include <QJsonArray>

namespace persistence {

inline
QJsonArray generateFromDoubles(const std::vector<double> &src)
{
  QJsonArray ret{};

  std::copy(src.begin(), src.end(), std::back_inserter(ret));

  return ret;
}

QJsonObject SerializeCommon::generateFromNucleusComplexForm(const std::string &ligandName, gdm::ChargeNumber ligandCharge, const gdm::ComplexForm &complexForm)
{
  QJsonObject ret{};

  ret[DataKeys::CPX_NAME] = QString::fromStdString(ligandName);
  ret[DataKeys::CPX_CHARGE] = ligandCharge;
  ret[DataKeys::CPX_MAX_COUNT] = static_cast<int>(complexForm.pBs().size());
  ret[DataKeys::CPX_PBS] = generateFromDoubles(complexForm.pBs());
  ret[DataKeys::CPX_MOBILITIES] = generateFromDoubles(complexForm.mobilities());

  return ret;
}

QJsonObject SerializeCommon::serializeComposition(const gdm::GDM &gdm)
{
   QJsonArray constituents{};

   for (auto it = gdm.cbegin(); it != gdm.cend(); ++it) {
     QJsonObject constituent{};

     switch (it->type()) {
     case gdm::ConstituentType::Nucleus:
       constituent[DataKeys::CTUENT_TYPE] = DataKeys::CTUENT_TYPE_NUCLEUS;
       break;
     case gdm::ConstituentType::Ligand:
       constituent[DataKeys::CTUENT_TYPE] = DataKeys::CTUENT_TYPE_LIGAND;
       break;
     }

     constituent[DataKeys::CTUENT_NAME] = QString::fromStdString(it->name());

     constituent[DataKeys::CTUENT_CHARGE_LOW] = it->physicalProperties().charges().low();
     constituent[DataKeys::CTUENT_CHARGE_HIGH] = it->physicalProperties().charges().high();

     constituent[DataKeys::CTUENT_PKAS] = generateFromDoubles(it->physicalProperties().pKas());
     constituent[DataKeys::CTUENT_MOBILITIES] = generateFromDoubles(it->physicalProperties().mobilities());
     constituent[DataKeys::CTUENT_VISCOSITY_COEFFICIENT] = it->physicalProperties().viscosityCoefficient();

     //complexForms
     if (it->type() == gdm::ConstituentType::Nucleus) {
       QJsonArray complexForms{};

       auto nucleusCharges = it->physicalProperties().charges();
       auto foundComplexations = findComplexations(gdm.composition(), it);

       for (auto nucleusCharge = nucleusCharges.low(); nucleusCharge <= nucleusCharges.high(); ++nucleusCharge) {
         QJsonObject complexForm{};

         complexForm[DataKeys::CPX_NUCLEUS_CHARGE] = nucleusCharge;

         QJsonArray ligandGroups{};

         for (const auto &foundComplexation : foundComplexations) {
           assert(foundComplexation.first->type() == gdm::ConstituentType::Ligand);

           const auto& ligandIt = foundComplexation.first;
           const auto& complexation = foundComplexation.second;

           auto ligandCharges = ligandIt->physicalProperties().charges();
           for (auto ligandCharge = ligandCharges.low(); ligandCharge <= ligandCharges.high(); ++ligandCharge) {
             gdm::ChargeCombination charges{nucleusCharge, ligandCharge};

             auto complexFormIt = complexation.find(charges);

             if (complexFormIt != complexation.end()) {
               auto ligand = generateFromNucleusComplexForm(ligandIt->name(), ligandCharge, *complexFormIt);

               QJsonObject ligandGroup{{DataKeys::CPX_LIGANDS, QJsonArray{ligand}}};

               ligandGroups.push_back(ligandGroup);
             }
           }
         }
         complexForm[DataKeys::CPX_LIGAND_GROUPS] = ligandGroups;

         complexForms.push_back(complexForm);
       }
       constituent[DataKeys::CPX_COMPLEX_FORMS] = complexForms;
    }
    constituents.push_back(constituent);
  }

  return QJsonObject{{DataKeys::CTUENT_CTUENTS, constituents}};
}

QJsonObject SerializeCommon::serializeConcentrations(const gdm::GDM &gdm)
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

} // namespace persistence
