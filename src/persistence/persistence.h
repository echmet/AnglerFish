#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "types.h"

#include <gdm/core/gdm.h>
#include <QString>

class ChemicalBuffer;

namespace persistence {

void loadPeakMasterBuffer(const QString &path);
void savePeakMasterBuffer(const QString &path, const ChemicalBuffer &buffer);

} // namespace persistence

#endif // PERSISTENCE_H
