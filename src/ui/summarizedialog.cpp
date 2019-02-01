#include "summarizedialog.h"
#include "ui_summarizedialog.h"

#include <summary/commonoptions.h>
#include <summary/exception.h>
#include <QMessageBox>
#include <cassert>

SummarizeDialog::SummarizeDialog(const gearbox::Gearbox &gbox, const std::vector<summary::Info> &sumInfo,
                                 QWidget *parent) :
  QDialog{parent},
  ui{new Ui::SummarizeDialog},
  m_summarizer{nullptr},
  m_specOpts{nullptr},
  h_gbox{gbox}
{
  ui->setupUi(this);

  assert(sumInfo.size() > 0);

  for (const auto &info : sumInfo)
    ui->qcbox_summarizers->addItem(QString::fromStdString(info.name), info.id);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SummarizeDialog::onSummarize);
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

summary::CommonOptions SummarizeDialog::makeCommonOptions()
{
  static const auto chk = [](QCheckBox *cb) {
    return cb->checkState() == Qt::Checked;
  };

  return {
    chk(ui->qcb_abbreviateBuffers),
    chk(ui->qcb_includeBuffers),
    chk(ui->qcb_includeCurve),
    chk(ui->qcb_includeEstimates),
    chk(ui->qcb_includeIonicEffects),
    ui->qle_title->text().toStdString()
  };
}

void SummarizeDialog::onOptionsClicked()
{
  assert(m_summarizer != nullptr);

  m_specOpts = m_summarizer->options();
}

void SummarizeDialog::onSummarize()
{
  const auto path = ui->qle_outputFile->text();

  if (path.isEmpty()) {
    QMessageBox mbox{QMessageBox::Warning, tr("Invalid options"), tr("No output path was specified")};
    mbox.exec();
    return;
  }

  try {
    m_summarizer->summarize(h_gbox, makeCommonOptions(), m_specOpts, path.toStdString());

    QMessageBox mbox{QMessageBox::Information, tr("Success"), tr("Summary was succesfully created")};
    mbox.exec();
  } catch (const summary::NoDefaultOptionsException &) {
    QMessageBox mbox{QMessageBox::Warning, tr("Invalid options"), tr("Specific options must be configured for the selected summarizer")};
    mbox.exec();
  }
}

void SummarizeDialog::onSummarizerChanged(const int idx)
{
  QVariant v = ui->qcbox_summarizers->itemData(idx);
  auto smrType = v.value<summary::SummarizerFactory::Types>();

  m_summarizer = summary::SummarizerFactory::make(smrType);
  m_specOpts = nullptr;
  assert(m_summarizer != nullptr);
}
