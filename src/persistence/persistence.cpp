#include "persistence.h"

#include "peakmastercompatibility.h"

#include <gearbox/gearbox.h>

namespace persistence {

void loadPeakMasterBuffer(const QString &path)
{
  auto model = PeakMasterCompatibility::load(path);

  auto gbox = Gearbox::instance();
  auto newModel = new gdm::GDM{std::move(model)};

  gbox->chemicalBuffersModel().add(ChemicalBuffer{newModel});
}

void savePeakMasterBuffer(const QString &path, const ChemicalBuffer &buffer)
{
  PeakMasterCompatibility::save(path, buffer);
}

} // namespace persistence
