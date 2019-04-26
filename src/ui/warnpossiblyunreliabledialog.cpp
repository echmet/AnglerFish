#include "warnpossiblyunreliabledialog.h"
#include "ui_warnpossiblyunreliabledialog.h"
#include "../globals.h"

WarnPossiblyUnreliableDialog::WarnPossiblyUnreliableDialog(const int maxOkChargeAbs, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::WarnPossiblyUnreliableDialog}
{
  ui->setupUi(this);

  ui->ql_message->setText(
    tr(
      "<html><head/><body>"
      "<p>"
      "<span style=\"color:#ff0000;\">WARNING: </span>"
      "Analyte you are attempting to fit has charge states that are considered "
      "outside of the scope where the model used by this software is valid. "
      "You are advised to treat the results as unreliable."
      "</p>"
      "</body></html>"
    )
  );

  ui->ql_maxOkCharge->setText(QString{tr("Maximum absolute value of charge for which can %1 "
                                         "calculate reliable results is %2")}.arg(Globals::SOFTWARE_NAME).arg(maxOkChargeAbs)
                             );
}

WarnPossiblyUnreliableDialog::~WarnPossiblyUnreliableDialog()
{
  delete ui;
}

bool WarnPossiblyUnreliableDialog::dontShowAgain() const
{
  return ui->qcb_dontShowAgain->checkState() == Qt::Checked;
}
