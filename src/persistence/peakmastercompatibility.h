#ifndef PEAKMASTERCOMPATIBILITY_H
#define PEAKMASTERCOMPATIBILITY_H

#include <gdm/core/gdm.h>

class ChemicalBuffer;
class QJsonObject;
class QString;

namespace persistence {

class PeakMasterCompatibility {
public:
  PeakMasterCompatibility() = delete;

  static gdm::GDM load(const QString &path);
  static void save(const QString &path, const ChemicalBuffer &buffer);
};

} // namespace persistence

#endif // PEAKMASTERCOMPATIBILITY_H
