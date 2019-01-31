#include "summarizedialog.h"
#include "ui_summarizedialog.h"

SummarizeDialog::SummarizeDialog(const std::vector<summary::Info> &sumInfo, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::SummarizeDialog}
{
  ui->setupUi(this);

  for (const auto &info : sumInfo)
    ui->qcbox_summarizers->addItem(QString::fromStdString(info.name), info.id);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SummarizeDialog::close);
}

SummarizeDialog::~SummarizeDialog()
{
  delete ui;
}
