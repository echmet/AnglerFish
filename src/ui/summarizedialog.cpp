#include "summarizedialog.h"
#include "ui_summarizedialog.h"

#include <summary/commonoptions.h>
#include <summary/exception.h>
#include <QFileDialog>
#include <QFileInfo>
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
  assert(sumInfo.size() > 0);

  ui->setupUi(this);

  m_browseDlg = new QFileDialog{this, tr("Pick summary destination")};
  m_browseDlg->setAcceptMode(QFileDialog::AcceptSave);

  for (const auto &info : sumInfo)
    ui->qcbox_summarizers->addItem(QString::fromStdString(info.name), info.id);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SummarizeDialog::onSummarize);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SummarizeDialog::close);
  connect(ui->qcb_includeBuffers, &QCheckBox::clicked, this, [this]() {
    ui->qcb_abbreviateBuffers->setEnabled(ui->qcb_includeBuffers->checkState() == Qt::Checked);
  });
  connect(ui->qpb_options, &QPushButton::clicked, this, &SummarizeDialog::onOptionsClicked);
  connect(ui->qpb_browse, &QPushButton::clicked, this, &SummarizeDialog::onBrowseClicked);

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

void SummarizeDialog::onBrowseClicked()
{
  QFileInfo finfo{ui->ql_outputFile->text()};
  if (finfo.exists())
    m_browseDlg->setDirectory(finfo.dir().absolutePath());

  if (m_browseDlg->exec() == QDialog::Accepted) {
    const auto &sel = m_browseDlg->selectedFiles();

    if (!sel.empty())
      ui->qle_outputFile->setText(sel.first());
  }
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
  assert(m_summarizer != nullptr);
  m_specOpts = nullptr;

  const auto &ftypes = m_summarizer->allowedFileTypes();
  QStringList filters{};
  for (const auto &t : ftypes) {
    QString suff{};

    for (const auto &s: t.suffix)
      suff.append(QString{"*.%1 "}.arg(s.c_str()));

    QString filter = QString{"%1 (%2)"}.arg(t.description.c_str(), suff);
    filters.push_back(std::move(filter));
  }

  m_browseDlg->setNameFilters(std::move(filters));
}
