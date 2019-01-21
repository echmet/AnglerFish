#include "persistence.h"

#include "entiresetup.h"
#include "peakmastercompatibility.h"

#include <gearbox/gearbox.h>

namespace persistence {

void loadEntireSetup(const QString &path)
{
  auto loaded = EntireSetup::load(path);

  auto gbox = Gearbox::instance();

  gbox->chemicalBuffersModel().setBuffers(std::move(std::get<0>(loaded)));

}

void saveEntireSetup(const QString &path, const ChemicalBuffersModel &buffers, const AnalyteInputParameters &analyte)
{
  EntireSetup::save(path, buffers, analyte);
}

void loadPeakMasterBuffer(const QString &path)
{
  auto model = PeakMasterCompatibility::load(path);

  auto gbox = Gearbox::instance();
  auto newModel = new gdm::GDM{std::move(model)};

  gbox->chemicalBuffersModel().add(ChemicalBuffer{newModel});
}

void savePeakMasterBuffer(const QString &path, const ChemicalBuffer &buffer)
{
  PeakMasterCompatibility::save(path, buffer.model(), buffer.model());
}

} // namespace persistence
