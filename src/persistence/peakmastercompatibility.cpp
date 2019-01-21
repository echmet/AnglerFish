#include "peakmastercompatibility.h"

#include "types.h"

#include <QJsonObject>
#include <QString>


namespace persistence {

const QString PeakMasterCompatibility::ROOT_COMPOSITION_BGE{"compositionBGE"};
const QString PeakMasterCompatibility::ROOT_COMPOSITION_SAMPLE{"compositionSample"};
const QString PeakMasterCompatibility::ROOT_CONCENTRATIONS_BGE{"concentrationsBGE"};
const QString PeakMasterCompatibility::ROOT_CONCENTRATIONS_SAMPLE{"concentrationsSample"};
const QString PeakMasterCompatibility::ROOT_SYSTEM{"system"};

const QString PeakMasterCompatibility::CTUENT_CTUENTS{"constituents"};
const QString PeakMasterCompatibility::CTUENT_TYPE{"type"};
const QString PeakMasterCompatibility::CTUENT_NAME{"name"};
const QString PeakMasterCompatibility::CTUENT_CHARGE_LOW{"chargeLow"};
const QString PeakMasterCompatibility::CTUENT_CHARGE_HIGH{"chargeHigh"};
const QString PeakMasterCompatibility::CTUENT_PKAS{"pKas"};
const QString PeakMasterCompatibility::CTUENT_MOBILITIES{"mobilities"};
const QString PeakMasterCompatibility::CTUENT_VISCOSITY_COEFFICIENT{"viscosityCoefficient"};

const QString PeakMasterCompatibility::CTUENT_TYPE_NUCLEUS{"N"};
const QString PeakMasterCompatibility::CTUENT_TYPE_LIGAND{"L"};

const QString PeakMasterCompatibility::CPX_NAME{"name"};
const QString PeakMasterCompatibility::CPX_CHARGE{"charge"};
const QString PeakMasterCompatibility::CPX_MAX_COUNT{"maxCount"};
const QString PeakMasterCompatibility::CPX_PBS{"pBs"};
const QString PeakMasterCompatibility::CPX_MOBILITIES{"mobilities"};

const QString PeakMasterCompatibility::CPX_COMPLEX_FORMS{"complexForms"};
const QString PeakMasterCompatibility::CPX_NUCLEUS_CHARGE{"nucleusCharge"};
const QString PeakMasterCompatibility::CPX_LIGAND_GROUPS{"ligandGroups"};
const QString PeakMasterCompatibility::CPX_LIGANDS{"ligands"};

const QString PeakMasterCompatibility::SYS_TOTAL_LENGTH{"totalLength"};
const QString PeakMasterCompatibility::SYS_DETECTOR_POSITION{"detectorPosition"};
const QString PeakMasterCompatibility::SYS_DRIVING_VOLTAGE{"drivingVoltage"};
const QString PeakMasterCompatibility::SYS_POSITIVE_VOLTAGE{"positiveVoltage"};
const QString PeakMasterCompatibility::SYS_EOF_TYPE{"eofType"};
const QString PeakMasterCompatibility::SYS_EOF_VALUE{"eofValue"};
const QString PeakMasterCompatibility::SYS_CORRECT_FOR_DEBYE_HUCKEL{"correctForDebyeHuckel"};
const QString PeakMasterCompatibility::SYS_CORRECT_FOR_ONSAGER_FUOSS{"correctForOnsagerFuoss"};
const QString PeakMasterCompatibility::SYS_CORRECT_FOR_VISCOSITY{"correctForViscosity"};
const QString PeakMasterCompatibility::SYS_INJECTION_ZONE_LENGTH{"injectionZoneLength"};

const QString PeakMasterCompatibility::SYS_EOF_TYPE_NONE{"N"};
const QString PeakMasterCompatibility::SYS_EOF_TYPE_MOBILITY{"U"};
const QString PeakMasterCompatibility::SYS_EOF_TYPE_TIME{"T"};

void PeakMasterCompatibility::checkIfContains(const QString &str, const QJsonObject &obj)
{
  if (!obj.contains(str))
    throw Exception{std::string{"Missing "} + str.toStdString()};
}

} // namespace persistence
