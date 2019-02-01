#ifndef SUMMARIZEDIALOG_H
#define SUMMARIZEDIALOG_H

#include <summary/summarizerfactory.h>

#include <QDialog>

class QFileDialog;

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace Ui {
  class SummarizeDialog;
}

class SummarizeDialog : public QDialog {
  Q_OBJECT
public:
  explicit SummarizeDialog(const gearbox::Gearbox &gbox, const std::vector<summary::Info> &sumInfo,
                           QWidget *parent = nullptr);
  ~SummarizeDialog();

private:
  summary::CommonOptions makeCommonOptions();

  Ui::SummarizeDialog *ui;

  QFileDialog *m_browseDlg;

  summary::AbstractSummarizerPtr m_summarizer;
  summary::SpecificOptionsPtr m_specOpts;

  const gearbox::Gearbox &h_gbox;

private slots:
  void onBrowseClicked();
  void onOptionsClicked();
  void onSummarize();
  void onSummarizerChanged(const int idx);
};

#endif // SUMMARIZEDIALOG_H
