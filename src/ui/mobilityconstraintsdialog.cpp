#include "mobilityconstraintsdialog.h"
#include "ui_mobilityconstraintsdialog.h"
#include "internal_models/mobilityconstraintsmodel.h"

#include <gearbox/gearbox.h>
#include <gearbox/limitmobilityconstraintsmodel.h>
#include <gearbox/floatingvaluedelegate.h>

MobilityConstraintsDialog::MobilityConstraintsDialog(gearbox::LimitMobilityConstraintsModel &model,
                                                     gearbox::Gearbox &gbox,
                                                     QWidget *parent) :
  QDialog{parent},
  ui{new Ui::MobilityConstraintsDialog},
  h_model{model},
  h_gbox{gbox}
{
  ui->setupUi(this);

  m_fltDelegate = new gearbox::FloatingValueDelegate{this};

  m_uiModel = new MobilityConstraintsModel{model, h_gbox, this->palette(), this};
  ui->qtvb_currentConstraints->setModel(m_uiModel);
  ui->qtvb_currentConstraints->setItemDelegateForColumn(0, m_fltDelegate);

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
