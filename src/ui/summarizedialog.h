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
};

#endif // SUMMARIZEDIALOG_H
