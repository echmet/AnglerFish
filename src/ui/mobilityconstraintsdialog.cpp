#include "mobilityconstraintsdialog.h"
#include "ui_mobilityconstraintsdialog.h"
#include "internal_models/mobilityconstraintsmodel.h"

#include <gearbox/gearbox.h>
#include <gearbox/limitmobilityconstraintsmodel.h>

MobilityConstraintsDialog::MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model,
                                                     const gearbox::Gearbox &gbox,
                                                     QWidget *parent) :
  QDialog{parent},
  ui{new Ui::MobilityConstraintsDialog},
  h_model{model},
  h_gbox{gbox}
{
  ui->setupUi(this);

  m_uiModel = new MobilityConstraintsModel{model, this->palette(), this};
  ui->qtvb_currentConstraints->setModel(m_uiModel);

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

void MobilityConstraintsDialog::onEstimatesChanged()
{
  const auto estimates = h_gbox.analyteEstimates();

  int charge = estimates.chargeLow;

  QVector<MobilityConstraintsModel::EstimatedMobility> estMobs{};
  for (const auto &mob : estimates.mobilities)
    estMobs.push_front(MobilityConstraintsModel::EstimatedMobility{charge++, mob.value});

  m_uiModel->updateConstraints(estimates.chargeLow, estimates.chargeHigh, estMobs);
}
