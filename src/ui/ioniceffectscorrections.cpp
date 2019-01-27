#include "ioniceffectscorrections.h"
#include "ui_ioniceffectscorrections.h"

#include <gearbox/ioniceffectsmodel.h>

namespace gearbox {

IonicEffectsCorrections::IonicEffectsCorrections(gearbox::IonicEffectsModel &ionEffs, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::IonicEffectsCorrections},
  h_ionEffs{ionEffs}
{
  ui->setupUi(this);

  ui->qcb_debyeHuckel->setChecked(h_ionEffs.debyeHuckel());
  ui->qcb_onsagerFuoss->setChecked(h_ionEffs.onsagerFuoss());

  connect(ui->buttonBox, &QDialogButtonBox::accepted, [this]() {
    h_ionEffs.setDebyeHuckel(ui->qcb_debyeHuckel->isChecked());
    h_ionEffs.setOnsagerFuoss(ui->qcb_onsagerFuoss->isChecked());
  });
}

IonicEffectsCorrections::~IonicEffectsCorrections()
{
  delete ui;
}

} // namespace gearbox
