#include "summarizedialog.h"
#include "ui_summarizedialog.h"

#include <cassert>

SummarizeDialog::SummarizeDialog(const std::vector<summary::Info> &sumInfo, QWidget *parent) :
  QDialog{parent},
  ui{new Ui::SummarizeDialog},
  m_summarizer{nullptr},
  m_specOpts{nullptr}
{
  ui->setupUi(this);

  assert(sumInfo.size() > 0);

  for (const auto &info : sumInfo)
    ui->qcbox_summarizers->addItem(QString::fromStdString(info.name), info.id);

  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SummarizeDialog::close);
  connect(ui->qcb_includeBuffers, &QCheckBox::clicked, this, [this]() {
    ui->qcb_abbreviateBuffers->setEnabled(ui->qcb_includeBuffers->checkState() == Qt::Checked);
  });
  connect(ui->qpb_options, &QPushButton::clicked, this, &SummarizeDialog::onOptionsClicked);

  ui->qcb_abbreviateBuffers->setEnabled(ui->qcb_includeBuffers->checkState() == Qt::Checked);

  onSummarizerChanged(ui->qcbox_summarizers->currentIndex());
}

SummarizeDialog::~SummarizeDialog()
{
  delete ui;
}

void SummarizeDialog::onOptionsClicked()
{
  assert(m_summarizer != nullptr);

  m_specOpts = m_summarizer->options();
}

void SummarizeDialog::onSummarizerChanged(const int idx)
{
  QVariant v = ui->qcbox_summarizers->itemData(idx);
  auto smrType = v.value<summary::SummarizerFactory::Types>();

  m_summarizer = summary::SummarizerFactory::make(smrType);
  m_specOpts = nullptr;
  assert(m_summarizer != nullptr);
}
