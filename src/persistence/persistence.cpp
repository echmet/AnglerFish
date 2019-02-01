#include "persistence.h"

#include "entiresetup.h"
#include "peakmastercompatibility.h"

#include <gearbox/gearbox.h>

namespace persistence {

void loadEntireSetup(const QString &path, gearbox::Gearbox &gbox)
{
  auto loaded = EntireSetup::load(path, gbox.ionicEffectsModel());

  gbox.chemicalBuffersModel().setBuffers(std::move(std::get<0>(loaded)));
  gbox.setAnalyteEstimates(std::move(std::get<1>(loaded)));
}

void saveEntireSetup(const QString &path, const gearbox::ChemicalBuffersModel &buffers,
                     const gearbox::AnalyteEstimates &analyte)
{
  EntireSetup::save(path, buffers, analyte);
}

void loadPeakMasterBuffer(const QString &path, gearbox::Gearbox &gbox)
{
  auto model = PeakMasterCompatibility::load(path);

  auto newModel = new gdm::GDM{std::move(model)};

  gbox.chemicalBuffersModel().add(gearbox::ChemicalBuffer{&gbox.ionicEffectsModel(), newModel});
}

void savePeakMasterBuffer(const QString &path, const gearbox::ChemicalBuffer &buffer)
{
  PeakMasterCompatibility::save(path, buffer.model(), buffer.model());
}

} // namespace persistence
