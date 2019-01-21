#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "types.h"

#include <gdm/core/gdm.h>
#include <QString>

class AnalyteInputParameters;
class ChemicalBuffer;
class ChemicalBuffersModel;

namespace persistence {

void saveEntireSetup(const QString &path, const ChemicalBuffersModel &buffers, const AnalyteInputParameters &analyte);
void loadPeakMasterBuffer(const QString &path);
void savePeakMasterBuffer(const QString &path, const ChemicalBuffer &buffer);

} // namespace persistence

#endif // PERSISTENCE_H
