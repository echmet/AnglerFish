#include "peakmastercompatibility.h"

#include <QString>

namespace persistence {

const QString PeakMasterCompatibility::ROOT_COMPOSITION_BGE{"compositionBGE"};
const QString PeakMasterCompatibility::ROOT_COMPOSITION_SAMPLE{"compositionSample"};
const QString PeakMasterCompatibility::ROOT_CONCENTRATIONS_BGE{"concentrationsBGE"};
const QString PeakMasterCompatibility::ROOT_CONCENTRATIONS_SAMPLE{"concentrationsSample"};
const QString PeakMasterCompatibility::ROOT_SYSTEM{"system"};

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

} // namespace persistence
