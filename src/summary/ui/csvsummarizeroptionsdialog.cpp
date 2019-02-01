#include "csvsummarizeroptionsdialog.h"
#include "ui_csvsummarizeroptionsdialog.h"

CSVSummarizerOptionsDialog::CSVSummarizerOptionsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CSVSummarizerOptionsDialog)
{
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CSVSummarizerOptionsDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &CSVSummarizerOptionsDialog::reject);
}

CSVSummarizerOptionsDialog::~CSVSummarizerOptionsDialog()
{
  delete ui;
}

std::tuple<std::string, bool> CSVSummarizerOptionsDialog::options() const
{
  return { ui->qle_delimiter->text().toStdString(), ui->qcb_experimentalpH->checkState() == Qt::Checked };
}

void CSVSummarizerOptionsDialog::setOptions(const char delimiter, const bool experimentalpH)
{
  ui->qle_delimiter->setText(QString{delimiter});
  ui->qcb_experimentalpH->setChecked(experimentalpH);
}
