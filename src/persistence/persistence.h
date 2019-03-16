#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "types.h"

#include <gdm/core/gdm.h>


namespace gearbox {
  class AnalyteEstimates;
  class ChemicalBuffer;
  class ChemicalBuffersModel;
  class Gearbox;
} // namespace gearbox

namespace persistence {

void loadEntireSetup(const QString &path, gearbox::Gearbox &gbox);
void saveEntireSetup(const QString &path, const gearbox::ChemicalBuffersModel &buffers,
                     const gearbox::AnalyteEstimates &analyte);
void loadPeakMasterBuffer(const Target &target, gearbox::Gearbox &gbox);
void savePeakMasterBuffer(const QString &path, const gearbox::ChemicalBuffer &buffer);

} // namespace persistence

#endif // PERSISTENCE_H
