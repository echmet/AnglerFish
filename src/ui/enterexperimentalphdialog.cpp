#include "enterexperimentalphdialog.h"
#include "ui_enterexperimentalphdialog.h"

#include <QMessageBox>

EnterExperimentalpHDialog::EnterExperimentalpHDialog(const QString &name, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EnterExperimentalpHDialog),
  m_pH{0.0}
{
  ui->setupUi(this);

  ui->ql_ctuentInUse->setText(QString(tr(
      "<html><head/><body>"
      "Concentration of <strong>%1</strong> will be adjusted."
      "</body></html>")).arg(name));

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
