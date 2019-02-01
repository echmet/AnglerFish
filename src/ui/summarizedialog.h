#ifndef SUMMARIZEDIALOG_H
#define SUMMARIZEDIALOG_H

#include <summary/summarizerfactory.h>

#include <QDialog>

namespace Ui {
  class SummarizeDialog;
}

class SummarizeDialog : public QDialog {
  Q_OBJECT
public:
  explicit SummarizeDialog(const std::vector<summary::Info> &sumInfo, QWidget *parent = nullptr);
  ~SummarizeDialog();

private:
  Ui::SummarizeDialog *ui;

  summary::AbstractSummarizerPtr m_summarizer;
  summary::SpecificOptionsPtr m_specOpts;

private slots:
  void onOptionsClicked();
  void onSummarizerChanged(const int idx);
};

#endif // SUMMARIZEDIALOG_H
