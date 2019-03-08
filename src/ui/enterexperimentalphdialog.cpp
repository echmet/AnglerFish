#include "enterexperimentalphdialog.h"
#include "ui_enterexperimentalphdialog.h"

#include <QMessageBox>

EnterExperimentalpHDialog::EnterExperimentalpHDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EnterExperimentalpHDialog)
{
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &EnterExperimentalpHDialog::reject);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
    if (ui->qle_pH->isInputValid())
      this->accept();
    else {
      QMessageBox mbox{QMessageBox::Warning, tr("Invalid input"), tr("Please enter a valid pH value")};
      mbox.exec();
    }
  });
}

EnterExperimentalpHDialog::~EnterExperimentalpHDialog()
{
  delete ui;
}
