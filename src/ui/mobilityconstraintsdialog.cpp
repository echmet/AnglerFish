#include "mobilityconstraintsdialog.h"
#include "ui_mobilityconstraintsdialog.h"

#include <gearbox/limitmobilityconstraintsmodel.h>

MobilityConstraintsDialog::MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::MobilityConstraintsDialog},
  h_model{model}
{
  ui->setupUi(this);

  ui->qcb_useMobilityConstraints->setChecked(h_model.enabled());

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          [this]() {
            h_model.setEnabled(ui->qcb_useMobilityConstraints->checkState() == Qt::Checked);
            accept();
         });
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          [this]() {
            ui->qcb_useMobilityConstraints->setChecked(h_model.enabled());
            reject();
         });
}

MobilityConstraintsDialog::~MobilityConstraintsDialog()
{
  delete ui;
}
