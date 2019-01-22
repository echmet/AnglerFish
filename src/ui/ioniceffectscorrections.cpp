#include "ioniceffectscorrections.h"
#include "ui_ioniceffectscorrections.h"

#include <gearbox/gearbox.h>

IonicEffectsCorrections::IonicEffectsCorrections(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::IonicEffectsCorrections)
{
  ui->setupUi(this);

  const auto &model = Gearbox::instance()->ionicEffectsModel();

  ui->qcb_debyeHuckel->setChecked(model.debyeHuckel());
  ui->qcb_onsagerFuoss->setChecked(model.onsagerFuoss());

  connect(ui->buttonBox, &QDialogButtonBox::accepted, [this]() {
    auto &_model = Gearbox::instance()->ionicEffectsModel();

    _model.setDebyeHuckel(ui->qcb_debyeHuckel->isChecked());
    _model.setOnsagerFuoss(ui->qcb_onsagerFuoss->isChecked());
  });
}

IonicEffectsCorrections::~IonicEffectsCorrections()
{
  delete ui;
}
