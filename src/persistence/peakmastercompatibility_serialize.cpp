#include "peakmastercompatibility.h"

#include "types.h"
#include "datakeys.h"
#include "serializecommon.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace persistence {

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

  QJsonObject compositionBGE = SerializeCommon:: serializeComposition(*modelBGE);
  QJsonObject compositionSample = SerializeCommon::serializeComposition(*modelSample);
  QJsonObject concentrationsBGE = SerializeCommon::serializeConcentrations(*modelBGE);
  QJsonObject concentrationsSample = SerializeCommon::serializeConcentrations(*modelSample);
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
