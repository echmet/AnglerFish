#include "enterexperimentalphdialog.h"
#include "ui_enterexperimentalphdialog.h"

#include <QMessageBox>

EnterExperimentalpHDialog::EnterExperimentalpHDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EnterExperimentalpHDialog),
  m_pH{0.0}
{
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &EnterExperimentalpHDialog::reject);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
    if (ui->qle_pH->isInputValid()) {
      m_pH = ui->qle_pH->numericValue();
      this->accept();
    } else {
      QMessageBox mbox{QMessageBox::Warning, tr("Invalid input"), tr("Please enter a valid pH value")};
      mbox.exec();
    }
  });
}

double EnterExperimentalpHDialog::pH() const
{
  return m_pH;
}

EnterExperimentalpHDialog::~EnterExperimentalpHDialog()
{
  delete ui;
}
