#include "peakmastercompatibility.h"

#include "types.h"
#include "datakeys.h"
#include "deserializecommon.h"

#include <trstr.h>
#include <gdm/core/common/gdmexcept.h>
#include <QFile>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace persistence {

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
    throw Exception{trstr("Bad root object")};

  DeserializeCommon::checkIfContains(ROOT_COMPOSITION_BGE, obj, QJsonValue::Object);
  DeserializeCommon::deserializeComposition(model, obj[ROOT_COMPOSITION_BGE].toObject());

  DeserializeCommon::checkIfContains(ROOT_CONCENTRATIONS_BGE, obj, QJsonValue::Object);
  DeserializeCommon::deserializeConcentrations(model, obj[ROOT_CONCENTRATIONS_BGE].toObject());

  return model;
}

} // namespace persistence
