#include "peakmastercompatibility.h"

#include "datakeys.h"
#include "deserializecommon.h"

#include <trstr.h>
#include <gdm/core/common/gdmexcept.h>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <cassert>

namespace persistence {

gdm::GDM PeakMasterCompatibility::load(const Target &target)
{
  gdm::GDM model;

  auto data = [&target]() {
    if (target.type == Target::TT_FILE) {
      QFile fh{target.path};

      if (!fh.open(QIODevice::ReadOnly | QIODevice::Text))
        throw Exception{trstr("Cannot open input file")};

      return fh.readAll();
    } else if (target.type == Target::TT_CLIPBOARD) {
      auto clipboard = QApplication::clipboard();
      if (clipboard == nullptr)
        throw Exception{trstr("Cannot access system clipboard")};

      return clipboard->text().toUtf8();
    } else
      assert(false); /* Invalid target type */

  }();

  QJsonParseError parseError{};
  QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
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
